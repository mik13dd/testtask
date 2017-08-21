#pragma once
#include "consumer_interface.hpp"
#include "boost/lockfree/queue.hpp"
#include "report.h"

#include <memory>

const int MAX_QUEUE_SIZE = 1000;

// Class for processing one single queue. Performs nonblocking
// processing in separate thread.
template<typename Key, typename Value>
class SingleQueueProcessor : public BaseThread {
public:
  using ConsumerPtr = std::shared_ptr< IConsumer<Key, Value> >;
  SingleQueueProcessor(const Key &key, const ConsumerPtr &consumer) 
      : consumer_(consumer)
      , queue_(MAX_QUEUE_SIZE)
      , key_(key) {}

  void Enqueue(const Value &val) {
    queue_.push(val);
  }

  bool Dequeue(Value &val) {
    return queue_.pop(val);
  }

  bool Consume() {
    Value val;
    bool res = Dequeue(val);
    if (res) {
      consumer_->Consume(key_, val);
    }
    return res;
  }

private:
  void Execute() override {
    while (Consume()) {
      if (!is_running_.load())
        break;
    };
  }

  ConsumerPtr consumer_;
  boost::lockfree::queue<Value, boost::lockfree::fixed_sized<true>> queue_;
  Key key_;
};