#include "test_value_type.h"
#include "multi_queue_processor.hpp"
#include "multi_queue_processor_new.hpp"
#include "test_producer.hpp"
#include "test_consumer.hpp"
#include "report.h"

#include <iostream>
#include <random>

#include <boost/program_options.hpp>

typedef ValData ValueType;
typedef std::string KeyType;

const int MAX_PRODUCERS = 8;
const int MAX_PUSHES_PER_PRODUCER = 1000;

std::vector<std::string> pairs = { "AUD/CHF",
                                  "AUD/JPY",
                                  "AUD/NZD",
                                  "AUD/USD",
                                  "BGN/RON",
                                  "CAD/CHF",
                                  "CAD/JPY",
                                  "CHF/BGN",
                                  "CHF/JPY",
                                  "CHF/RON",
                                  };

template<typename ProcessorType>
void RunProcessor(int prod_count, int iter_count) {
  using CPProducer = TestProducer<KeyType, ValueType, ProcessorType>;
  using CPConsumer = TestConsumer<KeyType, ValueType>;

  std::uniform_real_distribution<double> distr(0, 1000);
  std::default_random_engine re;

  std::shared_ptr<ProcessorType> processor(new ProcessorType);
  
  Report report(prod_count);
  auto start = std::chrono::high_resolution_clock::now();

  std::shared_ptr<CPConsumer> consumer(new CPConsumer([&report](KeyType key, const ValueType &value) {
    ++(report.pops_);
    auto t = std::chrono::high_resolution_clock::now();
    report.CalcLatency(value.start_);
  }));

  for (auto &pair : pairs) {
    processor->Subscribe(pair, consumer);
  }

  std::vector<std::unique_ptr<CPProducer>> producers;
  for (auto i{ 0 }; i < prod_count; ++i) {
    std::unique_ptr<CPProducer> prod(new CPProducer(processor, pairs, iter_count, [&distr, &re, &report]() {
      ++(report.pushes_);
      return ValData{ distr(re), std::chrono::high_resolution_clock::now() };
    }));
    producers.push_back(std::move(prod));
  }

  for (auto &prod : producers) {
    prod->Wait();
  }

  processor->StopProcessing();

  auto end = std::chrono::high_resolution_clock::now();
  report.total_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  report.Print();
}

int main(int argc, char** argv) {
  boost::program_options::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "produce help message")
    ("producers,p", boost::program_options::value<int>(), "set producer thread count")
    ("iterations,i", boost::program_options::value<int>(), "max interations per producer")
    ;
  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  int producers = MAX_PRODUCERS;
  int iterations = MAX_PUSHES_PER_PRODUCER;

  if (vm.count("producers")) {
    producers = vm["producers"].as<int>();
  }
  if (vm.count("iterations")) {
    iterations = vm["iterations"].as<int>();
  }

  std::cout << "Old implementation:" << std::endl;
  RunProcessor<MultiQueueProcessor<KeyType, ValueType>>(producers, iterations);

  std::cout << "New implementation:" << std::endl;
  RunProcessor<MultiQueueProcessorNew<KeyType, ValueType>>(producers, iterations);

  system("pause");
  return 0;
}