
#ifndef GCODEREWIND_H
#define GCODEREWIND_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#define GCODE_INITIAL_LINE_BUFFER_SIZE 2
#define GCODE_LINE_BUFFER_LINE_LENGTH 80

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
 * @brief Container for an array of lines.
 * 
 * @note Heap allocations should be made with LineBuffer allocator.
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

    /**
     * @brief Describes how many lines of the "count" lines that have been allocated.
     * 
     * @example for (int i=0; i < linesAllocated; i++) free(pLines[i]);
     * 
     */
    size_t linesAllocated;
};

/**
 * @brief Allocates a new LineBuffer with initialCount lines and zero allocated lines.
 * 
 * @param initialCount: 
 * @return struct LineBuffer* or NULL if failing.
 */
static inline struct LineBuffer* allocLineBuffer(const size_t initialCount)
{
    struct LineBuffer *lineBuffer = (struct LineBuffer*) malloc(sizeof(struct LineBuffer));
    if (lineBuffer == NULL)
    {
        fputs("Allocation for the LineBuffer struct failed", stderr);
        return NULL;
    }

    // Allocate and check allocation before proceeding
    lineBuffer->pLines = (char**) malloc(sizeof(char*) * initialCount);
    if (lineBuffer->pLines == NULL)
    {
        fprintf(stderr, "\nLineBuffer allocation of %lu lines failed!\n", initialCount);
        return NULL;
    }
    lineBuffer->count   = initialCount;
    lineBuffer->linesAllocated = 0;

    return lineBuffer;
}

/**
 * @brief Reallocates to fit more lines in the given LineBuffer. If nCount is 0 it just doubles the size.
 * 
 * @param lineBuffer 
 * @param nCount 
 * @return int 
 */
static inline int reallocLineBuffer(struct LineBuffer* lineBuffer, const size_t nCount)
{
    // Allocate the double size and update the LineBuffer
    void *nptr;
    if (nCount == 0)
    {
        const size_t newCount = lineBuffer->count * 2;
        nptr = (char**) realloc(lineBuffer->pLines, newCount * sizeof(char*));
        if (nptr == NULL)
        {
            fprintf(stderr, "Reallocation of LineBuffer failed reallocation of %lu lines\nErrorMsg: %s\n", newCount, strerror(errno));
            return -1;
        }
        lineBuffer->pLines = (char**) nptr;
        lineBuffer->count = newCount;
    }
    
    // Allocate the given size to the LineBuffer and update
    else
    {
        nptr = (char**) realloc(lineBuffer->pLines, nCount * sizeof(char*));
        if (nptr == NULL)
        {
            fprintf(stderr, "Reallocation of LineBuffer failed reallocation of %lu lines\nErrorMsg: %s\n", nCount, strerror(errno));
            return -1;
        }
        lineBuffer->pLines = (char**) nptr;
        lineBuffer->count = nCount;
    }

    return 0;
}

/**
 * @brief Function to free LineBuffer. Should only be used if everything is heap allocated.
 * 
 * @param lineBuffer: 
 * 
 */
static inline void freeLineBuffer(struct LineBuffer *lineBuffer)
{ 
    for (uint_fast32_t i=0; i < lineBuffer->linesAllocated; i++)
        free(lineBuffer->pLines[i]);
    free(lineBuffer->pLines); 
    free(lineBuffer); 
}

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
static inline void insertnHeader(struct ByteBuffer *byteBuffer)
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
static inline struct ByteBuffer* fillOutBuffer(const struct LineBuffer* pLineBuffer)
{
    // Declare header and size
    char header[1024]; insertHeader(header);
    const uint32_t headerLength = strlen(header);

    // Create output buffer
    uint32_t inputLength = 0;
    uint_fast32_t i;
    for (i=0; i < pLineBuffer->linesAllocated; i++)
        inputLength += strlen(pLineBuffer->pLines[i]);

    struct ByteBuffer *pByteBufferOut = allocByteBuffer(headerLength + inputLength + 1);

    // Insert header to buffer
    strcpy(pByteBufferOut->buffer, header);

    // Reverse code into buffer
    void *pos = pByteBufferOut->buffer + headerLength;
    while (i != 0)
    {
        pos = (char*) memccpy(pos, pLineBuffer->pLines[--i], '\0', GCODE_LINE_BUFFER_LINE_LENGTH) - 1;
    }

    // Return the ByteBuffer
    return pByteBufferOut;
}

/**
 * @brief Read file into LineBuffer line by line.
 * 
 * @param file Filename to read in.
 * @return struct LineBuffer* A completely heap allocated LineBuffer struct.
 * 
 * @warning Make sure to free LineBuffer!
 */
static inline struct LineBuffer* readFileIntoLineBuffer(const char* file)
{
    // Open inFile
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        fputs("Couldn't open file!", stderr);
        return NULL;
    }

    // Create LineBuffer with initial size
    struct LineBuffer *pLineBuffer = allocLineBuffer(GCODE_INITIAL_LINE_BUFFER_SIZE);
    if (pLineBuffer == NULL)
    {
        fclose(fp);
        return NULL;
    }

    // Read inFile lines into LineBuffer
    char *readRes; 
    char tmpLine[GCODE_LINE_BUFFER_LINE_LENGTH]; 
    uint_fast32_t index = 0;
    while (1)
    {
        // Check if we should allocate more lines
        if (pLineBuffer->linesAllocated == pLineBuffer->count)
        {
            if ( reallocLineBuffer(pLineBuffer, 0) != 0 )
            {
                freeLineBuffer(pLineBuffer);
                fclose(fp);
                return NULL;
            }
        }
        
        // Read into temp buffer
        readRes = fgets(tmpLine, GCODE_LINE_BUFFER_LINE_LENGTH, fp);

        // Check if there was a line
        if (readRes == NULL)
            break;

        // Copy into LineBuffer
        pLineBuffer->pLines[index] = (char*) malloc(GCODE_LINE_BUFFER_LINE_LENGTH * sizeof(char));
        if (pLineBuffer->pLines[index] == NULL)
        {
            fprintf(stderr, "\nAllocation of line %lu failed!\n", index);
            free(pLineBuffer->pLines[index]);
            freeLineBuffer(pLineBuffer);
            fclose(fp);
            return NULL;
        }

        pLineBuffer->linesAllocated++;
        strcpy(pLineBuffer->pLines[index], tmpLine);

        // Increment
        index++;
    };

    // Close inFile
    fclose(fp);

    // Return
    return pLineBuffer;
}

/**
 * @brief Writes the whole ByteBuffer to the given file.
 * 
 * @param file: File to Write ByteBuffer to.
 * @param byteBuffer: ByteBuffer.
 * @return int 0 if everything went OK.
 */
static inline int writeByteBufferToFile(const char* file, const struct ByteBuffer* byteBuffer)
{
    // Open file
    FILE *fp = fopen(file, "w+");
    if (fp == NULL)
    {
        fputs("Error couldn't open file which to write to!\n", stderr);
        return -1;
    }

    // Write to file
    fwrite(byteBuffer->buffer, sizeof(char), byteBuffer->size, fp);

    // Close file
    fclose(fp);

    // Return
    return 0;
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
    struct LineBuffer *pLineBuffer = readFileIntoLineBuffer(inFilename);
    if (pLineBuffer == NULL)
        return FAIL;

    // Create and fill a ByteBuffer with header and reverted code -fillOutBuffer()
    struct ByteBuffer *pByteBuffer = fillOutBuffer(pLineBuffer);

    // Write the whole ByteBuffer to outFile -writeByteBufferToFile()
    writeByteBufferToFile(outFilename, pByteBuffer);

    // Free everything
    freeLineBuffer(pLineBuffer);
    freeByteBuffer(pByteBuffer);

    return OK;
}

/* ------------------------ Testing area ------------------------ */

static inline struct LineBuffer* readFileIntoLineBufferTry(const char* file)
{
    // Open inFile
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        fputs("Couldn't open file!", stderr);
        return NULL;
    }

    // Move fp until first layer
    char *readRes; 
    char tmpLine[GCODE_LINE_BUFFER_LINE_LENGTH]; 
    while (1)
    {
        // Get the next line
        readRes = fgets(tmpLine, GCODE_LINE_BUFFER_LINE_LENGTH, fp);

        // Check if there was a line
        if (readRes == NULL)
        {
            fprintf(stderr, "\nError: Didn't find the first layer!\n");
            fclose(fp);
            return NULL;
        }

        // Check if we reached the start of the first layer
        if (strstr(tmpLine, "Layer #0") != NULL)
            break;
    }

    // Create LineBuffer with initial size
    struct LineBuffer *pLineBuffer = allocLineBuffer(GCODE_INITIAL_LINE_BUFFER_SIZE);
    if (pLineBuffer == NULL)
    {
        fclose(fp);
        return NULL;
    }

    // Read into LineBuffer
    uint_fast32_t index = 0;
    while (1)
    {
        // Read into temp buffer
        readRes = fgets(tmpLine, GCODE_LINE_BUFFER_LINE_LENGTH, fp);

        // Check if there was a line
        if (readRes == NULL)
            break;
        // Check if we should ignore the line
        else if (*tmpLine == '\n' || *tmpLine == ';')
            continue;

        // Check if we should allocate more lines
        if (pLineBuffer->linesAllocated == pLineBuffer->count)
        {
            if ( reallocLineBuffer(pLineBuffer, 0) != 0 )
            {
                freeLineBuffer(pLineBuffer);
                fclose(fp);
                return NULL;
            }
        }

        // Copy into LineBuffer
        pLineBuffer->pLines[index] = (char*) malloc(GCODE_LINE_BUFFER_LINE_LENGTH * sizeof(char));
        if (pLineBuffer->pLines[index] == NULL)
        {
            fprintf(stderr, "\nAllocation of line %lu failed!\n", index);
            free(pLineBuffer->pLines[index]);
            freeLineBuffer(pLineBuffer);
            fclose(fp);
            return NULL;
        }

        pLineBuffer->linesAllocated++;
        strcpy(pLineBuffer->pLines[index], tmpLine);

        // Increment
        index++;
    };

    // Close inFile
    fclose(fp);

    // Return
    return pLineBuffer;
}

RESULT gCodeRevertTry(const char* inFilename, const char* outFilename)
{
    // Read inFile to LineBuffer -readFileIntoLineBuffer()
    struct LineBuffer *pLineBuffer = readFileIntoLineBufferTry(inFilename);
    if (pLineBuffer == NULL)
        return FAIL;

    // Create and fill a ByteBuffer with header and reverted code -fillOutBuffer()
    struct ByteBuffer *pByteBuffer = fillOutBuffer(pLineBuffer);

    // Write the whole ByteBuffer to outFile -writeByteBufferToFile()
    writeByteBufferToFile(outFilename, pByteBuffer);

    // Free everything
    freeLineBuffer(pLineBuffer);
    freeByteBuffer(pByteBuffer);

    return OK;
}


#endif // GCODEREWIND_H