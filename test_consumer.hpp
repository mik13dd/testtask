#pragma once
#include "consumer_interface.hpp"

template<typename Key, typename Value>
class TestConsumer : public IConsumer<Key, Value> {
public:
  using ConsumeFunctionType = std::function<void(Key, Value)>;
  TestConsumer(const ConsumeFunctionType &func) 
    : consume_func_(func) {}

  void Consume(const Key &key, const Value &value) override {
    consume_func_(key, value);
  }
private:
  ConsumeFunctionType consume_func_;
};