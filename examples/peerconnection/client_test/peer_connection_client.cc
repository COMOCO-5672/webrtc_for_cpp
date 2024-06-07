#include "peer_connection_client.h"

#include "api/units/time_delta.h"
#include "defaults.h"
#include "rtc_base/async_dns_resolver.h"
#include "rtc_base/logging.h"
#include "rtc_base/checks.h"
#include "rtc_base/net_helpers.h"
#include "rtc_base/thread.h"

namespace {
    constexpr char kByeMessage[] = "BYE";
    constexpr webrtc::TimeDelta kReconnectDelay = webrtc::TimeDelta::Seconds(2);

    rtc::Socket* CreateClientSocket(int family) {
        rtc::Thread* thread = rtc::Thread::Current();
        RTC_DCHECK(thread != NULL);
        return thread->socketserver()->CreateSocket(family, SOCK_STREAM);
    }

}
