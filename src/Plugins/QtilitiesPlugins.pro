# ***************************************************************************
# Copyright (c) 2009-2012, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
#
# Qtilities Libraries Plugins
#
#****************************************************************************

TARGET      = Qtilities
TEMPLATE    = subdirs
CONFIG     += ordered

# Plugins:
SUBDIRS    += PluginTemplate
SUBDIRS    += SessionLogPlugin
SUBDIRS    += ProjectManagementPlugin
SUBDIRS    += DebugPlugin
SUBDIRS    += HelpPlugin
