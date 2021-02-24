
#ifndef GCODEREWIND_H
#define GCODEREWIND_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

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
 * @brief A simple buffer struct to contain byte data.
 * 
 */
struct ByteBuffer
{
    /**
     * @brief Pointer to the byte buffer.
     * 
     */
    char *buffer;

    /**
     * @brief Byte size of the buffer.
     * 
     */
    size_t size;
};

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
 * @return 
 * 
 */
struct ByteBuffer* reverseGCodeCommands(char *inBuffer, const size_t inSize)
{
    #define MAX_LINE_SIZE 50

    // Count lines
    const uint32_t lineCount = countNumberOfLines(inBuffer, inSize);

    // Temporary buffer
    char lines[lineCount][MAX_LINE_SIZE];

}

/**
 * @brief This function allocates a ByteBuffer struct with Header and reversed GCode.
 * 
 * @param inBuffer: 
 * 
 * @param inSize: 
 * 
 * @return A ByteBuffer struct containing the reversed GCode with default header.
 * 
 * @warning Make sure to free the array in the ByteBuffer and the ByteBuffer itself.
 * 
 */
struct ByteBuffer* fillOutBuffer(const char *inBuffer, const size_t inSize)
{
    // Declare header and buffer sizes
    const char defaultHeader[] = "T0 ; Select head 0\nM302 P1 ; Allow cold extrusion\nG90 ; Absolute position\nM83 ; Relative extrusion\nG21 ; Metric values\nG1 U0 F6000.00 ; Set default printing speed\n\n";
    const size_t headerSize = strlen(defaultHeader);
    const size_t totSize = headerSize + inSize;

    // Count lines and create buffers
    const uint32_t lineCount = countNumberOfLines(inBuffer, inSize);
    char inBufCpy[inSize];
    char *lines[lineCount];

    // Make inBuffer copy
    memcpy(inBufCpy, inBuffer, inSize);

    // Divide into tokens, and copy the into lines
    uint_fast32_t index = 0;
    char *token = strtok(inBufCpy, "\n");
    do {
        lines[index++] = token;
        token = strtok(NULL, "\n");
    } while (token != NULL);

    // Insert header to buffer
    char *array = (char*) malloc(totSize * sizeof(char));
    struct ByteBuffer *byteBufferOut = (struct ByteBuffer*) malloc(sizeof(struct ByteBuffer));
    byteBufferOut->buffer = array; byteBufferOut->size = totSize;
    strcpy(array, defaultHeader);

    // Reverse code into buffer
    do {
        strcat(array, lines[--index]);
    } while (index != 0);

    // Return the ByteBuffer
    return byteBufferOut;
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
    const size_t inFileBufSize = inFile->byteOffset+1;
#ifdef GCODE_USE_STACK_MEM
    
    char pInFileBuf[inFileBufSize];      // Plus null termination

#else

    char *pInFileBuf = (char*) malloc(inFileBufSize * sizeof(char));         // Plus null termination
    if (pInFileBuf == NULL)
    {
        fputs("Buffer Allocation for In File buffer failed!\n", stderr);
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
        free(pInFileBuf);
        return FAIL;
    }

    // Write reversed GCode to buffer
    struct ByteBuffer *genGCode = fillOutBuffer(pInFileBuf, inFile->byteOffset);

    // Write everything to file
    //fprintf(resFile->file, "%s", pOutFileBuf);
    fwrite(genGCode->buffer, sizeof(char), genGCode->size, resFile->file);

    // Clean up
    clean:
#ifndef GCODE_USE_STACK_MEM
    free(pInFileBuf);
#endif

    return (success) ? OK : FAIL;
}


#endif // GCODEREWIND_H