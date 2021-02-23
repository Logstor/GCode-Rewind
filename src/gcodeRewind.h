
#ifndef GCODEREWIND_H
#define GCODEREWIND_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

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
 * @brief Counts the amount of lines in the buffer.
 * 
 */
uint32_t countNumberOfLines(const char *buffer, const size_t size)
{
    uint32_t lineCount = 0;
    for (uint32_t i=0; i < size; i++)
    {
        if (buffer[i] == '\n') lineCount++;
    }

    return lineCount;
}

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
    "T0 ; Select head 0\nM302 P1 ; Allow cold extrusion\nG90 ; Absolute position\nM83 ; Relative extrusion\nG21 ; Metric values\nG1 U0 F6000.00 ; Set default printing speed\n\n"
    );
}

/**
 * @brief Reverses the GCode in inBuffer, and outputs it to outBuffer.
 * 
 * @param inBuffer: Should contain all the commands in natural order.
 * 
 * @param inSize: 
 * 
 * @param outBuffer: Buffer to output the reversed commands.
 * 
 */
void reverseGCodeCommands(char *inBuffer, const size_t inSize, char *outBuffer)
{
    #define MAX_LINE_SIZE 50

    // Count lines
    uint32_t lineCount = countNumberOfLines(inBuffer, inSize);

    // Fill all the lines into array
    char lines[lineCount][MAX_LINE_SIZE];
    uint_fast8_t curr = 0;
    char currCh; 
    uint32_t currIndex = 0; int_fast32_t i;
    for (i=0; i < lineCount; i++)
    {
        currCh = inBuffer[currIndex++];
        while (currCh != (char)'\n')
        {
            lines[i][curr++] = currCh;
            currCh = inBuffer[currIndex++];
        }

        lines[i][curr++]    = '\n'; 
        lines[i][curr]      = '\0';

        // Reset Indexes
        curr = 0;
    }

    // Reverse all the lines into outBuffer
    i--;
    for (; i >= 0; i--)
    {
        strncat(outBuffer, lines[i], MAX_LINE_SIZE-1);
    }

}

/**
 * @brief This function just fills the outBuffer with Header and reversed GCode.
 * 
 * @param inBuffer: 
 * 
 * @param inSize: 
 * 
 * @param outBuffer:
 * 
 */
void fillOutBuffer(char *inBuffer, const size_t inSize, char *outBuffer)
{
    // Write header to buffer
    insertHeader(outBuffer);

    // Write reversed commands to buffer
    reverseGCodeCommands(inBuffer, inSize, outBuffer);
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

    char pInFileBuf[inFile->byteOffset+1];      // Plus null termination
    char pOutFileBuf[inFile->byteOffset+1];     // Plus null termination

#else

    char *pInFileBuf = (char*) malloc(inFile->byteOffset*sizeof(char) + 1);         // Plus null termination
    if (pInFileBuf == NULL)
    {
        fputs("Buffer Allocation for In File buffer failed!\n", stderr);
        return FAIL;
    }
    char *pOutFileBuf = (char*) malloc( (inFile->byteOffset*sizeof(char) + 1) *2);  // Plus null termination //FIXME: This should be made accurate!
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
    pInFileBuf[bytesRead] = '\0';       // Add Null termination
    if (bytesRead != inFile->byteOffset)
    {
        fputs("File wasn't big enough to read until the byteoffset!\n", stderr);
        success = false;
        goto clean;
    }

    // Write reversed GCode to buffer
    fillOutBuffer(pInFileBuf, inFile->byteOffset, pOutFileBuf);

    // Write everything to file
    //fprintf(resFile->file, "%s", pOutFileBuf);
    fwrite(pOutFileBuf, sizeof(char), (inFile->byteOffset*sizeof(char) + 1), resFile->file);

    // Clean up
    clean:
#ifndef GCODE_USE_STACK_MEM
    free(pInFileBuf);
    free(pOutFileBuf);
#endif

    return (success) ? OK : FAIL;
}


#endif // GCODEREWIND_H