//==================================================
// FILE:	DigiDocStack.c
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

#include <libdigidoc/DigiDocStack.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocError.h>
#include <libdigidoc/DigiDocDebug.h>
#include <string.h>

//--------------------------------------------------
// Finds the last element on stack
// reader - XML reader cursor to current node
// pStack - address of stack begin. This one elemnt 
// must exist, but might be initially empty
// return error code or ERR_OK
//--------------------------------------------------
ElementEntry* ddocStackFindEnd(ElementEntry* pStack)
{
  ElementEntry* pElem = pStack;
  while(pElem->pNext)
    pElem = (ElementEntry*)pElem->pNext;
  return pElem;
}

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
			 ElementEntry** pLastElem)
{
  ElementEntry* pElem;

  RETURN_IF_NULL_PARAM(reader);
  RETURN_IF_NULL_PARAM(pStack);
  pElem = ddocStackFindEnd(pStack);
  if(pElem->tag && !pElem->pNext) {  // check if this entry is used
    pElem->pNext = (ElementEntry*)malloc(sizeof(ElementEntry));
    if(!pElem)
      SET_LAST_ERROR_RETURN_CODE(ERR_BAD_ALLOC);
    memset(pElem->pNext, 0, sizeof(ElementEntry));
    pElem = (ElementEntry*)pElem->pNext;
  }
  if(!pElem)
    return ERR_NULL_POINTER;
  pElem->tag = xmlTextReaderLocalName(reader);
  pElem->prefix = xmlTextReaderPrefix(reader);
  pElem->nsUri = xmlTextReaderNamespaceUri(reader);
  // atributes
  pElem->id = xmlTextReaderGetAttribute(reader, (const xmlChar*)"Id");
  pElem->uri = xmlTextReaderGetAttribute(reader, (const xmlChar*)"URI");
  pElem->content = xmlTextReaderGetAttribute(reader, (const xmlChar*)"ContentType");
  if(pLastElem) // this may be NULL if users doesn't want the last elem.
    *pLastElem = pElem;
  return ERR_OK;
}

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
			    const xmlChar** atts, ElementEntry** pLastElem)
{
  ElementEntry* pElem;
  char *p1, *p2;
  xmlChar *n, *v;
  int i;

  ddocDebug(5, "ddocStackPushElementSAX", "tag: %s", (char*)tagName);
  RETURN_IF_NULL_PARAM(tagName)
  RETURN_IF_NULL_PARAM(pStack)
  pElem = ddocStackFindEnd(pStack);
  if(pElem->tag && !pElem->pNext) {  // check if this entry is used
    pElem->pNext = (ElementEntry*)malloc(sizeof(ElementEntry));
    if(!pElem)
      SET_LAST_ERROR_RETURN_CODE(ERR_BAD_ALLOC);
    memset(pElem->pNext, 0, sizeof(ElementEntry));
    pElem = (ElementEntry*)pElem->pNext;
  }
  if(!pElem)
    return ERR_NULL_POINTER;
  p1 = (char*)strdup((char*)tagName);
  if(!p1)
    SET_LAST_ERROR_RETURN_CODE(ERR_BAD_ALLOC)
  // check for ns-prefix
  p2 = strchr(p1, ':');
  if(p2) {
    *p2 = 0;
    p2++;
    pElem->prefix = xmlStrdup((xmlChar*)p1);
    if(!pElem->prefix) {
      free(p1);
      SET_LAST_ERROR_RETURN_CODE(ERR_BAD_ALLOC);
    }
    pElem->tag = xmlStrdup((const xmlChar*)p2);
  }
  else
    pElem->tag = xmlStrdup((const xmlChar*)p1);
  free(p1);
  if(!pElem->tag)
    SET_LAST_ERROR_RETURN_CODE(ERR_BAD_ALLOC);
  for (i = 0; (atts != NULL) && (atts[i] != NULL); i += 2) {
    n = (xmlChar*)atts[i];
    v = (xmlChar*)atts[i+1];
    if(n && !strncmp((char*)n, "xmlns", 5) && v)
      pElem->nsUri = xmlStrdup((xmlChar*)v);
    if(n && !strncmp((char*)n, "Id", 2) && v)
      pElem->id = xmlStrdup((xmlChar*)v);
    if(n && !strncmp((char*)n, "URI", 3) && v)
      pElem->uri = xmlStrdup((xmlChar*)v);
    if(n && !strncmp((char*)n, "ContentType", 11) && v)
      pElem->content = xmlStrdup((xmlChar*)v);
  }
  if(pLastElem) // this may be NULL if users doesn't want the last elem.
    *pLastElem = pElem;
  return ERR_OK;
}

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
			ElementEntry** pLastElem)
{
  ElementEntry *pElem, *pOldElem;

  RETURN_IF_NULL_PARAM(pStack);
  do {
    pOldElem = pElem = pStack;
    while(pElem->pNext) {
      pOldElem = pElem;
      pElem = (ElementEntry *)pElem->pNext;
    }
    if(pElem && pElem != pStack) {
      ddocDebug(5, "ddocStackPopElement", "Pop: %s, cleanup: %d", pElem->tag, bCleanup);
      xmlFree(pElem->tag);
      pElem->tag = 0;
      xmlFree(pElem->prefix);
      pElem->prefix = 0;
      xmlFree(pElem->nsUri);
      pElem->nsUri = 0;
      xmlFree(pElem->id);
      pElem->id = 0;
      xmlFree(pElem->uri);
      pElem->uri = 0;
      xmlFree(pElem->content);
      pElem->content = 0;
      free(pElem);
      pOldElem->pNext = 0;
      if(!bCleanup)
	break;  // remove just the last one?
    }
  } while(pElem && pElem != pStack);
  if(bCleanup) {
    // now free up also the final first elements
    // dynamically allocated part
    xmlFree(pStack->tag);
    pStack->tag = 0;
    xmlFree(pStack->prefix);
    pStack->prefix = 0;
    xmlFree(pStack->nsUri);
    pStack->nsUri = 0;
    xmlFree(pStack->id);
    pStack->id = 0;
    xmlFree(pStack->uri);
    pStack->uri = 0;
    xmlFree(pStack->content);
    pStack->content = 0;
    pStack->pNext = 0;
  }
  if(pLastElem) // this may be NULL if users doesn't want the last elem.
    *pLastElem = pElem;
  return ERR_OK;
}


//--------------------------------------------------
// Retrieve the current/last/stack top elements tag (localname)
// pStack - address of stack begin.
//--------------------------------------------------
const xmlChar* ddocStackCurrentTag(ElementEntry* pStack)
{
  ElementEntry* pElem = 0;
  const xmlChar* pTag = 0;

  if(pStack) {
    pElem = ddocStackFindEnd(pStack);
    if(pElem) 
      pTag = (const xmlChar*)pElem->tag;
  }
  return pTag;
}

//--------------------------------------------------
// Retrieve the current/last/stack top elements ns prefix
// pStack - address of stack begin.
//--------------------------------------------------
const xmlChar* ddocStackCurrentNsPrefix(ElementEntry* pStack)
{
  ElementEntry* pElem = 0;
  const xmlChar* pPrefix = 0;

  if(pStack) {
    pElem = ddocStackFindEnd(pStack);
    if(pElem) 
      pPrefix = (const xmlChar*)pElem->prefix;
  }
  return pPrefix;
}


//--------------------------------------------------
// Retrieve the current/last/stack top elements ns prefix
// pStack - address of stack begin.
//--------------------------------------------------
const xmlChar* ddocStackCurrentNsUri(ElementEntry* pStack)
{
  ElementEntry* pElem = 0;
  const xmlChar* pUri = 0;

  if(pStack) {
    pElem = ddocStackFindEnd(pStack);
    if(pElem) 
      pUri = (const xmlChar*)pElem->nsUri;
  }
  return pUri;
}

//--------------------------------------------------
// Checks if there is a parent element with the given
// localname on the stack.
// pStack - address of stack begin.
// return 1 if there is such a parent elem or 0 if not.
//--------------------------------------------------
int ddocStackHasParentWithName(ElementEntry* pStack, 
			       const xmlChar* parentsName, ElementEntry* pCurrElem)
{
  int bFound = 0;
  ElementEntry *pElem = pStack;

  while(pElem && pElem != pCurrElem) {
    if(pElem->tag && !xmlStrcmp(pElem->tag, parentsName)) {
      bFound = 1;
      break;
    } else
      pElem = (ElementEntry *)pElem->pNext;
  }
  return bFound;
}

//--------------------------------------------------
// Checks if there is a parent element with the given
// localname on the stack.
// pStack - address of stack begin.
// return 1 if there is such a parent elem or 0 if not.
//--------------------------------------------------
ElementEntry* ddocStackGetParentWithName(ElementEntry* pStack, 
			       const xmlChar* parentsName, ElementEntry* pCurrElem)
{
  ElementEntry *pParent = 0;
  ElementEntry *pElem = pStack;

  while(pElem && pElem != pCurrElem) {
    if(pElem->tag && !xmlStrcmp(pElem->tag, parentsName)) {
      pParent = pElem;
      break;
    } else
      pElem = (ElementEntry *)pElem->pNext;
  }
  return pParent;
}
