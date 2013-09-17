
/* $Id: DatatypeInt.java 6 2005-12-05 15:43:30Z kleiner $

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


public class DatatypeInt
    extends Datatype
{
    public String getTypeName()
    {
        return "Int";
    }
    
    public String getJNISignature()
    {
        return "jint";
    }
    
    public String getJavaSignature()
    {
        return "I";
    }

    public String getNativeSignature()
    {
        return "int";
    }

    public String[] native2JavaPre(String var,String jvar)
    {
        return new String[] {"jint "+jvar+" = (jint)"+var+";"};
    }

    public String[] native2JavaPost(String var,String jvar)
    {
        return new String[0];
    }
    
    public String[] java2Native(String jvar,String var)
    {
        return new String[] {"int "+var+" = (int)"+jvar+";"};
    }
}
