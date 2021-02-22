
#ifndef GCODEREWIND_H
#define GCODEREWIND_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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
    size_t byteOffset;
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
    char* buf;

    /**
     * @brief Byte size of the char array.
     */
    size_t size;

    /**
     * @brief Current offset where the printer has reached.
     */
    size_t byteOffset;
};

/**
 * @brief This functions inserts a standard header to the buffer.
 * 
 * @param buffer The char buffer to insert the header data into.
 * 
 * @warning Be sure that the allocated buffer is big enough!
 */
void insertHeader(char* buffer)
{
    strcpy(buffer, 
    "\
    T0 ; Select head 0\n\
    M302 P1 ; Allow cold extrusion\n\
    G90 ; Absolute position\n\
    M83 ; Relative extrusion\n\
    G21 ; Metric values\n\
    G1 U0 F6000.00 ; Set speed\n\
    \n\
    "
    );
}


/**
 * @brief Generates a file from the inFile, which is reverted.
 * 
 * This function takes in a GCodeFileInstance, inFile, which contains a FILE* and corressponding byte offset. 
 * The byte offset describes where the printer is stopped which is used to determine where we should start backtracking from.
 * 
 * Beaware that this function isn't thread safe as the File IO functions used doesn't lock the file.
 * 
 * @param inFile The GCodeFileInstance containing the GCode file pointer to be processed.
 * 
 * @param resFile The GCodeFileInstance which is containing the file pointer to the newly created result file.
 * 
 * @return RESULT OK or FAIL
 * 
 * @warning NOT thread safe - Uses unlocked IO functions!
 */
RESULT gCodeRevert(const struct GCodeFileInstance *inFile, struct GCodeFileInstance *resFile)
{
    // Check File Pointer
    if (inFile == NULL || ferror_unlocked(inFile->file) != 0)
    {
        fputs("Error in FileStream!\n", stderr);
        return FAIL;
    }

    // Allocate for file buffer
    #ifdef GCODE_USE_STACK_MEM

    char pInFileBuf[inFile->byteOffset];
    char pOutFileBuf[inFile->byteOffset];

    #else

    char *pInFileBuf = (char*) malloc(inFile->byteOffset);
    if (pInFileBuf == NULL)
    {
        fputs("Buffer Allocation for In File buffer failed!\n", stderr);
        return FAIL;
    }
    char *pOutFileBuf = (char*) malloc(inFile->byteOffset);
    if (pOutFileBuf == NULL)
    {
        fputs("Buffer Allocation for Out File buffer failed!\n", stderr);
        free(pInFileBuf);
        return FAIL;
    }

    #endif

    // Success flag for memory allocations
    bool success = true;

    // Read file into the file buffer
    size_t bytesRead = fread_unlocked(pInFileBuf, sizeof(char), inFile->byteOffset, inFile->file);
    if (bytesRead != inFile->byteOffset)
    {
        fputs("File wasn't big enough to read until the byteoffset!\n", stderr);
        success = false;
        goto clean;
    }

    // Insert header first
    insertHeader(pOutFileBuf);

    // Clean up
    clean:
    #ifndef GCODE_USE_STACK_MEM
    free(pInFileBuf);
    free(pOutFileBuf);
    #endif

    return (success) ? OK : FAIL;
}


#endif