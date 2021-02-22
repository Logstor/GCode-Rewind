
#ifndef GCODEREWIND_H
#define GCODEREWIND

#include <stdio.h>

enum CMD
{
    G,
    M,
    N
};

/**
 * @brief
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
    const FILE* file;
    
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
 * @param inFile
 * @param resFile
 * 
 * @return RESULT OK or FAIL
 * 
 * @warning
 */
RESULT GCodeRevert(const struct GCodeFileInstance *inFile, struct GCodeFileInstance *resFile)
{
    return OK;
}

#endif