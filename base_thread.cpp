#include "base_thread.h"

BaseThread::BaseThread(int max_loops)
  : is_running_(true)
  , max_loops_(max_loops)
  , counter_(0)
  , thread_(max_loops == 0 ? &BaseThread::Run : &BaseThread::RunFinite, this) {}

BaseThread::~BaseThread() {
  Stop();
  Wait();
}

void BaseThread::Run() {
  while(is_running_.load()) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    Execute();
  }
}

void BaseThread::RunFinite() {
  for (counter_ = 0; counter_ < max_loops_; ++counter_) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    Execute();
  }
}

void BaseThread::Stop() {
  is_running_ = false;
}

void BaseThread::Wait() {
  if (thread_.joinable())
    thread_.join();
}

void BaseThread::Execute() {
}