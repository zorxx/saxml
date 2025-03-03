/* \copyright 2017-2025 Zorxx Software. All rights reserved.
 * \license This file is released under the MIT License. See the LICENSE file for details.
 * \brief Embedded XML Parser
 */
#include "saxml/saxml.h"
#include <getopt.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define UNUSED(x) (void)x;

static char *resultBuffer = NULL;
static uint32_t resultLength = 0;

typedef void (*pfnPrintHandler)(const char *event, const char *param);
static void print_console(const char *event, const char *param)
{
   printf("%s: '%s'\n", event, param);
}
static void print_buffer(const char *event, const char *param)
{
   uint32_t length = resultLength + strlen(event) + strlen(param) + 6;
   resultBuffer = realloc(resultBuffer, length);
   resultLength += sprintf(&resultBuffer[resultLength], "%s: '%s'\n", event, param);
}
static pfnPrintHandler PRINT = print_console;

static char *LoadFile(const char *filename)
{
    ssize_t length;
    char *resultBuffer = NULL;
    FILE *pFile = fopen(filename, "rb");
    if(NULL == pFile)
    {
       fprintf(stderr, "Error opening file '%s'\n", filename);
       return NULL;
    }

    fseek(pFile, 0, SEEK_END);
    length = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);
    resultBuffer = (char *) malloc(length+1);
    if(fread(resultBuffer, length, 1, pFile) != 1)
    {
       fprintf(stderr, "Failed to read %ld bytes from file '%s'\n", length, filename);
       return NULL; 
    }
    fclose(pFile);
    resultBuffer[length] = '\0';
    return resultBuffer;
}

/* -----------------------------------------------------------------------------------------------
 * Parse Event Handlers
 */

static void HandleTag(void *cookie, const char *szString)
{
    UNUSED(cookie);
    PRINT("tagHandler", szString);
}

static void HandleTagEnd(void *cookie, const char *szString)
{
    UNUSED(cookie);
    PRINT("tagEndHandler", szString);
}

static void HandleParameter(void *cookie, const char *szString)
{
    UNUSED(cookie);
    PRINT("parameterHandler", szString);
}

static void HandleContent(void *cookie, const char *szString)
{
    UNUSED(cookie);
    PRINT("contentHandler", szString);
}

static void HandleAttribute(void *cookie, const char *szString)
{
    UNUSED(cookie);
    PRINT("attributeHandler", szString);
}

/* -----------------------------------------------------------------------------------------------
 * main
 */

 #define DEFAULT_MAX_STRING_LENGTH 256 /* characters */
 #define PROGRAM_OPTIONS "c:s:?"
 #define STR(x) #x

 void DisplayHelp(const char* prog)
 {
    fprintf(stderr, "%s [xml file] <" PROGRAM_OPTIONS ">\n", prog);
    fprintf(stderr, "   c [compare file]   File to compare against test result\n");
    fprintf(stderr, "   s [length]         Maximum string length, in characters (default: " STR(DEFAULT_MAX_STRING_LENGTH) ")\n");
 }

int main(int argc, char *argv[])
{
    const char *filename;
    char *compareBuffer = NULL;
    int showHelp = 0;
    FILE *xml;
    void *saxml;
    tSaxmlContext saxml_context;
    uint32_t max_string_size = DEFAULT_MAX_STRING_LENGTH;
    int result = -1;
    char arg;

    if(argc < 2)
       showHelp = 1;
    else
    {
        filename = argv[1];
        while((arg = getopt(argc-1, &argv[1], PROGRAM_OPTIONS)) != -1)
        {
            switch(arg)
            {
                case 'c':
                    compareBuffer = LoadFile(optarg);
                    if(NULL == compareBuffer)
                        showHelp = 1;
                    break;
                case 's': max_string_size = strtoul(optarg, NULL, 10); break;
                default: showHelp = 1; break;
            }
        }
    }

    if(showHelp)
    {
        DisplayHelp(argv[0]);
        return -1;
    }

    if(NULL != compareBuffer)
       PRINT = print_buffer;

    xml = fopen(filename, "rb");
    if(NULL == xml)
    {
        fprintf(stderr, "Error opening XML file '%s' (%d, %s)\n",
            filename, errno, strerror(errno));
        return -1;
    }

    saxml_context.cookie = NULL;
    saxml_context.tagHandler = HandleTag;
    saxml_context.tagEndHandler = HandleTagEnd;
    saxml_context.parameterHandler = HandleParameter;
    saxml_context.contentHandler = HandleContent;
    saxml_context.attributeHandler = HandleAttribute;
    saxml = saxml_Initialize(&saxml_context, max_string_size);
    if(NULL == saxml)
    {
        fprintf(stderr, "Failed to initialize saxml\n");
        return -1;
    }

    while(!feof(xml))
    {
        /* Parse one character at a time */
        if(saxml_HandleCharacter(saxml, (const uint8_t) fgetc(xml)) != 0)
        {
            printf("Parsing failed\n");
            return -1;
        }
    }
    fclose(xml);
    printf("Parse successful\n");

    if(compareBuffer == NULL)
       result = 0;
    else
    {
        if(strcmp(compareBuffer, resultBuffer) == 0)
        {
            printf("Success\n");
            result = 0;
        }
        else
        {
            printf("Failed, mismatch\n");
            printf("----- Expected:\n%s\n", compareBuffer);
            printf("----- Received:\n%s\n", resultBuffer);
        }
        free(compareBuffer);
    }

    return result;
}
