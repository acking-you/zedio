#include "zedio/socket/net/socket.hpp"

#include <boost/ut.hpp>
#include <chrono>

using namespace zedio::socket::net;
using namespace std::chrono_literals;
using namespace boost::ut;

void tcp_socket_tests() {
    "tcp_socket_api_test"_test = [] {
        // Create socket and bind to address
        auto sock_ret = TcpSocket::v4();
        expect(sock_ret.has_value());
        auto sock = std::move(sock_ret.value());
        auto addr = SocketAddr::parse("localhost", 9898).value();
        expect(sock.bind(addr).has_value());

        // Test reuseaddr setting
        {
            auto ret = sock.reuseaddr();
            if (ret) {
                log << "reuseaddr: " << ret.value();
            } else {
                log << "reuseaddr fail: " << ret.error().message();
                expect(false);
            }

            auto reuseaddr_value = ret.value();
            expect(sock.set_reuseaddr(!reuseaddr_value).has_value());

            auto new_ret = sock.reuseaddr();
            expect(new_ret.has_value() && new_ret.value() == !reuseaddr_value);

            expect(sock.set_reuseaddr(reuseaddr_value).has_value());
            expect(sock.reuseaddr().value() == reuseaddr_value);
        }

        // Test reuseport setting
        {
            auto reuseport = sock.reuseport().value();
            expect(reuseport == false);

            expect(sock.set_reuseport(true).has_value());
            expect(sock.reuseport().value() == true);

            expect(sock.set_reuseport(false).has_value());
            expect(sock.reuseport().value() == false);
        }

        // Test linger setting
        {
            auto linger = sock.linger().value();
            expect(linger == std::nullopt);

            expect(sock.set_linger(1s).has_value());
            expect(sock.linger().value().value() == 1s);

            expect(sock.set_linger(std::nullopt).has_value());
            expect(sock.linger().value() == std::nullopt);
        }

        // Test nodelay setting
        {
            auto nodelay = sock.nodelay();
            if (nodelay) {
                log << "nodelay: " << nodelay.value();
            } else {
                log << "nodelay error: " << nodelay.error().message();
                expect(false);
            }

            auto nodelay_value = nodelay.value();
            expect(sock.set_nodelay(!nodelay_value).has_value());
            expect(sock.nodelay().value() == !nodelay_value);

            expect(sock.set_nodelay(nodelay_value).has_value());
            expect(sock.nodelay().value() == nodelay_value);
        }

        // Test keepalive setting
        {
            auto keepalive = sock.keepalive();
            if (keepalive) {
                log << "keepalive: " << keepalive.value();
            } else {
                log << "keepalive error: " << keepalive.error().message();
                expect(false);
            }

            auto keepalive_value = keepalive.value();
            expect(sock.set_keepalive(!keepalive_value).has_value());
            expect(sock.keepalive().value() == !keepalive_value);

            expect(sock.set_keepalive(keepalive_value).has_value());
            expect(sock.keepalive().value() == keepalive_value);
        }
        // Prevent calling @FD::do_close from causing a null pointer due to @t_ring.
        *reinterpret_cast<int*> (&sock) = -1;
    };
}

int main() {
    tcp_socket_tests();
    return 0;
}