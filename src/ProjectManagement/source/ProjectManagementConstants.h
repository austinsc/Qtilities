/****************************************************************************
**
** Copyright (c) 2009-2012, Jaco Naude
**
** This file is part of Qtilities which is released under the following
** licensing options.
**
** Option 1: Open Source
** Under this license Qtilities is free software: you can
** redistribute it and/or modify it under the terms of the GNU General
** Public License as published by the Free Software Foundation, either
** version 3 of the License, or (at your option) any later version.
**
** Qtilities is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Qtilities. If not, see http://www.gnu.org/licenses/.
**
** Option 2: Commercial
** Alternatively, this library is also released under a commercial license
** that allows the development of closed source proprietary applications
** without restrictions on licensing. For more information on this option,
** please see the project website's licensing page:
** http://www.qtilities.org/licensing.html
**
** If you are unsure which license is appropriate for your use, please
** contact support@qtilities.org.
**
****************************************************************************/

#ifndef PROJECT_MANAGEMENT_CONSTANTS_H
#define PROJECT_MANAGEMENT_CONSTANTS_H

namespace Qtilities {
    //! Namespace containing all the classes which forms part of the ProjectManagement Module.
    /*!
    To use this module, add the following to your .pro %file:
    \code
    QTILITIES += project_management;
    include(Qtilities_Path/src/Qtilities.pri)
    \endcode

    To include all files in this module:
    \code
    #include <QtilitiesProjectManagement>
    using namespace QtilitiesProjectManagement;
    \endcode

    For more information about the modules in %Qtilities, see \ref page_modules_overview.
    */
    namespace ProjectManagement {
        //! Namespace containing constants used inside the ProjectManagement Module.
        namespace Constants {
            // File Extensions
            //! The file extension used for binary project files.
            const char * const qti_def_SUFFIX_PROJECT_BINARY  = "prj";
            //! The file extension used for xml project files. By default xml and formatted in the %Qtilities Tree Format.
            const char * const qti_def_SUFFIX_PROJECT_XML     = "xml";
        }
    }
}

#endif // PROJECT_MANAGEMENT_CONSTANTS_H
