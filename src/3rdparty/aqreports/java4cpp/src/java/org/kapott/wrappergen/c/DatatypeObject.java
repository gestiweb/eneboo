
/* $Id: DatatypeObject.java 18 2005-12-22 09:14:04Z kleiner $

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


public class DatatypeObject
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
        return "L"+new ClassAnalyzer(getClassObject()).getSlashedName()+";";
    }

    public String getNativeSignature()
    {
        return new ClassAnalyzer(getClassObject()).getNamespacedName()+"*";
    }

    public String[] native2JavaPre(String var,String jvar)
    {
        return new String[] {
            "jobject "+jvar+";",
            "if ("+var+"!=NULL) {",
            "  "+jvar+"="+var+"->getJavaObject();",
            "} else {",
            "  "+jvar+"=NULL;",
            "}",
            Main.DEBUG?"printf(\"native2JavaPre("+var+"): "+jvar+"=%p\\n\", "+jvar+");":null
            };
    }

    public String[] native2JavaPost(String var,String jvar)
    {
        return new String[0];
    }
    
    public String[] java2Native(String jvar,String var)
    {
        String name=new ClassAnalyzer(getClassObject()).getNamespacedName();
        return new String[] {
            Main.DEBUG?"printf(\"java2Native(): jresult=%p\\n\", "+jvar+");":null,
            getNativeSignature()+" "+var+";",
            "if ("+jvar+"!=NULL) {",
            "  "+var+"=new "+name+"("+jvar+");",
            "  javaEnv->DeleteLocalRef("+jvar+");",
            "} else {",
            "  "+var+"=NULL;",
            "}",
            Main.DEBUG?"printf(\"java2Native(): created new native object: %p\\n\", "+var+");":null
            };
    }
}
