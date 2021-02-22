#include <stdlib.h>
#include <stdio.h>

#include <CUnit/CUnit.h>

#include "../../src/gcodeRewind.h"

int main(int argc, char** argv)
{
    // Start Framework
    if (CU_initialize_registry() != CUE_SUCCESS)
    {
        puts("ERROR! Couldn't allocate memory for test registry!");
        return -1;
    }

    // Create Suite
    
    
    // Clean Framework
    CU_cleanup_registry();
    return 0;
}