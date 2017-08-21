#pragma once
#include <chrono>

// A test type used to measure the latency
struct ValData {
  double value_;
  std::chrono::high_resolution_clock::time_point start_;

  ValData() : value_(-1) {}

  ValData(double val, const std::chrono::high_resolution_clock::time_point &t)
    : value_(val)
    , start_(t) {}

  bool operator!=(const ValData &right) {
    return value_ != right.value_;
  }
};

