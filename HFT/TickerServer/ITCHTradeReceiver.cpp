// g++ -std=c++20 ITCHTradeReceiver.cpp -o ITCHTradeReceiver -I../

#include <thread>
#include <chrono>
#include <functional>

#include "Socket.hpp"
#include "Queue.hpp"
#include "HashMap.hpp"
#include "MemoryPool.hpp"

#include "ITCHMessages.hpp"

namespace Config {
    constexpr std::string multicastIP = "239.255.0.1";
    constexpr int multicastPort = 30001;
    constexpr int multicastThrottle_us = 100;

    const std::string recoveryIP = "127.0.0.1";
    constexpr int recoveryPort = 8080;
    constexpr int maxSnapshotEvents = 100;
};

/**************************************************************************/
class IThreadRunner {
public:
    virtual ~IThreadRunner() {}
    virtual void run() = 0;
    void stop() { runFlag.store(false); }
protected:
    alignas(64) std::atomic<bool> runFlag{true};
};

/**************************************************************************/
template <typename TradeMsg, MyPool Pool>
class TradeRecoveryManager {
public:
    using TradeMsgPtr = TradeMsg*;
    using SequencerOnMsgCB = std::function<void(TradeMsgPtr)>;
    TradeRecoveryManager(SequencerOnMsgCB cb, Pool& pool) 
            : sequencerOnMsgCB_(std::move(cb))
            , msgPool_(pool) {
        
    }
    void connect() {
        socketFD_ = Socket(AF_INET, SOCK_STREAM, 0);
        if (socketFD_.get() < 0) 
            throw std::runtime_error("Failed to create TradeRecoveryManager socket");

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(Config::recoveryPort);
        inet_pton(AF_INET, Config::recoveryIP.c_str(), &addr.sin_addr);

        if (::connect(socketFD_.get(), (sockaddr*)&addr, sizeof(addr)) < 0)
            throw std::runtime_error("Failed to connect at TradeRecoveryManager");
        
        int flags = fcntl(socketFD_.get(), F_GETFL, 0);
        fcntl(socketFD_.get(), F_SETFL, flags | O_NONBLOCK);
    }
    void recover(uint64_t startSeq, uint64_t endSeq) {
        sendRecoveryRequest(startSeq, endSeq);
        receiveRecoveryMessages(startSeq, endSeq);
    }
private:
    void sendRecoveryRequest(const uint64_t startSeq, const uint64_t endSeq) {
        ITCHGapRequestMsg req{'0', startSeq, endSeq};
        ssize_t sent = send(socketFD_.get(), &req, sizeof(req), 0);
        if (sent != sizeof(req))
            throw std::runtime_error("Failed to send full recovery request at sendRecoveryRequest");
    }
    void receiveRecoveryMessages(uint64_t startSeq, uint64_t endSeq) {
        Socket epollFD(epoll_create1(0));
        if (epollFD.get() < 0) 
            throw std::runtime_error("epoll_create1() failed at receiveRecoveryMessages");

        epoll_event ev{}, event{};
        ev.events = EPOLLIN;
        ev.data.fd = socketFD_.get();

        if (epoll_ctl(epollFD.get(), EPOLL_CTL_ADD, socketFD_.get(), &ev) < 0)
            throw std::runtime_error("epoll_ctl() failed at receiveRecoveryMessages");

        uint64_t messagesReceived = 0;
        const uint64_t numMessages = endSeq - startSeq + 1;

        while (messagesReceived < numMessages) {
            int nfds = epoll_wait(epollFD.get(), &event, 1, 5000);  // 5s timeout
            if (nfds > 0 && event.events & EPOLLIN) [[likely]] {
                TradeMsgPtr msg = msgPool_.allocate();
                ssize_t bytes = recv(socketFD_.get(), msg, ITCHTradeMsgSize, MSG_WAITALL);
                if (bytes != ITCHTradeMsgSize) [[unlikely]] {
                    msgPool_.deallocate(msg);
                    if (bytes <= 0) {
                        std::cerr << "Connection closed or error\n";
                        break;
                    } 
                    else {
                        std::cerr << "Partial message received (" << bytes << " bytes)\n";
                    }
                    continue;
                }
                sequencerOnMsgCB_(msg);
                ++messagesReceived;
                std::cout << "Received message #" << messagesReceived << "\n";
            } 
            else if (nfds == 0) {
                std::cerr << "Timeout waiting for data at receiveRecoveryMessages\n";
                continue;
            } 
            else {
                std::cerr << "Recovery Server Not responded at receiveRecoveryMessages\n";
                break;
            }
        }
    }
    SequencerOnMsgCB sequencerOnMsgCB_;
    Pool& msgPool_;
    Socket socketFD_{-1};
};

/**************************************************************************/
template <typename TradeMsg, MyQ RecvMsgQueue, MyQ SendMsgQueue, MyPool Pool>
class TradeDataSequencer : public IThreadRunner {
public:
    using TradeMsgPtr = TradeMsg*;

    TradeDataSequencer(RecvMsgQueue& recvQueue, SendMsgQueue& sendQueue, Pool& pool) 
            : recvQueue_(recvQueue)
            , sendQueue_(sendQueue)
            , msgPool_(pool)
            , tradeRecoveryManager_([this](TradeMsgPtr msg) { onRecoveredMsg(msg); }, pool) {
        tradeRecoveryManager_.connect();
    }
    ~TradeDataSequencer() override {
        
    }

    void run() override {
        while (runFlag.load(std::memory_order_relaxed)) {
            TradeMsgPtr msg = recvQueue_.dequeue();
            if (!msg) {
                std::this_thread::yield();
                continue;
            }
            if (msg->sequence_number > nextSequence_) {
                // TODO : Send an invalidate message, avoid taking decisions on stale data
                tradeRecoveryManager_.recover(nextSequence_, msg->sequence_number - 1); // Blocking, required to keep the sequence
                // TODO : Not here, but send a validate message, considering some condition
            } 
            else if (msg->sequence_number < nextSequence_) { // Old message received, drop message   
                msgPool_.deallocate(msg);
                continue;
            }
            sendQueue_.enqueue(msg);
            ++nextSequence_;
        }    
    }

    void setSequenceNum(uint64_t sequence) { nextSequence_ = (sequence + 1); }
    uint64_t getSequenceNum() const { return (nextSequence_ - 1); }

private:
    void onRecoveredMsg(TradeMsgPtr msg) {
        if (msg->sequence_number != nextSequence_) {
            std::cerr << "Unrecoverable Gap [received seq: " << msg->sequence_number << "] [" <<
                "expected: " << nextSequence_ << "\n";
            throw std::runtime_error("Failed to recover message");
        }
        sendQueue_.enqueue(msg);
        ++nextSequence_;
    }
    RecvMsgQueue& recvQueue_;
    SendMsgQueue& sendQueue_;
    Pool& msgPool_;
    TradeRecoveryManager<TradeMsg, Pool> tradeRecoveryManager_;
    uint64_t nextSequence_ = 0;
};

/**************************************************************************/

int main() {
    
    LockFreeThreadSafePool<ITCHTradeMsg, true> msgPool;

    CustomSPSCLockFreeQueue<ITCHTradeMsg*> recvQueue;
    CustomSPSCLockFreeQueue<ITCHTradeMsg*> sendQueue;
    
    TradeDataSequencer<ITCHTradeMsg, 
        CustomSPSCLockFreeQueue<ITCHTradeMsg*>, 
        CustomSPSCLockFreeQueue<ITCHTradeMsg*>,
        LockFreeThreadSafePool<ITCHTradeMsg, true>> tradeSequencer(recvQueue, sendQueue, msgPool);
    
    std::thread seqThread([&] { tradeSequencer.run(); });
}