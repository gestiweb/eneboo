
/* $Id: java_array.cpp 6 2005-12-05 15:43:30Z kleiner $

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
#include <java_marker.h>
#include <java_array.h>
#include <java_lang_Object.h>

extern JNIEnv     *javaEnv;
extern JavaMarker *wrapperIntern;

JavaArray::JavaArray(JavaMarker* dummy)
  : java::lang::Object(dummy)
{
}

JavaArray::JavaArray(jarray obj)
  : java::lang::Object(obj)
{
}

void* JavaArray::getArrayData()
{
  return NULL;
}

int JavaArray::getArrayLength()
{
  int len = (int)(javaEnv->GetArrayLength((jarray)getJavaObject()));
  handleJavaException(wrapperIntern);
  return len;
}

// --------------------------------------------------------------------------

JavaByteArray::JavaByteArray(JavaMarker* dummy)
  : JavaArray(dummy)
{
}

JavaByteArray::JavaByteArray(jarray obj)
  : JavaArray(obj)
{
}

JavaByteArray::JavaByteArray(const char* data,int len)
  : JavaArray(wrapperIntern)
{
  jbyteArray obj = javaEnv->NewByteArray((jsize)len);
  handleJavaException(wrapperIntern);
  
  jbyte* jdata = new jbyte[len];
  for (int i=0;i<len;i++) {
    jdata[i] = data[i];
  }
  javaEnv->SetByteArrayRegion(obj, (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  
  delete[] jdata;
  
  setJavaObject(obj);
  javaEnv->DeleteLocalRef(obj);
}

void* JavaByteArray::getArrayData()
{
  int    len = getArrayLength();
  jbyte* jdata = new jbyte[len];
  javaEnv->GetByteArrayRegion((jbyteArray)getJavaObject(), (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  
  char* data = new char[len];
  for (int i=0;i<len;i++) {
    data[i] = jdata[i];
  }
  
  delete[] jdata;
  return data;
}

// --------------------------------------------------------------------------

JavaBooleanArray::JavaBooleanArray(JavaMarker* dummy)
  : JavaArray(dummy)
{
}

JavaBooleanArray::JavaBooleanArray(jarray obj)
  : JavaArray(obj)
{
}

JavaBooleanArray::JavaBooleanArray(const bool* data,int len)
  : JavaArray(wrapperIntern)
{
  jbooleanArray obj = javaEnv->NewBooleanArray((jsize)len);
  handleJavaException(wrapperIntern);
  
  jboolean* jdata = new jboolean[len];
  for (int i=0;i<len;i++) {
    jdata[i] = data[i];
  }
  javaEnv->SetBooleanArrayRegion(obj, (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  
  delete[] jdata;
  
  setJavaObject(obj);
  javaEnv->DeleteLocalRef(obj);
}

void* JavaBooleanArray::getArrayData()
{
  int       len = getArrayLength();
  jboolean* jdata = new jboolean[len];
  javaEnv->GetBooleanArrayRegion((jbooleanArray)getJavaObject(), (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  
  bool* data = new bool[len];
  for (int i=0;i<len;i++) {
    data[i] = jdata[i];
  }
  
  delete[] jdata;
  return data;
}

// --------------------------------------------------------------------------

JavaCharArray::JavaCharArray(JavaMarker* dummy)
  : JavaArray(dummy)
{
}

JavaCharArray::JavaCharArray(jarray obj)
  : JavaArray(obj)
{
}

JavaCharArray::JavaCharArray(const char* data,int len)
  : JavaArray(wrapperIntern)
{
  jcharArray obj = javaEnv->NewCharArray((jsize)len);
  handleJavaException(wrapperIntern);
  
  jchar* jdata = new jchar[len];
  for (int i=0;i<len;i++) {
    jdata[i] = data[i];
  }
  javaEnv->SetCharArrayRegion(obj, (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  
  delete[] jdata;
  
  setJavaObject(obj);
  javaEnv->DeleteLocalRef(obj);
}

void* JavaCharArray::getArrayData()
{
  int       len = getArrayLength();
  jchar* jdata = new jchar[len];
  javaEnv->GetCharArrayRegion((jcharArray)getJavaObject(), (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  
  char* data = new char[len];
  for (int i=0;i<len;i++) {
    data[i] = jdata[i];
  }
  
  delete[] jdata;
  return data;
}

// --------------------------------------------------------------------------

JavaShortArray::JavaShortArray(JavaMarker* dummy)
  : JavaArray(dummy)
{
}

JavaShortArray::JavaShortArray(jarray obj)
  : JavaArray(obj)
{
}

JavaShortArray::JavaShortArray(const short* data,int len)
  : JavaArray(wrapperIntern)
{
  jshortArray obj = javaEnv->NewShortArray((jsize)len);
  handleJavaException(wrapperIntern);
  
  jshort* jdata = new jshort[len];
  for (int i=0;i<len;i++) {
    jdata[i] = data[i];
  }
  javaEnv->SetShortArrayRegion(obj, (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  
  delete[] jdata;
  
  setJavaObject(obj);
  javaEnv->DeleteLocalRef(obj);
}

void* JavaShortArray::getArrayData()
{
  int       len = getArrayLength();
  jshort* jdata = new jshort[len];
  javaEnv->GetShortArrayRegion((jshortArray)getJavaObject(), (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  
  short* data = new short[len];
  for (int i=0;i<len;i++) {
    data[i] = jdata[i];
  }
  
  delete[] jdata;
  return data;
}

// --------------------------------------------------------------------------

JavaIntArray::JavaIntArray(JavaMarker* dummy)
  : JavaArray(dummy)
{
}

JavaIntArray::JavaIntArray(jarray obj)
  : JavaArray(obj)
{
}

JavaIntArray::JavaIntArray(const int* data,int len)
  : JavaArray(wrapperIntern)
{
  jintArray obj = javaEnv->NewIntArray((jsize)len);
  handleJavaException(wrapperIntern);
  
  jint* jdata = new jint[len];
  for (int i=0;i<len;i++) {
    jdata[i] = data[i];
  }
  javaEnv->SetIntArrayRegion(obj, (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  
  delete[] jdata;
  
  setJavaObject(obj);
  javaEnv->DeleteLocalRef(obj);
}

void* JavaIntArray::getArrayData()
{
  int       len = getArrayLength();
  jint* jdata = new jint[len];
  javaEnv->GetIntArrayRegion((jintArray)getJavaObject(), (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  
  int* data = new int[len];
  for (int i=0;i<len;i++) {
    data[i] = jdata[i];
  }
  
  delete[] jdata;
  return data;
}

// --------------------------------------------------------------------------

JavaLongArray::JavaLongArray(JavaMarker* dummy)
  : JavaArray(dummy)
{
}

JavaLongArray::JavaLongArray(jarray obj)
  : JavaArray(obj)
{
}

JavaLongArray::JavaLongArray(const long* data,int len)
  : JavaArray(wrapperIntern)
{
  jlongArray obj = javaEnv->NewLongArray((jsize)len);
  handleJavaException(wrapperIntern);
  
  jlong* jdata = new jlong[len];
  for (int i=0;i<len;i++) {
    jdata[i] = data[i];
  }
  javaEnv->SetLongArrayRegion(obj, (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  
  delete[] jdata;
  
  setJavaObject(obj);
  javaEnv->DeleteLocalRef(obj);
}

void* JavaLongArray::getArrayData()
{
  int       len = getArrayLength();
  jlong* jdata = new jlong[len];
  javaEnv->GetLongArrayRegion((jlongArray)getJavaObject(), (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  
  long* data = new long[len];
  for (int i=0;i<len;i++) {
    data[i] = jdata[i];
  }
  
  delete[] jdata;
  return data;
}

// --------------------------------------------------------------------------

JavaFloatArray::JavaFloatArray(JavaMarker* dummy)
  : JavaArray(dummy)
{
}

JavaFloatArray::JavaFloatArray(jarray obj)
  : JavaArray(obj)
{
}

JavaFloatArray::JavaFloatArray(const float* data,int len)
  : JavaArray(wrapperIntern)
{
  jfloatArray obj = javaEnv->NewFloatArray((jsize)len);
  handleJavaException(wrapperIntern);
  
  jfloat* jdata = new jfloat[len];
  for (int i=0;i<len;i++) {
    jdata[i] = data[i];
  }
  javaEnv->SetFloatArrayRegion(obj, (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  delete[] jdata;
  
  setJavaObject(obj);
  javaEnv->DeleteLocalRef(obj);
}

void* JavaFloatArray::getArrayData()
{
  int       len = getArrayLength();
  jfloat* jdata = new jfloat[len];
  javaEnv->GetFloatArrayRegion((jfloatArray)getJavaObject(), (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  
  float* data = new float[len];
  for (int i=0;i<len;i++) {
    data[i] = jdata[i];
  }
  
  delete[] jdata;
  return data;
}

// --------------------------------------------------------------------------

JavaDoubleArray::JavaDoubleArray(JavaMarker* dummy)
  : JavaArray(dummy)
{
}

JavaDoubleArray::JavaDoubleArray(jarray obj)
  : JavaArray(obj)
{
}

JavaDoubleArray::JavaDoubleArray(const double* data,int len)
  : JavaArray(wrapperIntern)
{
  jdoubleArray obj = javaEnv->NewDoubleArray((jsize)len);
  handleJavaException(wrapperIntern);
  
  jdouble* jdata = new jdouble[len];
  for (int i=0;i<len;i++) {
    jdata[i] = data[i];
  }
  javaEnv->SetDoubleArrayRegion(obj, (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  
  delete[] jdata;
  
  setJavaObject(obj);
  javaEnv->DeleteLocalRef(obj);
}

void* JavaDoubleArray::getArrayData()
{
  int       len = getArrayLength();
  jdouble* jdata = new jdouble[len];
  javaEnv->GetDoubleArrayRegion((jdoubleArray)getJavaObject(), (jsize)0, (jsize)len, jdata);
  handleJavaException(wrapperIntern);
  
  double* data = new double[len];
  for (int i=0;i<len;i++) {
    data[i] = jdata[i];
  }
  
  delete[] jdata;
  return data;
}

// --------------------------------------------------------------------------

JavaObjectArray::JavaObjectArray(JavaMarker* dummy)
  : JavaArray(dummy)
{
}

JavaObjectArray::JavaObjectArray(jarray obj)
  : JavaArray(obj)
{
}

JavaObjectArray::JavaObjectArray(java::lang::Object** data,int len)
  : JavaArray(wrapperIntern)
{
  jobjectArray obj = javaEnv->NewObjectArray((jsize)len, 
                                         javaEnv->GetObjectClass(data[0]->getJavaObject()),
                                         (jobject)NULL);
  handleJavaException(wrapperIntern);
  
  for (int i=0;i<len;i++) {
      javaEnv->SetObjectArrayElement(obj, (jsize)i, data[i]->getJavaObject());
      handleJavaException(wrapperIntern);
  }
  
  setJavaObject(obj);
  javaEnv->DeleteLocalRef(obj);
}

void* JavaObjectArray::getArrayData()
{
  int                  len = getArrayLength();
  java::lang::Object** data = new java::lang::Object*[len];
  jobjectArray         obj = (jobjectArray)getJavaObject();
  
  for (int i=0;i<len;i++) {
      jobject elem = javaEnv->GetObjectArrayElement(obj, (jsize)i);
      handleJavaException(wrapperIntern);
      
      data[i] = new java::lang::Object(elem);
      javaEnv->DeleteLocalRef(elem);
  }
  
  return data;
}
