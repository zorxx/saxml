/* \copyright 2017-2025 Zorxx Software. All rights reserved.
 * \license This file is released under the MIT License. See the LICENSE file for details.
 * \brief Embedded XML Parser
 */
#include "saxml/saxml.h"
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

int main(int argc, char *argv[])
{
    const char *filename;
    char *compareBuffer = NULL;
    FILE *xml;
    void *saxml;
    tSaxmlContext saxml_context;
    uint32_t max_string_size = 256;
    int result = -1;

    if(argc < 2)
    {
        fprintf(stderr, "%s [xml file]\n", argv[0]);
        return -1;
    }
    filename = argv[1];

    if(argc >= 3)
    {
       ssize_t length;
       FILE *pFile = fopen(argv[2], "rb");
       if(NULL == pFile)
       {
          fprintf(stderr, "Error opening compare file '%s'\n", argv[2]);
          return -1;
       }
       fseek(pFile, 0, SEEK_END);
       length = ftell(pFile);
       fseek(pFile, 0, SEEK_SET);
       compareBuffer = malloc(length+1);
       if(fread(compareBuffer, length, 1, pFile) != 1)
       {
          fprintf(stderr, "Failed to read %ld bytes from compare file '%s'\n", length, argv[2]);
          return -1;
       }
       fclose(pFile);
       compareBuffer[length] = '\0';
       PRINT = print_buffer;
    }

    xml = fopen(filename, "r");
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
