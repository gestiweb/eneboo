
/* $Id: ConstructorAnalyzer.java 6 2005-12-05 15:43:30Z kleiner $

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

public class ConstructorAnalyzer
{
    private Constructor constructor;
    
    public ConstructorAnalyzer(Constructor constructor)
    {
        this.constructor=constructor;
    }

    protected ConstructorAnalyzer()
    {
    }
    
    public String getNativeSignature()
    {
        return generateNativeSignature(constructor.getParameterTypes());
    }

    protected String generateNativeSignature(Class[] params)
    {
        StringBuffer ret=new StringBuffer();

        for (int i=0;i<params.length;i++) {
            Class    param=params[i];
            Datatype datatype=Datatype.getInstance(param);
            
            if (i!=0) {
                ret.append(", ");
            }
            ret.append(datatype.getNativeSignature()+" arg"+(i+1));
        }
        
        return ret.toString();
    }

    public String getJavaSignature()
    {
        return generateJavaSignature(constructor.getParameterTypes());
    }

    protected String generateJavaSignature(Class[] params)
    {
        StringBuffer ret=new StringBuffer();

        for (int i=0;i<params.length;i++) {
            Class    param=params[i];
            Datatype datatype=Datatype.getInstance(param);
            ret.append(datatype.getJavaSignature());
        }
        
        return ret.toString();
    }

}
