
/* $Id: java_base_class.cpp 6 2005-12-05 15:43:30Z kleiner $

   This file is part of java4cpp
   Copyright (C) 2005  Stefan Palme

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <jni.h>
#include <java_base_class.h>
#include <java_marker.h>

extern JNIEnv  *javaEnv;
extern jobject javaException;

JavaBaseClass::JavaBaseClass(JavaMarker* dummy)
{
  setJavaObject(NULL);
}

JavaBaseClass::JavaBaseClass(jobject obj)
{
  setJavaObject(obj);
}

JavaBaseClass::~JavaBaseClass()
{
  clearJavaObject();
}

void JavaBaseClass::handleJavaException(JavaMarker* dummy)
{
  jthrowable exc = javaEnv->ExceptionOccurred();
  if (exc) {
    javaException = exc;
    javaEnv->ExceptionDescribe();
    javaEnv->ExceptionClear();
    throw exc;
  }
}

void JavaBaseClass::clearJavaObject()
{
  javaEnv->DeleteGlobalRef(obj);
}

void JavaBaseClass::setJavaObject(jobject obj)
{
  if (obj!=NULL) {
    this->obj = javaEnv->NewGlobalRef(obj);
  } else {
    this->obj = obj;
  }
}

jobject JavaBaseClass::getJavaObject()
{
  return this->obj;
}
