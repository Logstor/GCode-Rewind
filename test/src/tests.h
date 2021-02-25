
#ifndef TESTHEADER_H
#define TESTHEADER_H

#include "../../src/gcodeRewind.h"

#include <CUnit/CUnit.h>
#include <stdio.h>

void gCodeRewindTest()
{
    const char testFilename[]   = "test/res/test1.gcode";
    const char resultFilename[] = "test/res/result.gcode";

    RESULT res = gCodeRevert(testFilename, resultFilename);

    CU_ASSERT_EQUAL(res, OK);
}

void gCodeRewindBigTest()
{
    const char testFilename1[]  = "res/big/AngolaHouseLH40.gcode";
    const char testFilename2[]  = "res/big/AngolaHouse.gcode";
    const char testFilename3[]  = "res/big/4housesLH20.gcode";

    const char resFilename[]    = "res/result.gcode";

    // Run tests
    RESULT res1 = gCodeRevert(testFilename1, resFilename);
    RESULT res2 = gCodeRevert(testFilename2, resFilename);
    RESULT res3 = gCodeRevert(testFilename3, resFilename);

    // Asserts
    CU_ASSERT_EQUAL(res1, OK);
    CU_ASSERT_EQUAL(res2, OK);
    CU_ASSERT_EQUAL(res3, OK);

    remove(resFilename);
}

#endif // TESTHEADER_H