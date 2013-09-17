
/* $Id: Datatype.java 6 2005-12-05 15:43:30Z kleiner $

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


public abstract class Datatype
{
    private Class cl;
    
    public abstract String getTypeName();
    public abstract String getJNISignature();
    public abstract String getJavaSignature();
    public abstract String getNativeSignature();
    
    public abstract String[] native2JavaPre(String var, String jvar);
    public abstract String[] native2JavaPost(String var, String jvar);
    public abstract String[] java2Native(String jvar, String var);
    
    public static Datatype getInstance(Class cl)
    {
        Datatype ret=null;
        
        if (cl.isArray()) {
            ret=new DatatypeArray();
        } else if (cl.isPrimitive()) {
            String name=cl.getName();
            
            if (name.equals("boolean")) {
                ret=new DatatypeBoolean();
            } else if (name.equals("byte")) {
                ret=new DatatypeByte();
            } else if (name.equals("char")) {
                ret=new DatatypeChar();
            } else if (name.equals("double")) {
                ret=new DatatypeDouble();
            } else if (name.equals("float")) {
                ret=new DatatypeFloat();
            } else if (name.equals("int")) {
                ret=new DatatypeInt();
            } else if (name.equals("long")) {
                ret=new DatatypeLong();
            } else if (name.equals("short")) {
                ret=new DatatypeShort();
            } else if (name.equals("void")) {
                ret=new DatatypeVoid();
            } else {
                throw new RuntimeException("unknown parameter type: "+cl);
            }
        } else {
            if (cl.equals(String.class)) {
                ret=new DatatypeString();
            } else {
                ret=new DatatypeObject();
            }
        }
        
        ret.setClassObject(cl);
        return ret;
    }
    
    public void setClassObject(Class cl)
    {
        this.cl=cl;
    }
    
    public Class getClassObject()
    {
        return this.cl;
    }
}
