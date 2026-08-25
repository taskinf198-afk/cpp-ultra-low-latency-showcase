/**
 * @file ring_buffer.h
 * @brief Ultra-low latency lock-free SPSC ring buffer optimized for HFT systems.
 * @author Deniz Taşkın
 */

#pragma once

#include <atomic>
#include <vector>
#include <memory>

template <typename T, size_t Capacity>
class LockFreeSPSCRingBuffer {
private:
    struct Node {
        T data;
    };

    alignas(64) std::atomic<size_t> head_{0};
    alignas(64) std::atomic<size_t> tail_{0};
    
    // Prevent false sharing with cache-line padding
    char pad_[64 - sizeof(std::atomic<size_t>) * 2];
    
    std::vector<T> buffer_;

public:
    LockFreeSPSCRingBuffer() : buffer_(Capacity) {}

    bool push(const T& item) {
        const size_t current_head = head_.load(std::memory_order_relaxed);
        const size_t current_tail = tail_.load(std::memory_order_acquire);

        if ((current_head - current_tail) >= Capacity) {
            return false; // Full
        }

        buffer_[current_head % Capacity] = item;
        head_.store(current_head + 1, std::memory_order_release);
        return true;
    }

    bool pop(T& item) {
        const size_t current_tail = tail_.load(std::memory_order_relaxed);
        const size_t current_head = head_.load(std::memory_order_acquire);

        if (current_tail == current_head) {
            return false; // Empty
        }

        item = buffer_[current_tail % Capacity];
        tail_.store(current_tail + 1, std::memory_order_release);
        return true;
    }
};
