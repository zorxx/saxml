# saxml
Embedded XML Parser

saxml is a truly small event-driven XML parser designed for use in embedded/microcontroller applications. Since saxml is a SAX XML parser (see https://en.wikipedia.org/wiki/Simple_API_for_XML), the parser has a very small memory footprint (there's no XML document stored on the heap). Instead, the XML document is streamed to the parser a single character at a time. As the parser encounters interesting events (such as a start tag, end tag, attribute, etc.), the parser executes callback functions which are registered by the calling application. This allows the calling application to perform application-specific operations based on XML parsing events.

saxml performs no validation of the XML document

## Example #1 (test.xml):

XML Document: 
```
<begin  > 
<second_begin>
<nothing_much/>
 content
</second_begin>
</begin>
```
Callbacks executed:

```
tagHandler: 'begin'
tagHandler: 'second_begin'
tagHandler: 'nothing_much'
tagEndHandler: 'nothing_much'
contentHandler: 'content
'
tagEndHandler: 'second_begin'
tagEndHandler: 'begin'

```

## Example #2 (test2.xml):

XML Document: 
```
tagHandler: 'begin'
tagHandler: 'second_begin'
tagHandler: 'nothing_much'
tagEndHandler: 'nothing_much'
contentHandler: 'content
'
tagEndHandler: 'second_begin'
tagEndHandler: 'begin'
```

Callbacks executed:
```
tagHandler: 'begin'
tagHandler: 'second_begin'
attributeHandler: 'yes'
attributeHandler: 'no="hello"'
tagHandler: 'nothing_much'
tagEndHandler: 'nothing_much'
contentHandler: 'content'
tagEndHandler: 'second_begin'
contentHandler: 'more content goes here
'
tagEndHandler: 'begin'
```
