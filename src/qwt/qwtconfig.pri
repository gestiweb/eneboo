exists(../../settings.pro) {
        include(../../settings.pro)
}
else {
        error("settings.pro missing, unable to build")
}

######################################################################
# Install paths
######################################################################

VER_MAJ      = 5
VER_MIN      = 1
VER_PAT      = 2
VERSION      = $$VER_MAJ.$$VER_MIN.$$VER_PAT

unix {
    INSTALLBASE    = $$PREFIX
}

win32 {
    INSTALLBASE    = $$PREFIX
}

######################################################################
# Build the static/shared libraries.
# If QwtDll is enabled, a shared library is built, otherwise
# it will be a static library.
######################################################################

CONFIG           += QwtDll

######################################################################
# QwtPlot enables all classes, that are needed to use the QwtPlot 
# widget. 
######################################################################

CONFIG       += QwtPlot

######################################################################
# QwtWidgets enables all classes, that are needed to use the all other
# widgets (sliders, dials, ...), beside QwtPlot. 
######################################################################

CONFIG     += QwtWidgets

######################################################################
# If you want to display svg imageson the plot canvas, enable the 
# line below. Note that Qwt needs the svg+xml, when enabling 
# QwtSVGItem.
######################################################################

CONFIG     += QwtSVGItem

######################################################################
# If you have a commercial license you can use the MathML renderer
# of the Qt solutions package to enable MathML support in Qwt.
# So if you want this, copy qtmmlwidget.h + qtmmlwidget.cpp to
# textengines/mathml and enable the line below.
######################################################################

#CONFIG     += QwtMathML

######################################################################
# If you want to build the Qwt designer plugin, 
# enable the line below.
# Otherwise you have to build it from the designer directory.
######################################################################

!macx-g++:CONFIG     += QwtDesigner

