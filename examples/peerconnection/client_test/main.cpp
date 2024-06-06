#include "media/base/media_engine.h"
#include "rtc_base/ssl_adapter.h"
#include "rtc_base/win32_socket_init.h"
#include "rtc_base/win32_socket_server.h"
#include "rtc_base/thread.h"



int main() {

    rtc::InitializeSSL();
    rtc::Win32SocketInitializer socket_init;

    auto media_engine = webrtc::MediaEngine::Create();

    rtc::SSLAdapter::Create();

    rtc::SocketServer w32_ss;
    rtc::AsyncSocket w32_socket(&w32_ss);

    rtc::Win32Thread w32_thread;



    return 0;
}
