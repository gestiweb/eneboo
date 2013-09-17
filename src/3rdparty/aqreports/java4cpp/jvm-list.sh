# jvm-list.sh: a list of available JVM and some shortcuts
# Copyright 2008, 2009 by Vincent Fourmond <fourmond@debian.org>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

# All variables defined here can be used as arguments for the
# find_java_runtime function if you strip the __jvm_ at the beginning
# of the variable...
#
# They can also be used for the JAVA_FLAVOR environment variable, see
# java-wrappers(7).

# Sun runtimes
__jvm_sun4="/usr/lib/j2*1.4-sun"
__jvm_sun5="/usr/lib/jvm/java-1.5.0-sun /usr/lib/j2*1.5-sun"
__jvm_sun6="/usr/lib/jvm/java-6-sun /usr/lib/j2*1.6-sun"

# all sun runtimes
__jvm_sun="$__jvm_sun4 $__jvm_sun5 $__jvm_sun6"
# Sun version at least 5:
__jvm_sunmin5="$__jvm_sun5 $__jvm_sun6"

# IBM runtimes. This comes from argouml.sh
# I'm not quite sure it is still useful, but, we never know !
__jvm_ibm="/usr/lib/j2*1.[456]-ibm"


# Now, free runtimes:
__jvm_gcj2="/usr/lib/jvm/java-*-gcj-4.* "
__jvm_gcj5="/usr/lib/jvm/java-1.5*-gcj-4.* "
__jvm_sablevm="/usr/lib/sablevm" # Probably should be dropped, only in etch
__jvm_kaffe="/usr/lib/kaffe /usr/lib/kaffe/pthreads /usr/lib/kaffe/jthreads"
__jvm_icedtea="/usr/lib/jvm/java-7-icedtea" # Probably should be dropped... 
__jvm_cacao="/usr/lib/jvm/cacao"
__jvm_openjdk6="/usr/lib/jvm/java-6-openjdk"
__jvm_cacaooj6="/usr/lib/jvm/java-6-cacao/jre"

# And a few aliases
__jvm_gcj="$__jvm_gcj2"
__jvm_openjdk="$__jvm_openjdk6 $__jvm_cacaooj6"

# The java* runtimes:
# -> corresponds to Provides: java6-runtime 
__jvm_java6="$__jvm_openjdk6 $__jvm_cacaooj6 $__jvm_sun6"
# -> corresponds to Provides: java5-runtime 
__jvm_java5="$__jvm_gcj5 $__jvm_java6 $__jvm_sun5"
# -> corresponds to Provides: java2-runtime 
__jvm_java2="$__jvm_java5 $__jvm_gcj2"


# All JVMs
__jvm_all="/usr/lib/jvm/* $__jvm_ibm $__jvm_sun4 $__jvm_sablevm $__jvm_kaffe"

# Probably here should come a few meaningful global aliases.