
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
 * @brief 
 * 
 */
struct LineBuffer 
{
    /**
     * @brief 
     * 
     */
    char **pLines;

    /**
     * @brief 
     * 
     */
    size_t count;
};

/**
 * @brief 
 * 
 * @param initialCount: 
 * @return struct LineBuffer* 
 */
static inline struct LineBuffer* allocLineBuffer(const size_t initialCount)
{
    struct LineBuffer *lineBuffer = (struct LineBuffer*) malloc(sizeof(struct LineBuffer));

    lineBuffer->pLines  = (char**) malloc(sizeof(char*) * initialCount);
    lineBuffer->count   = initialCount;

    return lineBuffer;
}

/**
 * @brief 
 * 
 * @param lineBuffer: 
 * 
 * @warning If lines isn't stack allocated, then remember to dealloc them all.
 */
static inline void freeLineBuffer(struct LineBuffer *lineBuffer)
{ free(lineBuffer->pLines); free(lineBuffer); }


/**
 * @brief 
 * 
 * @param byteBuffer 
 */
static inline void freeByteBuffer(struct ByteBuffer *byteBuffer) 
{ free(byteBuffer->buffer); free(byteBuffer); }

/**
 * @brief This can be seen as constructor for a ByteBuffer.
 * 
 * @param size: The size of the buffer.
 * @return struct ByteBuffer* 
 */
static inline struct ByteBuffer* allocByteBuffer(const size_t size)
{
    struct ByteBuffer *pByteBuffer = (struct ByteBuffer*) malloc(sizeof(struct ByteBuffer));

    pByteBuffer->buffer = (char*) malloc(size);
    pByteBuffer->size   = size;

    return pByteBuffer;
}

/**
 * @brief This functions inserts a standard header to the buffer.
 * 
 * @param buffer The char buffer to insert the header data into.
 * 
 * @warning Be sure that the allocated buffer is big enough!
 */
static inline void insertHeader(char* buffer)
{
    strcpy(buffer, 
    "T0 ; Select head 0\nM302 P1 ; Allow cold extrusion\nG90 ; Absolute position\nM83 ; Relative extrusion\nG21 ; Metric values\nG1 U0 F6000.00 ; Set default printing speed\n\n"
    );
}

/**
 * @brief This functions inserts a standard header to the ByteBuffer.
 * 
 * This won't copy more data into the ByteBuffer, than the size of the ByteBuffer.
 * 
 * @param buffer: ByteBuffer struct.
 * 
 * @warning Be sure that the allocated buffer is big enough!
 */
static inline void insertHeader(struct ByteBuffer *byteBuffer)
{
    strncpy(byteBuffer->buffer, 
    "T0 ; Select head 0\nM302 P1 ; Allow cold extrusion\nG90 ; Absolute position\nM83 ; Relative extrusion\nG21 ; Metric values\nG1 U0 F6000.00 ; Set default printing speed\n\n", 
    byteBuffer->size
    );
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
struct ByteBuffer* fillOutBuffer(const struct LineBuffer* pLineBuffer)
{
    // Declare header and size
    char header[1024]; insertHeader(header);
    const uint32_t headerLength = strlen(header);

    // Create output buffer
    uint32_t inputLength = 0;
    uint_fast32_t i;
    for (i=0; i < pLineBuffer->count; i++)
        inputLength += strlen(pLineBuffer->pLines[i]);

    struct ByteBuffer *pByteBufferOut = allocByteBuffer(headerLength + inputLength + 1);

    // Insert header to buffer
    strcpy(pByteBufferOut->buffer, header);

    // Reverse code into buffer
    while (i != 0)
    {
        strcat(pByteBufferOut->buffer, pLineBuffer->pLines[--i]);
    }

    // Return the ByteBuffer
    return pByteBufferOut;
}

/**
 * @brief 
 * 
 * @param file 
 * @return struct LineBuffer* 
 * 
 * @warning Make sure to free LineBuffer!
 */
static inline struct LineBuffer* readFileIntoLineBuffer(const char* file)
{
    // Open inFile

    // Read inFile lines into LineBuffer

    // Close inFile

    // Return
}

static inline void writeByteBufferToFile(const char* file, const ByteBuffer* byteBuffer)
{
    // Open file

    // Write to file

    // Close file

    // Return
}

/**
 * @brief Generates a file from the inFile, which is reverted.
 * 
 * @return RESULT OK or FAIL
 * 
 * @warning NOT thread safe - Uses unlocked IO functions!
 */
RESULT gCodeRevert(const char* inFilename, const char* outFilename)
{
    // Read inFile to LineBuffer -readFileIntoLineBuffer()

    // Create and fill a ByteBuffer with header and reverted code -fillOutBuffer()

    // Write the whole ByteBuffer to outFile -writeByteBufferToFile()

    // Free everything

    return OK;
}


#endif // GCODEREWIND_H