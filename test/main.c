/* \copyright 2017 Zorxx Software. All rights reserved.
 * \license This file is released under the MIT License. See the LICENSE file for details.
 * \brief Embedded XML Parser
 */
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "saxml.h"

static void HandleTag(void *cookie, const char *szString)
{
    fprintf(stderr, "tagHandler: '%s'\n", szString);
}

static void HandleTagEnd(void *cookie, const char *szString)
{
    fprintf(stderr, "tagEndHandler: '%s'\n", szString);
}

static void HandleParameter(void *cookie, const char *szString)
{
    fprintf(stderr, "parameterHandler: '%s'\n", szString);
}

static void HandleContent(void *cookie, const char *szString)
{
    fprintf(stderr, "contentHandler: '%s'\n", szString);
}

static void HandleAttribute(void *cookie, const char *szString)
{
    fprintf(stderr, "attributeHandler: '%s'\n", szString);
}

int main(int argc, char *argv[])
{
    const char *filename;
    FILE *xml;
    void *saxml;
    tSaxmlContext saxml_context;

    if(argc < 2)
    {
        fprintf(stderr, "%s [xml file]\n", argv[0]);
        return -1;
    }
    filename = argv[1];

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
    saxml = saxml_Initialize(&saxml_context, 256);

    while(!feof(xml))
        saxml_HandleCharacter(saxml, (const uint8_t) fgetc(xml));
    fclose(xml);

    return 0;
}
