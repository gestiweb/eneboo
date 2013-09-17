
/* $Id: java_array.h 6 2005-12-05 15:43:30Z kleiner $

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

#ifndef JAVA_ARRAY_H
#define JAVA_ARRAY_H

#include <jni.h>
#include <java_marker.h>
#include <java_lang_Object.h>

class JavaArray : public java::lang::Object {
  public:
    JavaArray(JavaMarker* dummy);
    JavaArray(jarray obj);
    
    virtual void* getArrayData();
    virtual int   getArrayLength();
};

class JavaBooleanArray : public JavaArray {
  public:
    JavaBooleanArray(JavaMarker* dummy);
    JavaBooleanArray(jarray obj);
    JavaBooleanArray(const bool* data,int len);
    
    virtual void* getArrayData();
};

class JavaByteArray : public JavaArray {
  public:
    JavaByteArray(JavaMarker* dummy);
    JavaByteArray(jarray obj);
    JavaByteArray(const char* data,int len);
    
    virtual void* getArrayData();
};

class JavaCharArray : public JavaArray {
  public:
    JavaCharArray(JavaMarker* dummy);
    JavaCharArray(jarray obj);
    JavaCharArray(const char* data,int len);
    
    virtual void* getArrayData();
};

class JavaShortArray : public JavaArray {
  public:
    JavaShortArray(JavaMarker* dummy);
    JavaShortArray(jarray obj);
    JavaShortArray(const short* data,int len);
    
    virtual void* getArrayData();
};

class JavaIntArray : public JavaArray {
  public:
    JavaIntArray(JavaMarker* dummy);
    JavaIntArray(jarray obj);
    JavaIntArray(const int* data,int len);
    
    virtual void* getArrayData();
};

class JavaLongArray : public JavaArray {
  public:
    JavaLongArray(JavaMarker* dummy);
    JavaLongArray(jarray obj);
    JavaLongArray(const long* data,int len);
    
    virtual void* getArrayData();
};

class JavaFloatArray : public JavaArray {
  public:
    JavaFloatArray(JavaMarker* dummy);
    JavaFloatArray(jarray obj);
    JavaFloatArray(const float* data,int len);
    
    virtual void* getArrayData();
};

class JavaDoubleArray : public JavaArray {
  public:
    JavaDoubleArray(JavaMarker* dummy);
    JavaDoubleArray(jarray obj);
    JavaDoubleArray(const double* data,int len);
    
    virtual void* getArrayData();
};

class JavaObjectArray : public JavaArray {
  public:
    JavaObjectArray(JavaMarker* dummy);
    JavaObjectArray(jarray obj);
    JavaObjectArray(java::lang::Object** data,int len);
    
    virtual void* getArrayData();
};

#endif
