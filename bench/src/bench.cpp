#include <benchmark/benchmark.h>

#include "../../src/gcodeRewind.h"

static void BM_countNumberOfLines(benchmark::State& state)
{
  const char *s = 
    "Here\n\
    is\n\
    six\n\
    lines\n\
    to\n\
    test\n";

  for (auto _ : state)
    countNumberOfLines(s, strlen(s));
}
BENCHMARK(BM_countNumberOfLines);

//
static void BM_insertHeader(benchmark::State& state) 
{
  char buf[1000];
  for (auto _ : state)
    insertHeader(buf);
}
BENCHMARK(BM_insertHeader);

// 
static void BM_gCodeRevert(benchmark::State& state) 
{
  // Setup
  FILE* fp = fopen("res/small/test1.gcode", "r");
  FILE* fpout = fopen("res/out/result.gcode", "rw");
  struct GCodeFileInstance inst   = { .file = fp, .byteOffset = 1200 };
  struct GCodeFileInstance res    = { .file = fpout, .byteOffset = 0 };

  for (auto _ : state)
  {
    gCodeRevert(&inst, &res);
    fseek(fp, 0, SEEK_SET);
    //fseek(fpout, 0, SEEK_SET);
  }

  fclose(fp);
  //fclose(fpout);
}
BENCHMARK(BM_gCodeRevert);

// Run the Benchmark
BENCHMARK_MAIN();