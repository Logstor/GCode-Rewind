COMPILER := gcc
BENCHCOMPILER := g++
VALGRIND := valgrind

BENCHFLAGS := -O3 -std=c++11 -lbenchmark -lpthread 
TESTFLAGS := -Wall -ggdb3 -fstack-protector-strong -Wpedantic -Wextra -O0 -lcunit 
VALGRINDFLAGS := --leak-check=full --track-origins=yes --track-fds=yes --time-stamp=yes

SOURCE := $(src/**)
BENCHSOURCE := bench/src/*.cpp
TESTSOURCE := test/src/*.c

BENCHBIN := bench/bin/benchbin
TESTBIN := test/bin/testbin

all: build docs

build: buildtest buildbench

buildtest:
	@echo
	@echo -- Compiling test source --
	@$(COMPILER) $(TESTFLAGS) $(TESTSOURCE) -o $(TESTBIN)
	@echo
	@echo -- Done! --

buildbench:
	@echo
	@echo -- Compiling bench source --
	@$(BENCHCOMPILER) $(BENCHFLAGS) $(BENCHSOURCE) -o $(BENCHBIN)
	@echo
	@echo -- Done! --

buildbenchtry:
	@echo
	@echo -- Compiling bench source --
	@$(BENCHCOMPILER) $(BENCHFLAGS) -DGCODE_REWIND_TRY_TESTS $(BENCHSOURCE) -o $(BENCHBIN)
	@echo
	@echo -- Done! --

test: buildtest
	@echo -- Running the tests --
	@echo
	@./$(TESTBIN)
	@echo
	@echo -- DONE! --

bench: buildbench
	@echo -- Running the benchmarks --
	@echo
	@./$(BENCHBIN)
	@echo
	@echo -- DONE! --

benchtry: buildbenchtry
	@echo -- Running the benchmarks --
	@echo
	@./$(BENCHBIN)
	@echo
	@echo -- DONE! --

# Static and Dynamic Analysis with Valgrind
staticanalysis:
	@echo
	@echo -- Compiling test source --
	@$(COMPILER) $(TESTFLAGS) -fanalyzer $(TESTSOURCE) -o $(TESTBIN)
	@echo
	@echo -- Done! --

valgrind: test/src/main.c
	@echo
	@echo -- Compiling test source --
	@$(COMPILER) $(TESTFLAGS) -DGCODE_REWIND_ONLY_SMALL_TESTS $(TESTSOURCE) -o $(TESTBIN)
	@echo
	@echo -- Done! --
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
