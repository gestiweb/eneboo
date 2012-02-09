/*
odf-gen: Simple API to generate OpenDocument documents.
    Copyright (C) 2009  Pablo Jorge, FuDePAN

    This file is part of the odf-gen project.

    odf-gen is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    odf-gen is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with odf-gen.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ODFGEN_H
#define ODFGEN_H

//#define ODFGEN_NAMESPACE
 
#ifdef ODFGEN_NAMESPACE
namespace odfgen
{
#endif

#ifndef NO_ODSGENERATOR
#   include "odsgenerator.h"
#endif

#ifdef ODFGEN_NAMESPACE
}
#endif

#endif // ODFGEN_H
