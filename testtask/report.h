#pragma once

#include <atomic>
#include <chrono>
#include <string>

struct Report {
  std::atomic_uint64_t pushes_;
  std::atomic_uint64_t pops_;
  std::chrono::microseconds latency_;
  std::chrono::duration<double> total_time_;

  Report(int producers_count);
  void CalcLatency(const std::chrono::high_resolution_clock::time_point &t);

  void Print() const;

private:
  int producers_count_;
};