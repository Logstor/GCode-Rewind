#include <benchmark/benchmark.h>

#include "../../src/gcodeRewind.h"

// 
static void BM_gCodeRevert(benchmark::State& state) 
{
  // Setup
  const char testFile[] = "res/small/test1.gcode";
  const char resFile[]  = "bench/out/result1.gcode";

  for (auto _ : state)
  {
    gCodeRevert(testFile, resFile);
  }
}
BENCHMARK(BM_gCodeRevert);

// Run the Benchmark
BENCHMARK_MAIN();