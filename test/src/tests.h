
#ifndef TESTHEADER_H
#define TESTHEADER_H

#include "../../src/gcodeRewind.h"

#include <CUnit/CUnit.h>

void gCodeRewindTest()
{
    const char testFilename[]   = "test/res/test1.gcode";
    const char resultFilename[] = "test/res/result.gcode";

    RESULT res = gCodeRevert(testFilename, resultFilename);

    CU_ASSERT_EQUAL(res, OK);
}

#endif // TESTHEADER_H