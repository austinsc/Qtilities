# ***************************************************************************
# Copyright (c) 2009-2011, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
# See the Conan section below for exceptions when enabling Conan Integration
# in the Qtilities Debug Plugin.
#
# ***************************************************************************
#
# Qtilities - Qt Utility Libraries
# Dependencies Config PRI File
#
# ***************************************************************************

#****************************************************************************
# Set up dependencies paths:
#
# By default Qtilities assumes that you have a folder where all your
# libraries are placed, including Qtilities. It assumes you have a folder
# for Qtilities with a trunk or specific version which you attempt to build.
# Thus all dependencies are expected two levels up from the Qtilities
# build directory.
#****************************************************************************
QTILITIES_DEPENDENCIES  = $$PWD/../../../

# --------------------------
# Qt Property Browser Solution
# --------------------------
# http://qt.gitorious.org/qt-solutions
# Tested with v2.6.
# --------------------------
# Builds Qtilities with the ObjectPropertyBrowser and
# ObjectDynamicPropertyBrowser classes, thus you
# need the Qt Property Browser Solution.
DEFINES += QTILITIES_PROPERTY_BROWSER
# --------------------------
contains(DEFINES, QTILITIES_PROPERTY_BROWSER) {
    DEFINES += QT_QTPROPERTYBROWSER_IMPORT
    QT_SOLUTIONS_PATH = $$QTILITIES_DEPENDENCIES/qt-solutions
    PROPERTY_EDITOR_BASE = $$QT_SOLUTIONS_PATH/qtpropertybrowser
    DEPENDPATH += $$PROPERTY_EDITOR_BASE/src
    INCLUDEPATH += $$PROPERTY_EDITOR_BASE/src
    CONFIG(debug, debug|release) {
        win32:LIBS += $$PROPERTY_EDITOR_BASE/lib/libQtSolutions_PropertyBrowser-headd.a
        unix::LIBS += -L$$PROPERTY_EDITOR_BASE/lib \
            -lQtSolutions_PropertyBrowser-head
    }
    else {
        win32:LIBS += $$PROPERTY_EDITOR_BASE/lib/libQtSolutions_PropertyBrowser-head.a
        unix::LIBS += -L$$PROPERTY_EDITOR_BASE/lib \
            -lQtSolutions_PropertyBrowser-head
    }
}

# --------------------------
# Conan
# --------------------------
# https://sourceforge.net/projects/conanforqt/
# Requires at least, and tested with Conan v1.0.2.
#
# Integrates the Conan Library into the debug widget:
# --------------------------
# Builds Qtilities with the Conan library integrated into the debug plugin.
# Note that this integration was only tested in Qt 4.7.x on Windows XP 32bit.
# DEFINES += QTILITIES_CONAN
#
# The Conan library is released under GPL v3.0. Therefore, when uncommenting
# the above DEFINE, the Debug Plugin (which links against Conan) is not released
# under the normal Qtilities license found at http://www.qtilities.org/licensing.html.
# It is released under the following license:
#
# Under this license the Debug Plugin is free software: you can
# redistribute it and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation, either
# version 3 of the License, or (at your option) any later version.
#
# The Debug Plugin is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the Debug Plugin. If not, see http://www.gnu.org/licenses/.
# --------------------------

contains(DEFINES, QTILITIES_CONAN) {
    CONAN_BASE = $$QTILITIES_DEPENDENCIES/conanforqt/Conan
    INCLUDEPATH += $$CONAN_BASE/include

    CONFIG(debug, debug|release) {
        win32:LIBS += $$CONAN_BASE/lib/libConand1.a
        unix::LIBS += -L$$CONAN_BASE/lib -lConand
    }
    else {
        win32:LIBS += $$CONAN_BASE/lib/libConan1.a
        unix::LIBS += -L$$CONAN_BASE/lib -lConan
    }
}