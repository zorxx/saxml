/* \copyright 2017 Zorxx Software. All rights reserved.
 * \license This file is released under the MIT License. See the LICENSE file for details.
 * \brief Embedded XML Parser
 */
#ifndef SAXML_H
#define SAXML_H

#include <stdint.h>

typedef void (*pfnStringHandler)(void *cookie, const uint8_t *szString);

typedef struct
{
    void *cookie;
    pfnStringHandler tagHandler;
    pfnStringHandler tagEndHandler;
    pfnStringHandler parameterHandler;
    pfnStringHandler contentHandler;
    pfnStringHandler attributeHandler;
} tSaxmlContext;

typedef void *tSaxmlParser;

/*! \brief Create an XML parsing instance
 *  \param context Pointer to structure containing pointers to parsing handling
 *                 functions, which are called when parsing events occur.
 *  \param maxStringSize Maximum number of characters for parsed strings. If the 
 *                       parser encounters a string longer than this, it will be
 *                       truncated to this length when provided via the szString
 *                       parameter of the corresponding pfnStringHandler function.
 *  \return parser instance
 */
tSaxmlParser saxml_Initialize(tSaxmlContext *context, const uint32_t maxStringSize);

/*! \brief Destroy an XML parsing instance
 *  \param parser tSaxmlParser instance, obtained from a call to saxml_Initialize
 */
void saxml_Deinitialize(tSaxmlParser parser);

/*! \brief Provide a single character to the XML parser. Based on the processing of this
 *         character, one of the pfnStringHandler functions may be called.
 *  \param parser tSaxmlParser instance, obtained from a call to saxml_Initialize
 *  \param character Character to process
 */
void saxml_HandleCharacter(tSaxmlParser parser, const uint8_t character);

/*! \brief Reset the parser to its initial state 
 *  \param parser tSaxmlParser instance, obtained from a call to saxml_Initialize
 */
void saxml_Reset(tSaxmlParser parser);

#endif /* SAXML_H */
