#include <CUnit/CUnit.h>
#include <CUnit/Console.h>
#include <CUnit/Automated.h>
#include <stdlib.h>
#include <stdio.h>

#include "tests.h"

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
    CU_ADD_TEST(unitSuite, countNumberOfLinesTest);
    CU_ADD_TEST(unitSuite, insertHeaderTest);
    CU_ADD_TEST(unitSuite, gCodeRewindTest);
    
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