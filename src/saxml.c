/*! \copyright 2017-2025 Zorxx Software. All rights reserved.
 *  \license This file is released under the MIT License. See the LICENSE file for details.
 *  \brief Embedded XML Parser
 */
#include "saxml/saxml.h"
#include "helpers.h"
#include <stddef.h> /* for NULL */
#ifndef SAXML_NO_MALLOC
#include <stdlib.h> /* malloc and free */
#endif

#if defined(SAXML_NO_MALLOC) && !defined(SAXML_MAX_STRING_LENGTH)
#error saxml: Must define SAXML_MAX_STRING_LENGTH
#endif

#define SAXML_MIN_STRING_SIZE   2

typedef int (*pfnParserStateHandler)(void *context, const char character);

typedef struct
{
    tSaxmlContext *user;

    pfnParserStateHandler pfnHandler;

    int bInitialize; /* true for first call into a state */
    int bInQuotedText; /* true if we're within quotes */
    int bAllowTruncatedStrings; /* true if truncated parsing results are acceptable */

    char *buffer;
    uint32_t maxStringSize;
    uint32_t length;
} tParserContext;
#ifdef SAXML_NO_MALLOC
static tParserContext g_saxmlParserContext;
static char g_saxmlBuffer[SAXML_MAX_STRING_LENGTH];
#endif

static int state_Begin(void *context, const char character);
static int state_StartTag(void *context, const char character);
static int state_TagName(void *context, const char character);
static int state_TagContents(void *context, const char character);
static int state_EndTag(void *context, const char character);
static int state_EmptyTag(void *context, const char character);
static int state_Attribute(void *context, const char character);

#define ChangeState(ctxt, state) \
    (ctxt)->pfnHandler = state;  \
    (ctxt)->bInitialize = 1;

static __inline int ContextBufferAddChar(tParserContext *ctxt, const char character)
{
    if((ctxt)->length < ((ctxt)->maxStringSize) - 2)
    {
        (ctxt)->buffer[(ctxt)->length] = character;
        ++((ctxt)->length);
        return 0;
    }

    /* string truncated */
    if(ctxt->bAllowTruncatedStrings)
        return 0;
    else
        return SAXML_ERROR_BUFFER_OVERFLOW;
}

#define CallHandler(ctxt, handlerName)                                   \
    if(NULL != (ctxt)->user->handlerName && (ctxt)->length > 0)          \
    {                                                                    \
        (ctxt)->buffer[(ctxt)->length] = '\0';                           \
        ++((ctxt)->length);                                              \
        (ctxt)->user->handlerName((ctxt)->user->cookie, (ctxt)->buffer); \
    }

/* ---------------------------------------------------------------------------------------------
 * Exported Functions
 */

tSaxmlParser saxml_Initialize(tSaxmlContext *context, const uint32_t maxStringSize)
{
    tParserContext *ctxt;

    if(maxStringSize < SAXML_MIN_STRING_SIZE)
        return NULL;
    if(NULL == context)
        return NULL;

    #ifdef SAXML_NO_MALLOC
    ctxt = &g_saxmlParserContext;
    ctxt->buffer = g_saxmlBuffer;
    if(NULL != ctxt->user)
       return NULL; /* already in use */
    #else
    ctxt = (tParserContext *) malloc(sizeof(*ctxt));
    if(NULL == ctxt)
        return NULL;

    ctxt->buffer = (char *) malloc(maxStringSize);
    if(NULL == ctxt->buffer)
    {
        free(ctxt);
        return NULL;
    }
    #endif

    ctxt->user = context;
    ctxt->length = 0;
    ctxt->maxStringSize = maxStringSize;
    ctxt->bAllowTruncatedStrings = 0;
    ChangeState(ctxt, state_Begin);

    return (tSaxmlParser) ctxt;
}

void saxml_Deinitialize(tSaxmlParser parser)
{
    #ifndef SAXML_NO_MALLOC
    tParserContext *ctxt = (tParserContext *) parser;
    if(NULL != ctxt)
    {
        if(NULL != ctxt->buffer)
            free(ctxt->buffer);
        free(ctxt);
    }
    #else
    UNUSED(parser);
    #endif
}

int saxml_HandleCharacter(tSaxmlParser parser, const char character)
{
    tParserContext *ctxt = (tParserContext *) parser;
    return ctxt->pfnHandler(ctxt, character);
}

void saxml_Reset(tSaxmlParser parser)
{
    tParserContext *ctxt = (tParserContext *) parser;
    ChangeState(ctxt, state_Begin);
}

void saxml_AllowTruncatedStrings(tSaxmlParser parser, const int allow)
{
    tParserContext *ctxt = (tParserContext *) parser;
    ctxt->bAllowTruncatedStrings = (allow) ? 1 : 0;
}

/* ---------------------------------------------------------------------------------------------
 * State Handlers
 */

/* Wait for a tag start character */
static int state_Begin(void *context, const char character)
{
    tParserContext *ctxt = (tParserContext *) context;
    pfnParserStateHandler nextState = NULL;

    DBG1("[state_Begin] %c\n", character);

    if(ctxt->bInitialize)
    {
        DBG("[state_Begin] Initialize\n");
        ctxt->length = 0;
        ctxt->bInitialize = 0;
    }

    switch(character)
    {
        case '<':
           nextState = state_StartTag;
           break;
        default:
            break;
    }

    if(NULL != nextState)
    {
        ChangeState(ctxt, nextState);
    }

    return 0;
}

/* We've already found a tag start character, determine if this is start or end tag,
 *  and parse the tag name */
static int state_StartTag(void *context, const char character)
{
    tParserContext *ctxt = (tParserContext *) context;

    DBG1("[state_StartTag] %c\n", character);

    if(ctxt->bInitialize)
    {
        DBG("[state_StartTag] Initialize\n");
        ctxt->bInitialize = 0;
    }

    switch(character)
    {
        case '<': case '>':
            /* Syntax error! */
            return SAXML_ERROR_SYNTAX;
        case ' ': case '\r': case '\n': case '\t':
            /* Ignore whitespace */
            break;
        case '/':
            ChangeState(ctxt, state_EndTag);
            break;
        default:
            ctxt->buffer[0] = character;
            ctxt->length = 1;
            ChangeState(ctxt, state_TagName);
            break;
    }

    return 0;
}

static int state_TagName(void *context, const char character)
{
    tParserContext *ctxt = (tParserContext *) context;
    pfnParserStateHandler nextState = NULL;

    DBG1("[state_TagName] %c\n", character);

    if(ctxt->bInitialize)
    {
        /* Expect one character in the buffer; the start of the tag name from the previous state*/
        DBG("[state_TagName] Initialize\n");
        ctxt->bInitialize = 0;
    }

    switch(character)
    {
        case ' ': case '\r': case '\n': case '\t':
            /* Tag name complete, whitespace indicates tag attribute */
            nextState = state_Attribute;
            break;
        case '/':
            nextState = state_EmptyTag;
            break;
        case '>':
            nextState = state_TagContents; /* Done with tag, contents may follow */
            break;
        default:
            if(ContextBufferAddChar(ctxt, character) != 0)
               return SAXML_ERROR_BUFFER_OVERFLOW;
            break;
    }

    if(NULL != nextState)
    {
        CallHandler(ctxt, tagHandler);
        ChangeState(ctxt, nextState);
    }

    return 0;
}

static int state_EmptyTag(void *context, const char character)
{
    tParserContext *ctxt = (tParserContext *) context;
    pfnParserStateHandler nextState = NULL;

    DBG1("[state_EmptyTag] %c\n", character);

    if(ctxt->bInitialize)
    {
        /* We need to keep the buffer as-is, since it contains the tag name */
        DBG("[state_EmptyTag] Initialize\n");
        ctxt->bInitialize = 0;
    }

    switch(character)
    {
        case '>':
            nextState = state_TagContents;
            break;
        default:
            break;
    }

    if(NULL != nextState)
    {
        CallHandler(ctxt, tagEndHandler);
        ChangeState(ctxt, nextState);
    }

    return 0;
}

static int state_TagContents(void *context, const char character)
{
    tParserContext *ctxt = (tParserContext *) context;
    pfnParserStateHandler nextState = NULL;

    DBG1("[state_TagContents] %c\n", character);

    if(ctxt->bInitialize)
    {
        DBG("[state_TagContents] Initialize\n");
        ctxt->length = 0;
        ctxt->bInitialize = 0;
        ctxt->bInQuotedText = 0;
    }

    switch(character)
    {
        case '<':
            if(0 == ctxt->bInQuotedText)
                nextState = state_StartTag;
            else
            {
                if(ContextBufferAddChar(ctxt, character) != 0)
                    return SAXML_ERROR_BUFFER_OVERFLOW;
            }
            break;
        case '"':
            ctxt->bInQuotedText ^= 1;
            if(ContextBufferAddChar(ctxt, character) != 0)
                return SAXML_ERROR_BUFFER_OVERFLOW;
            break;
        case ' ': case '\r': case '\n': case '\t':
            if(0 == ctxt->bInQuotedText && 0 == ctxt->length)
                break; /* Ignore leading whitespace */
            if(ContextBufferAddChar(ctxt, character) != 0)
               return SAXML_ERROR_BUFFER_OVERFLOW;
            break;
        default:
            if(ContextBufferAddChar(ctxt, character) != 0)
               return SAXML_ERROR_BUFFER_OVERFLOW;
            break;
    }

    if(NULL != nextState)
    {
        CallHandler(ctxt, contentHandler);
        ChangeState(ctxt, nextState);
    }

    return 0;
}

static int state_Attribute(void *context, const char character)
{
    tParserContext *ctxt = (tParserContext *) context;
    pfnParserStateHandler nextState = NULL;

    DBG1("[state_Attribute] %c\n", character);

    if(ctxt->bInitialize)
    {
        DBG("[state_Attribute] Initialize\n");
        ctxt->length = 0;
        ctxt->bInitialize = 0;
        ctxt->bInQuotedText = 0;
    }

    switch(character)
    {
        case ' ': case '\r': case '\n': case '\t':
            if(0 == ctxt->bInQuotedText)
            {
                if(0 != ctxt->length)
                    nextState = state_Attribute;
            }
            else
            {
                if(ContextBufferAddChar(ctxt, character) != 0)
                    return SAXML_ERROR_BUFFER_OVERFLOW;
            }
            break;
        case '/':
            if(0 == ctxt->bInQuotedText)
            {
                /* Handle the case where an attribute is included in an empty tag,
                   and the attribute name/value has no trailing whitespace
                   prior to the empty tag terminator. */
                if (ctxt->length > 0)
                {
                    CallHandler(ctxt, attributeHandler);
                    ctxt->length = 0;
                }

                /* We've found an empty tag that contains at least one attribute.
                   Since the buffer containing the tag name is long-gone (the attribute
                   is now in the parser's string buffer), we don't have a way to get it
                   back. In order to generate a "tagEnd" event, store a dummy string
                   containing a single space character (which isn't a valid tag name),
                   which will be provided to the tagEndHandler callback. */
                if(ContextBufferAddChar(ctxt, ' ') != 0)
                    return SAXML_ERROR_BUFFER_OVERFLOW;
                nextState = state_EmptyTag;
            }
            else
            {
                if(ContextBufferAddChar(ctxt, character) != 0)
                    return SAXML_ERROR_BUFFER_OVERFLOW;
            }
            break;
        case '>':
            if(0 == ctxt->bInQuotedText)
                nextState = state_TagContents; /* Done with tag, contents may follow */
            else
            {
                if(ContextBufferAddChar(ctxt, character) != 0)
                    return SAXML_ERROR_BUFFER_OVERFLOW;
            }
            break;
        case '"':
            ctxt->bInQuotedText ^= 1;
            ContextBufferAddChar(ctxt, character);
            break;
        default:
            if(ContextBufferAddChar(ctxt, character) != 0)
               return SAXML_ERROR_BUFFER_OVERFLOW;
            break;
    }

    if(NULL != nextState)
    {
        if(nextState != state_EmptyTag)
        {
            CallHandler(ctxt, attributeHandler);
        }
        ChangeState(ctxt, nextState);
    }

    return 0;
}

static int state_EndTag(void *context, const char character)
{
    tParserContext *ctxt = (tParserContext *) context;
    pfnParserStateHandler nextState = NULL;

    DBG1("[state_EndTag] %c\n", character);

    if(ctxt->bInitialize)
    {
        DBG("[state_EndTag] Initialize\n");
        ctxt->length = 0;
        ctxt->bInitialize = 0;
    }

    switch(character)
    {
        case '<': /* syntax error */
            return SAXML_ERROR_SYNTAX;
        case ' ': case '\r': case '\n': case '\t':
            /* Ignore whitespace */
            break;
        case '>':
            nextState = state_TagContents;
            break;
        default:
            if(ContextBufferAddChar(ctxt, character) != 0)
               return SAXML_ERROR_BUFFER_OVERFLOW;
            break;
    }

    if(NULL != nextState)
    {
        CallHandler(ctxt, tagEndHandler);
        ChangeState(ctxt, nextState);
    }

    return 0;
}
