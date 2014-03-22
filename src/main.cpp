/* main.cpp - CS 472 Project #2: Genetic Programming
 * Copyright 2014 Andrew Schwartzmeyer
 */

#include <algorithm>
#include <cassert>
#include <chrono>
#include <ctime>
#include <future>
#include <iostream>
#include <thread>

#include "algorithm/algorithm.hpp"
#include "individual/individual.hpp"
#include "problem/problem.hpp"
#include "random_generator/random_generator.hpp"

int main() {
  using individual::Individual;
  using namespace problem;
  const Problem problem{get_data(), 256, 256, 2, 3};
  const int trials = 24;
  int trial = 0;
  const unsigned long hardware_threads = std::thread::hardware_concurrency();
  const unsigned long blocks = hardware_threads != 0 ? hardware_threads : 2;
  assert(trials % blocks == 0);
  std::vector<Individual> candidates;
  std::chrono::time_point<std::chrono::system_clock> start, end;
  // begin timing trials
  std::time_t time = std::time(nullptr);
  start = std::chrono::system_clock::now();
  // spawn trials number of threads in blocks
  for (unsigned long t = 0; t < trials / blocks; ++t) {
    std::vector<std::future<const Individual>> results;
    for (unsigned long i = 0; i < blocks; ++i)
      results.emplace_back(std::async(std::launch::async, algorithm::genetic, problem, time, ++trial));
    // gather results
    for (std::future<const Individual> & result : results)
      candidates.emplace_back(result.get());
  }
  // end timing trials
  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  const Individual best = *std::min_element(candidates.begin(), candidates.end(), algorithm::compare_fitness);
  std::cout << " elapsed time: " << elapsed_seconds.count() << "s\n"
	    << best.print_formula()
	    << best.print_calculation(problem.values);
}
