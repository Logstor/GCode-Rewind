COMPILER := gcc
BENCHCOMPILER := g++

BENCHFLAGS := -Wall -g -O0 -std=c++11 -lbenchmark -lpthread
TESTFLAGS := -Wall -g -O0 -lcunit

SOURCE := $(src/**)
BENCHSOURCE := bench/src/*.cpp
TESTSOURCE := test/src/*.c

BENCHBIN := bench/bin/benchbin
TESTBIN := test/bin/testbin

test: test/src/main.c
	@echo
	@echo -- Compiling test source --
	@$(COMPILER) $(TESTFLAGS) $(TESTSOURCE) -o $(TESTBIN)
	@echo -- Running the tests --
	@echo
	@./$(TESTBIN)
	@echo
	@echo -- DONE! --

bench: bench/src/bench.cpp
	@echo
	@echo -- Compiling bench source --
	@$(BENCHCOMPILER) $(BENCHFLAGS) $(BENCHSOURCE) -o $(BENCHBIN)
	@echo
	@echo -- Running the benchmarks --
	@echo
	@./$(BENCHBIN)
	@echo
	@echo -- DONE! --

docs: Doxyfile
	@echo
	@echo -- Generating Documentation --
	@echo
	doxygen Doxyfile
	@echo
	@echo -- Documentation Done! --

# Static Analysis using Clang Tidy and Valgrind