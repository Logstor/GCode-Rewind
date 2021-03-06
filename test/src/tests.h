
#ifndef TESTHEADER_H
#define TESTHEADER_H

#include "../../src/gcodeRewind.h"

#include <CUnit/CUnit.h>
#include <stdio.h>

//#define GCODE_REWIND_TRY_TESTS

const struct RewindSettings settings0 = { .byteOffset = 885, .endLayer = 0, .stopExtrusion = true };
const struct RewindSettings settings1 = { .byteOffset = 950, .endLayer = 0, .stopExtrusion = false };
const struct RewindSettings settingsBig = { .byteOffset = 0, .endLayer = 21, .stopExtrusion = false };

#ifdef GCODE_REWIND_TRY_TESTS
void gCodeRewindTryTest()
{
    const char testFilename[]   = "test/res/test1.gcode";
    const char resultFilename[] = "test/res/resulttry0.gcode";

    RESULT res = gCodeRevertTry(testFilename, resultFilename, &settings0);

    CU_ASSERT_EQUAL(res, OK);
}

void gCodeRewindBigTryTest()
{
    const char testFilename1[]  = "res/big/AngolaHouseLH40.gcode";
    const char testFilename2[]  = "res/big/AngolaHouse.gcode";
    const char testFilename3[]  = "res/big/4housesLH20.gcode";

    const char resFilename[]    = "res/result0.gcode";

    // Run tests
    RESULT res1 = gCodeRevertTry(testFilename1, resFilename, &settings0);
    RESULT res2 = gCodeRevertTry(testFilename2, resFilename, &settings0);
    RESULT res3 = gCodeRevertTry(testFilename3, resFilename, &settings0);

    // Asserts
    CU_ASSERT_EQUAL(res1, OK);
    CU_ASSERT_EQUAL(res2, OK);
    CU_ASSERT_EQUAL(res3, OK);

    remove(resFilename);
}
#endif

void gCodeRewindTest()
{
    const char testFilename[]   = "test/res/test1.gcode";
    const char resultFilename[] = "test/res/result0.gcode";

    RESULT res = gCodeRevert(testFilename, resultFilename, &settings0);

    CU_ASSERT_EQUAL(res, OK);

    // Read whole the resulting file and check if it's valid
    FILE* fdRes = fopen(resultFilename, "r");
    FILE* fdVal = fopen("test/res/validate.gcode", "r");

    // Check size
    fseek(fdRes, 0, SEEK_END);
    fseek(fdVal, 0, SEEK_END);
    const off_t resSize = ftello(fdRes);
    const off_t valSize = ftello(fdVal);
    rewind(fdRes);
    rewind(fdVal);

    CU_ASSERT_EQUAL(resSize, valSize);

    // Check data
    char* resBuffer[resSize];
    char* valBuffer[valSize];

    fread(resBuffer, sizeof(char), resSize, fdRes);
    fread(valBuffer, sizeof(char), valSize, fdVal);

    const int cmpRes = memcmp(resBuffer, valBuffer, resSize);
    const int cmpVal = memcmp(valBuffer, resBuffer, valSize);

    CU_ASSERT_EQUAL(cmpRes, cmpVal);

    // Close everything
    fclose(fdRes); fclose(fdVal);
}

void gCodeRewindTestWithExtrusion()
{
    const char testFilename[]   = "test/res/test1.gcode";
    const char resultFilename[] = "test/res/result1.gcode";

    RESULT res = gCodeRevert(testFilename, resultFilename, &settings1);

    CU_ASSERT_EQUAL(res, OK);
}

void gCodeRewindBigTest()
{
    const char testFilename1[]  = "res/big/AngolaHouseLH40.gcode";
    const char testFilename2[]  = "res/big/AngolaHouse.gcode";
    const char testFilename3[]  = "res/big/4housesLH20.gcode";

    const char resFilename[]    = "res/result0.gcode";

    // Run tests
    RESULT res1 = gCodeRevert(testFilename1, resFilename, &settings0);
    RESULT res2 = gCodeRevert(testFilename2, resFilename, &settings1);
    RESULT res3 = gCodeRevert(testFilename3, resFilename, &settingsBig);

    // Asserts
    CU_ASSERT_EQUAL(res1, OK);
    CU_ASSERT_EQUAL(res2, OK);
    CU_ASSERT_EQUAL(res3, OK);

    remove(resFilename);
}

#endif // TESTHEADER_H