# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
QTILITIES += extension_system
QTILITIES += project_management
include(../../Qtilities.pri)
include(../../Dependencies.pri)

QT       += core
QT       += gui
CONFIG   += help
QT += network
QT += webkit
TARGET = MainWindowExample
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/MainWindowExample

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/MainWindowExample
MOC_DIR         = $$QTILITIES_TEMP/MainWindowExample
RCC_DIR         = $$QTILITIES_TEMP/MainWindowExample
UI_DIR          = $$QTILITIES_TEMP/MainWindowExample

# --------------------------
# Application Files
# --------------------------
SOURCES += main.cpp \
        ExampleMode.cpp

HEADERS += ExampleMode.h

FORMS   += ExampleMode.ui

RC_FILE = rc_file.rc

