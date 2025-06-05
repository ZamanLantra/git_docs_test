#include <iostream>
#include <asio.hpp>
#include <asio/experimental/as_tuple.hpp> // for coroutine-friendly handlers
#include <asio/awaitable.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/detached.hpp>

// For convenience
using asio::ip::udp;
using asio::awaitable;
using asio::use_awaitable;
namespace this_coro = asio::this_coro;

// Coroutine that listens for incoming UDP packets
awaitable<void> udp_market_data_listener(udp::endpoint listen_endpoint) {
    auto executor = co_await this_coro::executor;

    udp::socket socket(executor, listen_endpoint);
    std::array<char, 2048> recv_buffer;

    while (true) {
        // Asynchronously receive a packet
        auto [error, bytes_received] = co_await socket.async_receive(
            asio::buffer(recv_buffer), 
            asio::experimental::as_tuple(use_awaitable));

        if (error) {
            std::cerr << "Receive error: " << error.message() << std::endl;
            continue; // Or break/handle error
        }

        // Process the received data
        std::string_view packet(recv_buffer.data(), bytes_received);
        std::cout << "Received packet: " << packet << std::endl;

        // In real HFT, decode the packet here (e.g. ITCH/FIX parsing)
    }
}

int main() {
    try {
        asio::io_context io;

        // Set up the listening endpoint (multicast or unicast)
        udp::endpoint listen_endpoint(udp::v4(), 5000);

        // Start the coroutine
        asio::co_spawn(io, udp_market_data_listener(listen_endpoint), asio::detached);

        // Run the event loop
        io.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
