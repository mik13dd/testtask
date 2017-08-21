#pragma once
#include <thread>
#include <atomic>

// Helper class to handle thread running job
// It's main purpose to serve the main cycle of the thread
class BaseThread {
public:
  BaseThread(int max_loops = 0);
  ~BaseThread();
  virtual void Stop();
  virtual void Wait();
protected:
  virtual void Execute();
  void Run();
  void RunFinite();

  std::atomic_bool is_running_;
  int max_loops_;
  std::atomic_int counter_;

  std::thread thread_;
};