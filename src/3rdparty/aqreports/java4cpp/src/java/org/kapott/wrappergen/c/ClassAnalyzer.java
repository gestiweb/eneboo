
/* $Id: ClassAnalyzer.java 6 2005-12-05 15:43:30Z kleiner $

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

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.List;


public class ClassAnalyzer
{
    private Class cl;
    
    public ClassAnalyzer(Class cl)
    {
        this.cl=cl;
    }
    
    public Class getClassObject()
    {
        return cl;
    }
    
    public String getMangledName()
    {
        return cl.getName().replace('.', '_').replaceAll("\\$", "__");
    }
    
    public String getNamespacedName()
    {
        return cl.getName().replaceAll("\\.", "::").replaceAll("\\$", "__");
    }
    
    public String[] getNamespaces()
    {
        return cl.getName().replaceAll("\\$", "__").split("\\.");
    }
    
    public String getSlashedName()
    {
        return cl.getName().replace('.', '/');
    }
    
    public Class getSuperClass()
    {
        Class parent=cl.getSuperclass();
        if (parent==null && cl.isInterface()) {
            parent=Object.class;
        }
        return parent;
    }
    
    private Class findArrayType(Class clazz)
    {
        while (clazz.isArray()) {
            clazz=clazz.getComponentType();
        }
        return clazz;
    }
    
    public Class[] getAllRequiredClasses()
    {
        // System.out.println("searching all classes required by "+cl.getName());
        
        List  ret=new ArrayList();
        
        // cl.getSuperclass()
        Class temp=getSuperClass();
        if (temp!=null && !ret.contains(temp)) {
            // System.out.println("  found superclass "+temp.getName());
            ret.add(temp);
        }
        
        // cl.getInterfaces()
        Class[] interfaces=cl.getInterfaces();
        for (int i=0;i<interfaces.length;i++) {
            temp=interfaces[i];
            if (temp!=null && !ret.contains(temp)) {
                // System.out.println("  found interface "+temp.getName());
                ret.add(temp);
            }
        }
        
        // cl.getConstructors()
        Constructor[] constructors=cl.getDeclaredConstructors();
        for (int i=0;i<constructors.length;i++) {
            Constructor constructor=constructors[i];
            if (!Modifier.isPublic(constructor.getModifiers())) {
                continue;
            }
            
            Class[] params=constructor.getParameterTypes();
            for (int j=0;j<params.length;j++) {
                temp=params[j];
                if (temp.isArray()) {
                    temp=findArrayType(temp);
                }
                if (!temp.isPrimitive() && !ret.contains(temp)) {
                    // System.out.println("  found constructor argument "+temp.getName());
                    ret.add(temp);
                }
            }
        }

        // cl.getDelaredMethods()
        Method[] methods=cl.getDeclaredMethods();
        for (int i=0;i<methods.length;i++) {
            Method method=methods[i];
            if (!Modifier.isPublic(method.getModifiers())) {
                continue;
            }
            
            temp=method.getReturnType();
            if (temp.isArray()) {
                temp=findArrayType(temp);
            }
            if (!temp.isPrimitive() && !ret.contains(temp)) {
                // System.out.println("  found method ("+method.getName()+") return type "+temp.getName());
                ret.add(temp);
            }
            
            Class[] params=method.getParameterTypes();
            for (int j=0;j<params.length;j++) {
                temp=params[j];
                if (temp.isArray()) {
                    temp=findArrayType(temp);
                }
                if (!temp.isPrimitive() && !ret.contains(temp)) {
                    // System.out.println("  found method ("+method.getName()+") argument "+temp.getName());
                    ret.add(temp);
                }
            }
        }


        // cl.getDeclaredFields()
        Field[] fields=cl.getDeclaredFields();
        for (int i=0;i<fields.length;i++) {
            Field field=fields[i];
            if (!Modifier.isPublic(field.getModifiers())) {
                continue;
            }
            
            temp=field.getType();
            if (temp.isArray()) {
                temp=findArrayType(temp);
            }
            if (!temp.isPrimitive() && !ret.contains(temp)) {
                // System.out.println("  found field ("+field.getName()+") type "+temp.getName());
                ret.add(temp);
            }
        }
        
        // TODO: cl.getDeclaredClasses()
        
        return (Class[])ret.toArray(new Class[ret.size()]);
    }
}
