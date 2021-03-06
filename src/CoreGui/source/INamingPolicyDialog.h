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

#ifndef INAMINGPOLICYDIALOG_H
#define INAMINGPOLICYDIALOG_H

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCoreGuiConstants.h"
#include "NamingPolicyFilter.h"

#include <IObjectBase>

#include <QDialog>

namespace Qtilities {
    namespace CoreGui {
        namespace Interfaces {
            using namespace Qtilities::Core::Interfaces;

            /*!
            \class INamingPolicyDialog
            \brief NamingPolicyFilter classes uses implementations of this interface in order to prompt users for actions during naming conflicts.

            <i>This class was added in %Qtilities v1.0.</i>
              */
            class QTILITIES_CORE_GUI_SHARED_EXPORT INamingPolicyDialog: public QDialog, virtual public IObjectBase
            {
            public:
                INamingPolicyDialog(QWidget *parent = 0) : QDialog(parent) {}
                virtual ~INamingPolicyDialog() {}

                //! Indicates if the "All in this cycle" check box is selected.
                virtual bool useCycleResolution() const = 0;
                //! Unchecks the "All in this cycle" check box.
                virtual void endValidationCycle() = 0;
                //! Returns the current auto generated name.
                virtual QString autoGeneratedName() const = 0;
                //! Returns the currect selected resolution.

                virtual NamingPolicyFilter::ResolutionPolicy selectedResolution() const = 0;
                //! Sets the naming policy filter from which this dialog was constructed.
                virtual void setNamingPolicyFilter(NamingPolicyFilter* naming_policy_subject_filter)  = 0;
                //! Sets the current object which is causing the conflict.
                virtual void setObject(QObject* obj) = 0;
                //! Sets the context in which the naming policy filter is used.
                virtual void setContext(int context_id, const QString& context_name, const QIcon& window_icon = QIcon()) = 0;
                //! Initializes the dialog with the speficic validity.
                virtual bool initialize(NamingPolicyFilter::NameValidity validity_result) = 0;

            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::CoreGui::Interfaces::INamingPolicyDialog,"com.Qtilities.CoreGui.INamingPolicyDialog/1.0");

#endif // INAMINGPOLICYDIALOG_H
