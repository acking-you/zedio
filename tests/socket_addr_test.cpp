#include "zedio/socket/net/addr.hpp"

#include <algorithm> // for std::transform
#include <boost/ut.hpp>

using namespace zedio::socket::net;
using namespace boost::ut;

void address_tests() {
    "ipv4_test"_test = [] {
        // Test IPv4 address parsing and properties
        {
            auto ip = Ipv4Addr::parse("192.168.15.33").value();
            auto addr = SocketAddr{ip, 9999};

            expect(addr.is_ipv4() == true);
            expect(addr.is_ipv6() == false);
            expect(addr.port() == 9999u);
            expect(addr.length() == sizeof(sockaddr_in));
            expect(addr.to_string() == "192.168.15.33:9999");
            expect(std::get<0>(addr.ip()).to_string() == "192.168.15.33");

            // Test socket address construction from raw sockaddr
            SocketAddr tmp(addr.sockaddr(), addr.length());
            expect(tmp.is_ipv4() == true);
            expect(tmp.is_ipv6() == false);
            expect(tmp.port() == 9999u);
            expect(tmp.to_string() == "192.168.15.33:9999");
            expect(tmp.length() == sizeof(sockaddr_in));
            expect(std::get<0>(tmp.ip()).to_string() == ip.to_string());
        }

        // Test IPv4 address construction from integers
        {
            auto ip = Ipv4Addr(192, 168, 15, 33);
            auto addr = SocketAddr{ip, 9999};

            expect(addr.is_ipv4() == true);
            expect(addr.is_ipv6() == false);
            expect(addr.port() == 9999u);
            expect(addr.length() == sizeof(sockaddr_in));
            expect(addr.to_string() == "192.168.15.33:9999");
            expect(std::get<0>(addr.ip()).to_string() == "192.168.15.33");
        }

        // Test resolution of localhost
        {
            auto addr = SocketAddr::parse("localhost", 21).value();

            expect(addr.is_ipv4() == true);
            expect(addr.is_ipv6() == false);
            expect(addr.port() == 21u);
            expect(std::get<0>(addr.ip()).to_string() == "127.0.0.1");
        }
    };

    "ipv6_test"_test = [] {
        // Test IPv6 address parsing and properties
        {
            std::string str = "96CC:F03C:ED86:D0FC:C2BB:8696:7EA5:3D79";
            std::transform(str.begin(), str.end(), str.begin(), tolower);
            auto ip = Ipv6Addr::parse(str).value();

            auto addr = SocketAddr{ip, 1234};

            expect(addr.is_ipv4() == false);
            expect(addr.is_ipv6() == true);
            expect(addr.port() == 1234u);
            expect(std::get<1>(addr.ip()).to_string() == ip.to_string());
            expect(addr.to_string() == std::string("[") + str + "]:1234");
            expect(addr.length() == sizeof(sockaddr_in6));

            // Test socket address construction from raw sockaddr
            SocketAddr tmp(addr.sockaddr(), addr.length());
            expect(tmp.is_ipv4() == false);
            expect(tmp.is_ipv6() == true);
            expect(tmp.port() == 1234u);
            expect(std::get<1>(tmp.ip()).to_string() == ip.to_string());
            expect(tmp.to_string() == std::string("[") + str + "]:1234");
            expect(tmp.length() == sizeof(sockaddr_in6));
        }

        // Test IPv6 address construction from integers
        {
            auto ip = Ipv6Addr(0x96cc, 0xf03c, 0xed86, 0xd0fc, 0xc2bb, 0x8696, 0x7ea5, 0x3d79);
            auto addr = SocketAddr{ip, 1234};

            expect(addr.is_ipv4() == false);
            expect(addr.is_ipv6() == true);
            expect(addr.port() == 1234u);
            expect(std::get<1>(addr.ip()).to_string() == ip.to_string());
            expect(addr.to_string() == "[96cc:f03c:ed86:d0fc:c2bb:8696:7ea5:3d79]:1234");
            expect(addr.length() == sizeof(sockaddr_in6));
        }
    };
}

int main() {
    address_tests();
    return 0;
}