#pragma once
#include "multi_queue_processor_new.hpp"
#include "base_thread.h"

#include <vector>
#include <thread>
#include <atomic>
#include <memory>

template <typename Key, typename Value, typename TypedProcessor>
class TestProducer : public BaseThread {
public:
  using GetValueFunctionType = std::function<Value(void)>;
  TestProducer(const std::shared_ptr<TypedProcessor> &processor, 
               const std::vector<Key> &keys, int max_pushes,
               const GetValueFunctionType &value_getter)
    : BaseThread(max_pushes)
    , queue_processor_(processor)
    , keys_(keys)
    , get_value_func_(value_getter) {}
  
  ~TestProducer() {
    Stop();
  }

private:
  void Execute() override {
    for (auto &key : keys_) {
      queue_processor_->Enqueue(key, get_value_func_());
    }
  }
private:
  std::shared_ptr<TypedProcessor> queue_processor_;
  std::vector<Key> keys_;
  int max_pushes_;
  GetValueFunctionType get_value_func_;
};