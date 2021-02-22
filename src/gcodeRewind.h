
#ifndef GCODEREWIND_H
#define GCODEREWIND

#include <stdlib.h>
#include <stdio.h>

enum CMD
{
    G,
    M,
    N
};

/**
 * @brief Used as return type of library functions to indicate if everything went well.
 * 
 */
typedef enum
{
    OK,
    FAIL
} RESULT;

/**
 * @brief GCode instance, where the data from file has to read.
 * 
 * 
 */
struct GCodeFileInstance
{
    /**
     * @brief File pointer to the GCode file.
     */
    FILE* file;
    
    /**
     * @brief The byte offset of where the printer currently is.
     */
    const int byteOffset;
};

/**
 * @brief GCode instance, where the contents is loaded into RAM.
 * 
 * 
 */
struct GCodeFileBufferedInstance
{
    /**
     * @brief Pointer to the char array.
     */
    const char* buf;

    /**
     * @brief Byte size of the char array.
     */
    const size_t size;

    /**
     * @brief Current offset where the printer has reached.
     */
    const int byteOffset;
};

/**
 * @brief
 * 
 * 
 * 
 * @param inFile The GCodeFileInstance containing the GCode file pointer to be processed.
 * @param resFile The GCodeFileInstance which to put the resulting GCode file pointer into.
 * 
 * @return RESULT OK or FAIL
 * 
 * @warning NOT thread safe - Uses unlocked IO functions!
 */
RESULT GCodeRevert(const struct GCodeFileInstance *inFile, struct GCodeFileInstance *resFile)
{
    // Check File Pointer
    if (inFile == NULL || ferror_unlocked(inFile->file) != 0)
    {
        fputs("Error in FileStream!\n", stderr);
        return FAIL;
    }

    // Allocate for file buffer
    #ifdef GCODE_USE_STACK_MEM

    char pFileBuf[inFile->byteOffset];

    #else

    char *pFileBuf = (char*) malloc(inFile->byteOffset);
    if (pFileBuf == NULL)
    {
        fputs("Buffer Allocation for File buffer failed!\n", stderr);
        return FAIL;
    }

    #endif

    // Read file into the file buffer
    size_t bytesRead = fread_unlocked(pFileBuf, sizeof(char), inFile->byteOffset, inFile->file);
    if (bytesRead != inFile->byteOffset)
    {
        fputs("File wasn't big enough to read until the byteoffset!\n", stderr);
        return FAIL;
    }

    // Clean up
    #ifndef GCODE_USE_STACK_MEM
    free(pFileBuf);
    #endif


    return OK;
}

#endif