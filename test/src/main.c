#include <CUnit/CUnit.h>
#include <CUnit/Console.h>
#include <CUnit/Automated.h>
#include <stdlib.h>
#include <stdio.h>

#include "tests.h"

//#define GCODE_REWIND_TRY_TESTS

int setup() 
{
    // Create Suite
    CU_pSuite unitSuite = CU_add_suite("Unit tests", NULL, NULL);
    if (unitSuite == NULL)
    {
        CU_cleanup_registry();
        return -1;
    }

    // Add Unit tests
    CU_ADD_TEST(unitSuite, gCodeRewindTest);
#ifdef GCODE_REWIND_TEST_BIG 
    CU_ADD_TEST(unitSuite, gCodeRewindBigTest);
#endif

#ifdef GCODE_REWIND_TRY_TESTS
    CU_pSuite unitTrySuite = CU_add_suite("Unit try tests", NULL, NULL);

    CU_ADD_TEST(unitTrySuite, gCodeRewindTryTest);
    #ifdef GCODE_REWIND_TEST_BIG 
        CU_ADD_TEST(unitTrySuite, gCodeRewindBigTryTest);
    #endif

#endif
    
    return 0;
}

int main(int argc, char** argv)
{
    // Start Framework
    if (CU_initialize_registry() != CUE_SUCCESS)
    {
        puts("ERROR! Couldn't allocate memory for test registry!");
        return -1;
    }

    // Setup
    if (setup() == 0)
    {
        // Start testing
        CU_console_run_tests();
    }
    
    // Clean Framework
    CU_cleanup_registry();
    return 0;
}