#include "report.h"

#include <iostream>

Report::Report(int producers_count)
  : pushes_(0)
  , pops_(0)
  , latency_(0)
  , producers_count_(producers_count) {}

void Report::Print() const {
  if (producers_count_ == 0 || pops_ == 0) {
    std::cout << "Nothing to do" << std::endl;
    return;
  }
  std::cout << " Total producers                : " << producers_count_ << std::endl
            << " Iterations(pushes per producer): " << pushes_ / producers_count_ << std::endl
            << " Iterations(pushes total)       : " << pushes_ << std::endl
            << " Pops(total in consumer)        : " << pops_ << std::endl
            << " Ratio(pops / pushes)           : " << (float)pops_/pushes_ << std::endl
            << " Avg Latency                    : " << latency_.count() / pops_ << "mcs"<< std::endl
            << " Total time:                    : " << total_time_.count() << "s" << std::endl;
}

void Report::CalcLatency(const std::chrono::high_resolution_clock::time_point &t) {
  auto tnow = std::chrono::high_resolution_clock::now();
  latency_ += std::chrono::duration_cast<std::chrono::microseconds>(tnow - t);
}