
#ifndef TESTHEADER_H
#define TESTHEADER_H

#include "../../src/gcodeRewind.h"

#include <CUnit/CUnit.h>

void countNumberOfLinesTest()
{
    const char *s = 
    "Here\n\
    is\n\
    six\n\
    lines\n\
    to\n\
    test\n";

    int res = countNumberOfLines(s, strlen(s));
    CU_ASSERT_EQUAL(res, 6);
};

void insertHeaderTest()
{
    char buf[1024];
    insertHeader(buf);
}

void gCodeRewindTest()
{
    struct GCodeFileInstance inFile = { .file = fopen("test/res/test1.gcode", "r"), .byteOffset = 1598 };
    struct GCodeFileInstance outFile = { .file = fopen("test/res/result.gcode", "rw"), .byteOffset = 0 };

    CU_ASSERT_EQUAL(gCodeRevert(&inFile, &outFile), OK);
}

#endif // TESTHEADER_H