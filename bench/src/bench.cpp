#include <benchmark/benchmark.h>

#include "../../src/gcodeRewind.h"

static void BM_insertHeader(benchmark::State& state) 
{
  char buf[1000];
  for (auto _ : state)
    insertHeader(buf);
}
BENCHMARK(BM_insertHeader);

static void BM_gCodeRevert(benchmark::State& state) 
{
    // Setup
    FILE* fp = fopen("res/small/test1.gcode", "r");
    FILE* fpout = fopen("res/out/result.gcode", "rw");
    struct GCodeFileInstance inst   = { .file = fp, .byteOffset = 1200 };
    struct GCodeFileInstance res    = { .file = fpout, .byteOffset = 0 };

    for (auto _ : state)
        gCodeRevert(&inst, &res);
}
BENCHMARK(BM_gCodeRevert);

// Run the Benchmark
BENCHMARK_MAIN();