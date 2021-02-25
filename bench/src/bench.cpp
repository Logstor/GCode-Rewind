#include <benchmark/benchmark.h>

#include "../../src/gcodeRewind.h"
#include <stdio.h>

// 
static void BM_gCodeRevert(benchmark::State& state) 
{
  // Setup
  const char testFile[] = "res/small/test1.gcode";
  const char resFile[]  = "res/result1.gcode";

  for (auto _ : state)
  {
    gCodeRevert(testFile, resFile);
  }

  remove(resFile);
}
BENCHMARK(BM_gCodeRevert);

#ifdef GCODE_REWIND_TRY_TESTS
// 
static void BM_gCodeRevertTry(benchmark::State& state) 
{
  // Setup
  const char testFile[] = "res/small/test1.gcode";
  const char resFile[]  = "res/result1.gcode";

  for (auto _ : state)
  {
    gCodeRevertTry(testFile, resFile);
  }

  remove(resFile);
}
BENCHMARK(BM_gCodeRevertTry);
#endif

// Run the Benchmark
BENCHMARK_MAIN();