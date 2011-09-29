#ifndef __DIGIDOC_STACK_H__
#define __DIGIDOC_STACK_H__
//==================================================
// FILE:	DigiDocStack.h
// PROJECT:     Digi Doc
// DESCRIPTION: Digi Doc functions for simple stack
//  to keep track of xml parsing
// AUTHOR:  Veiko Sinivee, S|E|B IT Partner Estonia
//==================================================
// Copyright (C) AS Sertifitseerimiskeskus
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// GNU Lesser General Public Licence is available at
// http://www.gnu.org/copyleft/lesser.html
//==========< HISTORY >=============================
//      09.09.2004      Veiko Sinivee
//                      Creation
//==================================================

#include <libxml/xmlreader.h>
#include <libdigidoc/DigiDocDefs.h>

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct ElementEntry_st {
  xmlChar* tag;     // xml elements tag
  xmlChar* prefix;  // namespace local prefix
  xmlChar* nsUri;   // namespace URI
  // some important atributes
  xmlChar* id;      // attribute "Id"
  xmlChar* uri;     // attribute "URI"
  xmlChar* content; // attribute "ContentType"
  void* pNext;      // next element in list/stack
} ElementEntry;

//--------------------------------------------------
// Finds the last element on stack
// reader - XML reader cursor to current node
// pStack - address of stack begin. This one elemnt 
// must exist, but might be initially empty
// return error code or ERR_OK
//--------------------------------------------------
ElementEntry* ddocStackFindEnd(ElementEntry* pStack);

//--------------------------------------------------
// Push a new element info onto stack
// reader - XML reader cursor to current node
// pStack - address of stack begin. This one elemnt 
// must exist, but might be initially empty
// pLastElem - address for new elements pointer. 
//   If not NULL then will be used to return the
//   newly allocated elemnt, so you don't have to
//   do a new search.
// return error code or ERR_OK
//--------------------------------------------------
int ddocStackPushElement(ElementEntry* pStack, xmlTextReaderPtr reader,
			 ElementEntry** pLastElem);

//--------------------------------------------------
// Push a new element info onto stack
// tagName - elements tag name, Possibly with ns-prefix
// atts - array of atributa names and values
// pStack - address of stack begin. This one elemnt 
// must exist, but might be initially empty
// pLastElem - address for new elements pointer. 
//   If not NULL then will be used to return the
//   newly allocated elemnt, so you don't have to
//   do a new search.
// return error code or ERR_OK
//--------------------------------------------------
int ddocStackPushElementSAX(ElementEntry* pStack, const xmlChar* tagName,
			    const xmlChar** atts, ElementEntry** pLastElem);

//--------------------------------------------------
// Pop the last element from the stack
// pStack - address of stack begin. This one elemnt 
// must exist, and will never be deleted. 
// bCleanup - flag: 1=cleanup the whole stack, 0=just the last element
// return error code or ERR_OK
// pLastElem - address for new elements pointer. 
//   If not NULL then will be used to return the
//   last element on stack.
//--------------------------------------------------
int ddocStackPopElement(ElementEntry* pStack, int bCleanup, 
			ElementEntry** pLastElem);

//--------------------------------------------------
// Retrieve the current/last/stack top elements tag (localname)
// pStack - address of stack begin.
//--------------------------------------------------
const xmlChar* ddocStackCurrentTag(ElementEntry* pStack);

//--------------------------------------------------
// Retrieve the current/last/stack top elements ns prefix
// pStack - address of stack begin.
//--------------------------------------------------
const xmlChar* ddocStackCurrentNsPrefix(ElementEntry* pStack);

//--------------------------------------------------
// Retrieve the current/last/stack top elements ns prefix
// pStack - address of stack begin.
//--------------------------------------------------
const xmlChar* ddocStackCurrentNsUri(ElementEntry* pStack);

//--------------------------------------------------
// Checks if there is a parent element with the given
// localname on the stack.
// pStack - address of stack begin.
// return 1 if there is such a parent elem or 0 if not.
//--------------------------------------------------
int ddocStackHasParentWithName(ElementEntry* pStack, 
			       const xmlChar* parentsName, ElementEntry* pCurrElem);

//--------------------------------------------------
// Checks if there is a parent element with the given
// localname on the stack.
// pStack - address of stack begin.
// return 1 if there is such a parent elem or 0 if not.
//--------------------------------------------------
ElementEntry* ddocStackGetParentWithName(ElementEntry* pStack, 
					 const xmlChar* parentsName, ElementEntry* pCurrElem);



#ifdef  __cplusplus
}
#endif

#endif // __DIGIDOC_STACK_H__
