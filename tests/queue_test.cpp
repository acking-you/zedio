#include "zedio/runtime/multi_thread/queue.hpp"

#include <atomic>
#include <coroutine>
#include <thread>
#include <vector>

#include <boost/ut.hpp>

using namespace zedio::runtime::multi_thread;
namespace ut = boost::ut;

ut::suite ring_buffer_test = [] {
    ut::test("local_queue_test") = [] {
        GlobalQueue              gq;
        LocalQueue               lq;
        std::atomic<std::size_t> cnt{0};

        // Initial state check
        ut::expect(lq.size() == 0u);
        ut::expect(ut::that % lq.empty() == true);
        ut::expect(gq.size() == 0u);
        ut::expect(ut::that % gq.empty() == true);

        // Test overflow mechanism
        auto           ele = std::noop_coroutine();
        constexpr auto len = lq.capacity() + lq.capacity() / 2 + 1;
        for (auto i = 0uz; i < len; ++i) {
            lq.push_back_or_overflow(ele, gq);
        }

        // Status check after overflow
        ut::expect(lq.size() == lq.capacity());
        ut::expect(ut::that % lq.empty() == false);
        ut::expect(gq.size() == static_cast<size_t>(lq.capacity() / 2 + 1));
        ut::expect(ut::that % gq.empty() == false);

        // Multithreaded consumption test
        std::vector<std::thread> threads;
        for (auto i = 0uz; i < 4uz; ++i) {
            threads.emplace_back([&]() {
                std::size_t num{0};
                while (lq.pop().has_value()) {
                    ++num;
                }
                cnt.fetch_add(num, std::memory_order_relaxed);
            });
        }

        // Wait for all threads to complete
        for (auto &thread : threads) {
            thread.join();
        }

        // Final result verification (compare the loaded atomic value)
        ut::expect(static_cast<std::size_t>(cnt.load()) == lq.capacity());
    };

    ut::test("global_queue_test") = [] {
        GlobalQueue gq;

        // Initial state check
        ut::expect(gq.size() == 0u);
        ut::expect(ut::that % gq.empty() == true);

        // Multithreaded production test
        auto                     ele = std::noop_coroutine();
        std::atomic<std::size_t> cnt{0};
        std::vector<std::thread> threads;
        constexpr std::size_t    num_per_thread = 512;
        constexpr std::size_t    thread_count = 4;

        for (auto i = 0uz; i < thread_count; ++i) {
            threads.emplace_back([&]() {
                for (auto i = 0uz; i < num_per_thread; ++i) {
                    gq.push(ele);
                }
            });
        }

        // Waiting for the producer to complete
        for (auto &thread : threads) {
            thread.join();
        }
        threads.clear();

        // Post-production status check
        ut::expect(ut::that % gq.empty() == false);
        ut::expect(gq.size() == num_per_thread * thread_count);

        // Multithreaded consumption test
        for (auto i = 0uz; i < thread_count; ++i) {
            threads.emplace_back([&]() {
                std::size_t local_cnt = 0;
                while (gq.pop().has_value()) {
                    ++local_cnt;
                }
                cnt.fetch_add(local_cnt, std::memory_order_relaxed);
            });
        }

        // Waiting for the consumer to complete
        for (auto &thread : threads) {
            thread.join();
        }

        // Final result verification
        // Directly compare the results of atomic value loading, without going through ut::that %
        ut::expect(cnt.load() == num_per_thread * thread_count);
        ut::expect(gq.size() == 0u);
        ut::expect(gq.empty());
    };
};

int main() {
    return ut::cfg<>.run();
}