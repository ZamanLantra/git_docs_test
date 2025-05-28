#include <rte_eal.h>               // DPDK Environment Abstraction Layer initialization
#include <rte_ethdev.h>            // Ethernet device API for DPDK
#include <rte_mbuf.h>              // Packet buffer management
#include <iostream>                // Standard output
#include <thread>                  // For using std::thread
#include <atomic>                  // For thread-safe run flag

constexpr uint16_t PORT_ID = 0;               // Port ID to bind to (typically 0 if only one NIC is bound)
constexpr uint16_t RX_RING_SIZE = 1024;       // Size of the RX descriptor ring
constexpr uint16_t TX_RING_SIZE = 1024;       // Size of the TX descriptor ring (unused here)
constexpr uint16_t NUM_MBUFS = 8192;          // Number of packet buffers in the pool
constexpr uint16_t BURST_SIZE = 32;           // Number of packets to process in one burst

// Structure to hold mock market data
struct MarketData {
    uint64_t timestamp;     // Timestamp for received data
    double price;           // Mock price value
    uint32_t size;          // Mock trade size
};

// Simple trading strategy: print buy signal for low prices
void process_market_data(const MarketData& data) {
    if (data.price < 100.0) {
        std::cout << "[Strategy] Buy signal at price: " << data.price << std::endl;
    } else {
        std::cout << "[Strategy] Price too high: " << data.price << std::endl;
    }
}

// Mock packet parser that simulates parsing market data from packet
MarketData parse_packet(const rte_mbuf* mbuf) {
    MarketData data;
    data.timestamp = rte_rdtsc();  // Read timestamp counter (simulates tick time)
    data.price = 99.5 + (std::rand() % 1000) / 100.0; // Generate pseudo-random price
    data.size = 100 + (std::rand() % 50);            // Generate pseudo-random size
    return data;
}

// Packet receiver loop running on one thread
void receive_packets(std::atomic<bool>& run_flag) {
    rte_mbuf* bufs[BURST_SIZE];  // Buffer array to receive burst of packets

    while (run_flag.load()) {    // Continue while main thread says "run"
        uint16_t nb_rx = rte_eth_rx_burst(PORT_ID, 0, bufs, BURST_SIZE); // Receive a burst
        for (uint16_t i = 0; i < nb_rx; i++) {
            MarketData data = parse_packet(bufs[i]); // Parse mock market data
            process_market_data(data);               // Run simple strategy
            rte_pktmbuf_free(bufs[i]);               // Free the packet buffer
        }
    }
}

int main(int argc, char** argv) {
    int ret = rte_eal_init(argc, argv);      // Initialize the DPDK EAL
    if (ret < 0) rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");

    // Create memory pool for packets
    rte_mempool* mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS * 2,
                                                     0, 0, RTE_MBUF_DEFAULT_BUF_SIZE,
                                                     rte_socket_id());
    if (!mbuf_pool) rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");

    // Configure Ethernet port
    rte_eth_conf port_conf = {};
    ret = rte_eth_dev_configure(PORT_ID, 1, 0, &port_conf); // One RX queue
    if (ret < 0) rte_exit(EXIT_FAILURE, "Cannot configure device: err=%d\n", ret);

    // Setup RX queue
    ret = rte_eth_rx_queue_setup(PORT_ID, 0, RX_RING_SIZE,
                                 rte_eth_dev_socket_id(PORT_ID), nullptr, mbuf_pool);
    if (ret < 0) rte_exit(EXIT_FAILURE, "RX queue setup failed\n");

    // Start Ethernet port
    ret = rte_eth_dev_start(PORT_ID);
    if (ret < 0) rte_exit(EXIT_FAILURE, "Device start failed\n");

    std::cout << "DPDK HFT Receiver Running on port " << PORT_ID << std::endl;

    std::atomic<bool> run_flag(true);              // Flag to control RX thread
    std::thread rx_thread(receive_packets, std::ref(run_flag)); // Start RX thread

    std::this_thread::sleep_for(std::chrono::seconds(10)); // Run for 10 seconds
    run_flag = false;                          // Signal RX thread to stop
    rx_thread.join();                          // Wait for RX thread to finish

    rte_eth_dev_stop(PORT_ID);                 // Stop the Ethernet port
    rte_eth_dev_close(PORT_ID);                // Close the Ethernet port
    std::cout << "Stopped." << std::endl;

    return 0;
}
