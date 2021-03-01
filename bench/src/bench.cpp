#include <benchmark/benchmark.h>

#include "../../src/gcodeRewind.h"
#include <stdio.h>

//#define GCODE_REWIND_TRY_TESTS

const struct RewindSettings settings0 = { .byteOffset = 0, .endLayer = 0, .stopExtrusion = true };
const struct RewindSettings settings1 = { .byteOffset = 0, .endLayer = 0, .stopExtrusion = false };

// 
static void BM_gCodeRevertNoExtrusion(benchmark::State& state) 
{
  // Setup
  const char testFile[] = "res/small/test1.gcode";
  const char resFile[]  = "res/result0.gcode";

  for (auto _ : state)
  {
    gCodeRevert(testFile, resFile, &settings0);
  }

  remove(resFile);
}
BENCHMARK(BM_gCodeRevertNoExtrusion);

static void BM_gCodeRevertWithExtrusion(benchmark::State& state) 
{
  // Setup
  const char testFile[] = "res/small/test1.gcode";
  const char resFile[]  = "res/result1.gcode";

  for (auto _ : state)
  {
    gCodeRevert(testFile, resFile, &settings1);
  }

  remove(resFile);
}
BENCHMARK(BM_gCodeRevertWithExtrusion);

static void BM_gCodeRevertBig(benchmark::State& state) 
{
  // Setup
  const char testFile[] = "res/big/4housesLH20.gcode";
  const char resFile[]  = "res/result0.gcode";

  for (auto _ : state)
  {
    gCodeRevert(testFile, resFile, &settings0);
  }

  remove(resFile);
}
BENCHMARK(BM_gCodeRevertBig);

#ifdef GCODE_REWIND_TRY_TESTS
// 
static void BM_gCodeRevertTry(benchmark::State& state) 
{
  // Setup
  const char testFile[] = "res/small/test1.gcode";
  const char resFile[]  = "res/result1.gcode";

  for (auto _ : state)
  {
    gCodeRevertTry(testFile, resFile, &settings0);
  }

  remove(resFile);
}
BENCHMARK(BM_gCodeRevertTry);
#endif

// Run the Benchmark
BENCHMARK_MAIN();