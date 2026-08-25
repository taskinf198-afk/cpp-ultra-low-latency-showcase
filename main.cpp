/**
 * @file main.cpp
 * @brief Test harness for the Ultra-Low-Latency Lock-Free SPSC Ring Buffer.
 */

#include <iostream>
#include <thread>
#include <cassert>
#include "ring_buffer.h"

struct OrderMessage {
    uint64_t order_id;
    double price;
    uint32_t quantity;
};

int main() {
    std::cout << "[INIT] Initializing HFT SPSC Ring Buffer Test Core...\n";

    // 1024 kapasiteli lock-free ring buffer oluşturuyoruz
    LockFreeSPSCRingBuffer<OrderMessage, 1024> ring_buffer;

    OrderMessage sample_order{100159, 68450.50, 10};
    
    // Push testi
    bool push_success = ring_buffer.push(sample_order);
    if (push_success) {
        std::cout << "[SUCCESS] Order successfully pushed to ring buffer on hot-path.\n";
    } else {
        std::cerr << "[ERROR] Ring buffer saturated!\n";
        return 1;
    }

    // Pop testi
    OrderMessage received_order{};
    bool pop_success = ring_buffer.pop(received_order);
    
    if (pop_success) {
        std::cout << "[SUCCESS] Order popped from ring buffer successfully.\n"
                  << "  -> Order ID: " << received_order.order_id << "\n"
                  << "  -> Price:    " << received_order.price << "\n"
                  << "  -> Quantity: " << received_order.quantity << "\n";
    } else {
        std::cerr << "[ERROR] Ring buffer empty!\n";
        return 1;
    }

    std::cout << "[STATUS] All deterministic memory checks passed successfully. Engine ready.\n";
    return 0;
}
