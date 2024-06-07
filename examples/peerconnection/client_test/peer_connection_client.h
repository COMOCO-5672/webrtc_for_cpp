#ifndef EXAMPLES_PEERCONNECTION_CLIENT_PEER_CONNECTION_CLIENT_H_
#define EXAMPLES_PEERCONNECTION_CLIENT_PEER_CONNECTION_CLIENT_H_

#include <map>
#include <memory>
#include <string>

#include "api/async_dns_resolver.h"
#include "api/task_queue/pending_task_safety_flag.h"
#include "rtc_base/net_helpers.h"
#include "rtc_base/physical_socket_server.h"
#include "rtc_base/third_party/sigslot/sigslot.h"

typedef std::map<int, std::string> Peers;

struct PeerConnectionClientObserver {
    virtual void OnSignedIn() = 0;
    virtual void OnDisconnected() = 0;
    virtual void OnPeerConnected(int id, const std::string& name) = 0;
    virtual void OnPeerDisconnected(int peer_id) = 0;
    virtual void OnMessageFromPeer(int peer_id, const std::string& message) = 0;
    virtual void OnMessageSent(int err) = 0;
    virtual void OnServerConnectionFailure() = 0;

 protected:
    virtual ~PeerConnectionClientObserver() {}
}

class PeerConnectionClient : public sigslot::has_slots<> {
    public:
    enum State {
        NOT_CONNECTED,
        RESOLVING,
        SIGNING_IN,
        CONNECTED,
        SIGNING_OUT_WAITING,
        SIGNING_OUT,
    };

    PeerConnectionClient();
    ~PeerConnectionClient();

    int id() const;
    bool is_connected() const;
    const Peers& peers() const;

    void RegisterObserver(PeerConnectionClientObserver* callback);    

    void Connect(const std::string& server
                , int port
                , const std::string& client_name);
    bool SendToPeer(int peer_id, const std::string& message);
    bool SendHangUp(int peer_id);
    bool IsSendingMessage();

    bool SignOut();

    protected:

    void DoConnect();
    void Close();
    void InitSocketSignals();
    void ConnectControlSocket();
    void OnConnect(rtc::Socket* socket);
    void OnHangingGetConnect();
    void OnMessageFromPeer(int peer_id, const std::string& message);

    bool GetHeaderValue(const std::string& data
                        , size_t eoh
                        , const char* header_pattern
                        , size_t *value);
    
    bool GetHeaderValue(const std::string& data
                        , size_t eoh
                        , const char* header_pattern
                        , std::string* value);
    bool ReadIntoBuffer(rtc::Socket* socket
                        , std::string *data
                        , size_t* content_length);

    void OnRead(rtc::Socket* socket);

    void OnHangingGetRead(rtc::Socket* socket) ;

    bool ParseEntry(const std::string& entry, std::string *name, int* id, bool* connected);

    int GetResponseStatus(const std::string& response);

    bool ParseServerResponse(const std::string& response
                            , size_t content_length
                            , size_t* peer_id
                            , size_t* eoh);

    void OnClose(rtc::Socket* socket, int err);

    void OnResolveResult(const webrtc::AsyncDnsResolverResult& result);

    PeerConnectionClientObserver* callback_;
    rtc::SocketAddress server_address_;
    std::unique_ptr<webrtc::AsyncDnsResolverInterface> resolver_;
    std::unique_ptr<rtc::Socket> control_socket_;
    std::unique_ptr<rtc::Socket> hanging_get_;

    std::string onconnect_data_;
    std::string control_data_;
    std::string notification_data_;
    std::string client_name_;
    Peers peers_;
    State state_;
    
    int my_id_;
    webrtc::ScopedTaskSafety safety_;
};

#endif