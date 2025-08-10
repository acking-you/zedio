#include "zedio/common/util/ring_buffer.hpp"

#include <boost/ut.hpp>

using namespace boost::ut;

void tests() {
    constexpr std::size_t N = 100;

    "heap_ring_buffer_simple_test"_test = [] {
        zedio::util::HeapRingBuffer<std::size_t> rb(N);
        expect(rb.capacity() == N);

        // Test pushing elements to capacity
        for (std::size_t i = 1; i <= N; ++i) {
            rb.push(i);
            expect(rb.size() == i);
        }
        expect(rb.is_fill());

        // Test removing half of the elements
        for (std::size_t i = 1; i <= N / 2; ++i) {
            expect(rb.pop().value() == i);
            expect(rb.size() == N - i);
        }

        // Refill the ring buffer
        for (std::size_t i = 1; i <= N / 2; ++i) {
            rb.push(i);
            expect(rb.size() == N / 2 + i);
        }

        // Test sequential removal of all elements
        std::size_t expected = N / 2 + 1;
        while (!rb.is_empty()) {
            expect(rb.pop().value() == expected++);
            if (expected == rb.capacity() + 1) {
                expected = 1;
            }
        }
        expect(rb.is_empty());
    };

    "flex_ring_buffer_resize_test"_test = [] {
        constexpr int                    N = 5;
        zedio::util::HeapRingBuffer<int> rb(N);
        expect(rb.capacity() == N);

        // Fill the buffer
        for (int i = 1; i <= N; ++i) {
            rb.push(i);
            expect(rb.size() == static_cast<std::size_t>(i));
        }
        expect(rb.is_fill());

        // Test successful resize
        expect(rb.resize(2 * N));
        expect(rb.capacity() == 2 * N);

        // Fill the expanded buffer
        for (int i = N + 1; i <= 2 * N; ++i) {
            rb.push(i);
            expect(rb.size() == static_cast<std::size_t>(i));
        }
        expect(rb.is_fill());

        // Verify sequential order after resize
        int expected = 1;
        while (!rb.is_empty()) {
            expect(rb.pop().value() == expected++);
        }
        expect(rb.is_empty());

        // Refill the buffer
        for (int i = 1; i <= 2 * N; ++i) {
            rb.push(i);
            expect(rb.size() == static_cast<std::size_t>(i));
        }

        // Test failed resize (buffer full)
        expect(!rb.resize(N));
        expect(rb.is_fill());

        // Test removal with wrap-around
        for (int i = 1; i <= N; ++i) {
            expect(rb.pop().value() == i);
            rb.push(i);
        }
        expect(rb.is_fill());

        expected = N + 1;
        while (!rb.is_empty()) {
            expect(rb.pop().value() == expected++);
            if (expected == static_cast<int>(rb.capacity()) + 1) {
                expected = 1;
            }
        }
        expect(rb.is_empty());

        // Test resize with partial data
        rb.push(1);
        rb.push(2);
        expect(rb.resize(N));
        expect(rb.capacity() == N);
        expect(rb.size() == 2u);
        expect(rb.pop().value() == 1);
        expect(rb.pop().value() == 2);
        expect(rb.is_empty());
    };

    "stack_ring_buffer_test"_test = [] {
        zedio::util::StackRingBuffer<std::size_t, N> rb;
        expect(rb.capacity() == N);

        // Fill the stack-based buffer
        for (std::size_t i = 1; i <= N; ++i) {
            rb.push(i);
            expect(rb.size() == i);
        }
        expect(rb.is_fill());

        // Remove half of the elements
        for (std::size_t i = 1; i <= N / 2; ++i) {
            expect(rb.take() == i);
            expect(rb.size() == N - i);
        }

        // Refill the buffer
        for (std::size_t i = 1; i <= N / 2; ++i) {
            rb.push(i);
            expect(rb.size() == N / 2 + i);
        }

        // Verify sequential removal with wrap-around
        std::size_t expected = N / 2 + 1;
        while (!rb.is_empty()) {
            expect(rb.take() == expected++);
            if (expected == rb.capacity() + 1) {
                expected = 1;
            }
        }
        expect(rb.is_empty());
    };
}

int main() {
    tests();
    return 0;
}