#pragma once
#include "single_queue_processor.hpp"
#include <unordered_map>
#include <shared_mutex>


template<typename Key, typename Value>
class MultiQueueProcessorNew {
public:
  using ConsumerPtr = std::shared_ptr< IConsumer<Key, Value> >;
  
  ~MultiQueueProcessorNew() {
    StopProcessing();
  }

  void StopProcessing() {
    for (auto &it : queues_) {
      it.second->Stop();
      it.second->Wait();
    }
    queues_.clear();
  }

  void Subscribe(const Key &id, const ConsumerPtr &consumer) {
    std::unique_lock<std::shared_mutex> lock{q_mutex_};
    auto it = queues_.find(id);
    if (it == queues_.end()) {
      std::unique_ptr<SQProcessor> proc{ new SQProcessor{id, consumer} };
      queues_.insert(std::make_pair(id, std::move(proc)));
    }
  }

  void Unsubscribe(const Key &id) {
    std::unique_lock<std::shared_mutex> lock{q_mutex_};
    auto it = queues_.find(id);
    if (it != queues_.end()) {
      queues_.erase(id);
    }
  }

  void Enqueue(const Key &id, const Value &value) {
    std::shared_lock<std::shared_mutex> lock{ q_mutex_ };
    auto it = queues_.find(id);
    if (it != queues_.end()) {
      it->second->Enqueue(value);
    }
  }

  Value Dequeue(const Key &id) {
    std::shared_lock<std::shared_mutex> lock{ q_mutex_ };
    auto iter = queues.find(id);
    if (iter != queues.end()) {
      Value val;
      if (iter->second->Dequeue(val)) {
        return val;
      }
    }
    return Value{};
  }

protected:
  using SQProcessor = SingleQueueProcessor<Key, Value>;
  using Queues = std::unordered_map<Key, std::unique_ptr<SQProcessor>>;
  std::shared_mutex q_mutex_;
  Queues queues_;
};