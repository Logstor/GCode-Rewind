COMPILER := gcc
BENCHCOMPILER := g++
VALGRIND := valgrind

BENCHFLAGS := -Wall -g -O0 -std=c++11 -lbenchmark -lpthread #-DGCODE_USE_STACK_MEM
TESTFLAGS := -Wall -ggdb3 -O0 -lcunit #-DGCODE_USE_STACK_MEM
VALGRINDFLAGS := --leak-check=full --track-origins=yes --track-fds=yes --time-stamp=yes

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

teststack: test/src/main.c
	@echo
	@echo -- Compiling test source --
	@$(COMPILER) $(TESTFLAGS) -DGCODE_USE_STACK_MEM $(TESTSOURCE) -o $(TESTBIN)
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

benchstack: bench/src/bench.cpp
	@echo
	@echo -- Compiling bench source --
	@$(BENCHCOMPILER) $(BENCHFLAGS) -DGCODE_USE_STACK_MEM $(BENCHSOURCE) -o $(BENCHBIN)
	@echo
	@echo -- Running the benchmarks --
	@echo
	@./$(BENCHBIN)
	@echo
	@echo -- DONE! --

# Static and Dynamic Analysis with Valgrind
valgrind: test/src/main.c
	@echo
	@echo -- Compiling test source --
	@$(COMPILER) $(TESTFLAGS) $(TESTSOURCE) -o $(TESTBIN)
	@echo -- Running the tests --
	@echo
	@$(VALGRIND) $(VALGRINDFLAGS) $(TESTBIN)
	@echo
	@echo -- DONE! --

docs: Doxyfile
	@echo
	@echo -- Generating Documentation --
	@echo
	doxygen Doxyfile
	@echo
	@echo -- Documentation Done! --
