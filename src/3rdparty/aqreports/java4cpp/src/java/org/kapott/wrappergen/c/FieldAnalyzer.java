
/* $Id: FieldAnalyzer.java 6 2005-12-05 15:43:30Z kleiner $

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

import java.lang.reflect.Field;

/**
 * @author stefan.palme
 */
public class FieldAnalyzer
{
    private Field field;
    
    public FieldAnalyzer(Field field)
    {
        this.field=field;
    }

    public String getNativeType()
    {
        return Datatype.getInstance(field.getType()).getNativeSignature();
    }

    public String getJavaSignature()
    {
        return Datatype.getInstance(field.getType()).getJavaSignature();
    }

    public String getReturnTypeName()
    {
        return Datatype.getInstance(field.getType()).getTypeName();
    }
    
    public String getJNIReturnType()
    {
        return Datatype.getInstance(field.getType()).getJNISignature();
    }
}
