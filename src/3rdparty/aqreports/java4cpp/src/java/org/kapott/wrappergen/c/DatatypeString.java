
/* $Id: DatatypeString.java 18 2005-12-22 09:14:04Z kleiner $

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

package org.kapott.wrappergen.c;

import java.util.ArrayList;
import java.util.List;


public class DatatypeString
    extends Datatype
{
    public String getTypeName()
    {
        return "Object";
    }
    
    public String getJNISignature()
    {
        return "jobject";
    }
    
    public String getJavaSignature()
    {
        return "Ljava/lang/String;";
    }

    public String getNativeSignature()
    {
        return "const char*";
    }

    public String[] native2JavaPre(String var,String jvar)
    {
        return new String[] {
            "jstring "+jvar+";",
            "if ("+var+"!=NULL) {",
            "  "+jvar+" = javaEnv->NewStringUTF("+var+");",
            "} else {",
            "  "+jvar+"=NULL;",
            "}",
            "handleJavaException(wrapperIntern);"
            };
    }

    public String[] native2JavaPost(String var,String jvar)
    {
        return new String[] {
            "if ("+jvar+"!=NULL) {",
            "  javaEnv->DeleteLocalRef("+jvar+");",
            "}"
            };
    }
    
    public String[] java2Native(String jvar,String var)
    {
        List out=new ArrayList();
        
        // TODO: utf-8!
        out.add("char* "+var+";");
        out.add("if ("+jvar+"!=NULL) {");
        out.add("  const char*  "+jvar+"_bytes = javaEnv->GetStringUTFChars((jstring)"+jvar+",NULL);");
        out.add("  handleJavaException(wrapperIntern);");
        out.add("  jsize        "+jvar+"_size = javaEnv->GetStringUTFLength((jstring)"+jvar+");");
        out.add("  handleJavaException(wrapperIntern);");
        out.add("               "+var+" = new char["+jvar+"_size+1];");
        out.add("  for (int i=0;i<"+jvar+"_size;i++) {");
        out.add("    "+var+"[i] = "+jvar+"_bytes[i];");
        out.add("  }");
        out.add("  "+var+"["+jvar+"_size]=0;");
        out.add("  javaEnv->ReleaseStringUTFChars((jstring)"+jvar+", "+jvar+"_bytes);");
        out.add("  handleJavaException(wrapperIntern);");
        out.add("  javaEnv->DeleteLocalRef("+jvar+");");
        out.add("} else {");
        out.add("  "+var+"=NULL;");
        out.add("}");
        
        return (String[])out.toArray(new String[out.size()]);
    }
}
