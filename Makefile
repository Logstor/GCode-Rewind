COMPILER := gcc
TESTFLAGS := -Wall -g -O0

SOURCE := $(src/**)
TESTSOURCE := test/src/*.c
TESTBIN := test/bin/testbin

test: test/src/main.c
	@echo
	@echo -- Compiling test source --
	@$(COMPILER) $(TESTFLAGS) $(TESTSOURCE) -o $(TESTBIN)
	@echo -- Running the tests --
	@echo
	@./$(TESTBIN)
	@echo

docs: Doxyfile
	@echo
	@echo -- Generating Documentation --
	@echo
	doxygen Doxyfile
	@echo
	@echo -- Documentation Done! --

# Static Analysis using Clang Tidy and Valgrind