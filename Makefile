


SOURCE = $(src/**)
TESTSOURCE = $(test/**)


docs: Doxyfile
	doxygen Doxyfile

# Static Analysis using Clang Tidy and Valgrind