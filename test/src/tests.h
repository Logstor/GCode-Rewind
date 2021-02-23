
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
    struct GCodeFileInstance outFile = { .file = fopen("test/res/result.gcode", "w+"), .byteOffset = 0 };

    CU_ASSERT_EQUAL(gCodeRevert(&inFile, &outFile), OK);

    // ------------ Test output ------------
    FILE* fValid = fopen("test/res/validate.gcode", "r");
    fseek(fValid, 0, SEEK_END);
    long bSizeValid = ftell(fValid);
    rewind(fValid);

    fseek(outFile.file, 0, SEEK_END);
    long bSizeOut = ftell(outFile.file);
    rewind(outFile.file);

    // Read output and validation file
    char *pValidBuf = (char*) calloc(bSizeValid, sizeof(char));
    char *pOutBuf   = (char*) calloc(bSizeOut, sizeof(char));

    size_t inReadValid  = fread(pValidBuf, sizeof(char), bSizeValid, fValid);
    size_t inReadOut    = fread(pOutBuf, sizeof(char), bSizeOut, outFile.file);

    CU_ASSERT_EQUAL(inReadValid, inReadOut);// Make sure the same amount of bytes is read
    CU_ASSERT_TRUE(inReadOut > 0);          // Make sure anything is read
    CU_ASSERT_TRUE(pOutBuf[0] != '\0');     // Make sure first character isn't "String end"

    int cmpRes = memcmp(pValidBuf, pOutBuf, inReadValid);
    printf("\n\nValidate buffer:\n%s\n\nOut Buffer:\n%s\n\n", pValidBuf, pOutBuf);
    
    CU_ASSERT_EQUAL(cmpRes, 0);             // Make sure the buffers is equal

    // ------------ Close files and cleanup ------------
    free(pValidBuf);
    free(pOutBuf);
    fclose(fValid);
    fclose(inFile.file);
    fclose(outFile.file);
}

#endif // TESTHEADER_H