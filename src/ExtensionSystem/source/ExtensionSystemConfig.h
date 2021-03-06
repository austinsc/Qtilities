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

#ifndef EXTENSIONSYSTEMCONFIG_H
#define EXTENSIONSYSTEMCONFIG_H

#include <IConfigPage>
#include <ObserverWidget>

#include <QWidget>

namespace Ui {
    class ExtensionSystemConfig;
}

namespace Qtilities {
    namespace ExtensionSystem {
        using namespace Qtilities::CoreGui;
        using namespace Qtilities::CoreGui::Interfaces;

        /*!
          \class ExtensionSystemConfig
          \brief The ExtensionSystemConfig class provides a ready to use configuration widget for the extension system.

        The ExtensionSystemConfig widget exposes settings of the extension system through a configuration page shown below:

        \image html extension_system_configuration_widget.jpg "Extension System Configuration Widget"

        To add this page to your application's ConfigurationWidget, do the following:
\code
EXTENSION_SYSTEM->initialize();

// Register extension system config page.
OBJECT_MANAGER->registerObject(EXTENSION_SYSTEM->configWidget());
\endcode

        For more information see the \ref page_extension_system article.
         */
        class ExtensionSystemConfig : public QWidget, public IConfigPage {
            Q_OBJECT
            Q_INTERFACES(Qtilities::CoreGui::Interfaces::IConfigPage)

            friend class ExtensionSystemCore;

        public:
            ExtensionSystemConfig(QWidget *parent = 0);
            ~ExtensionSystemConfig();

            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------------------
            // IConfigPage Implementation
            // --------------------------------------------
            QIcon configPageIcon() const;
            QWidget* configPageWidget();
            QtilitiesCategory configPageCategory() const;
            QString configPageTitle() const;
            void configPageApply() {}
            bool supportsApply() const { return false; }

        public slots:
            void handleBtnDetailsClicked();
            void handleSelectionChanged(QList<QObject*> selection);
            void handleSelectionDoubleClicked(QObject* selection);
            void setStatusMessage(const QString& status_message);

        protected:
            void changeEvent(QEvent *e);

        private:
            void setPluginListWidget(QWidget* plugin_list_widget);
            ObserverWidget* observer_widget;

            Ui::ExtensionSystemConfig *ui;
        };
    }
}


#endif // EXTENSIONSYSTEMCONFIG_H
