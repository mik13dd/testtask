#pragma once

template<typename Key, typename Value>
class IConsumer {
public:
  virtual void Consume(const Key &key, const Value &value) = 0;
  virtual ~IConsumer(){}
};
