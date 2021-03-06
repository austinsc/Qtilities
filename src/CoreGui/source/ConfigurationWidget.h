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

#ifndef CONFIGURATION_WIDGET_H
#define CONFIGURATION_WIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCoreGuiConstants.h"
#include "IConfigPage.h"
#include "IGroupedConfigPageInfoProvider.h"

#include <QWidget>
#include <QAbstractButton>

namespace Ui {
    class ConfigurationWidget;
}
using namespace Qtilities::CoreGui::Interfaces;

namespace Qtilities {
    namespace CoreGui {
        /*!
          \struct ConfigurationWidgetPrivateData
          \brief The ConfigurationWidgetPrivateData struct stores private data used by the ConfigurationWidget class.
         */
        struct ConfigurationWidgetPrivateData;

        /*!
        \class ConfigurationWidget
        \brief Configuration widget which displays config pages from widgets implementing the Qtilities::CoreGui::Interfaces::IConfigPage interface.

        The configuration widget is a simple widget which can be used to display user settings in your applications. There are many such widgets available to Qt developers, and it is not very difficult to create a simple widget yourself. However, to do it properly takes time and doing it properly normally introduces bugs. Thus, the %Qtilities configuration widget provides a ready to use, tested, extendable and configurable configuration widget solution.

        ConfigurationWidget shows configuration pages that implements Qtilities::CoreGui::Interfaces::IConfigPage. To show pages in the configuration widget, register them in the global object pool and call initialize() on the configuration widget. The widget will find all configuration pages in the object pool. Parts of %Qtilities provides ready to use configuration pages to use in your application. For example:

\code
// The shortcuts editor for commands in your application:
OBJECT_MANAGER->registerObject(ACTION_MANAGER->commandEditor());

// Logging configuration page:
OBJECT_MANAGER->registerObject(LoggerGui::createLoggerConfigWidget());

// Extension system configuration page:
OBJECT_MANAGER->registerObject(EXTENSION_SYSTEM->configWidget());

// The project manager configuration page:
OBJECT_MANAGER->registerObject(PROJECT_MANAGER->configWidget());
\endcode

        To create your widget, add something like the following to your \p main.cpp code:

\code
int main(int argc, char *argv[])
{
    QtilitiesApplication a(argc, argv);

    // Create a settings window for our application:
    ConfigurationWidget* config_widget = new ConfigurationWidget;
    QtilitiesApplication::setConfigWidget(config_widget);

    // ... Lots of application code ...
    // Initialize the widget as soon as all your pages are registered in the global object pool:
    config_widget->initialize();

\endcode

        It is now possible to access the configuration widget from anywhere in your application and perform actions on it:
\code
if (QtilitiesApplication::configWidget()) {
    ConfigurationWidget* config_widget = qobject_cast<ConfigurationWidget*> (QtilitiesApplication::configWidget());
    if (config_widget) {
        // First call initialize on the config_widget to make sure it has all the pages available in the global object pool:
        config_widget->initialize();
        if (config_widget->hasPage(tr("My Page"))) {
            // We can get the interface to this page like this:
            IConfigWidget* config_widget_iface = config_widget->getPage("My Page");

            // Or we can show this page:
            config_widget->setActivePage(tr("Code Editors"));
            config_widget->show();
        }
    }
}
\endcode

        Below is an example of the configuartion widget taken from the Object Management Example in the QtilitiesExamples project:

        \image html project_configuration_widget.jpg "Project Configuration Widget"

        It is possible to either show the configuration pages as a list as shown above, or it can be shown as a categorized tree by using the correct Qtilities::DisplayMode in the constructor. This allows your pages to provide a Qtilities::Core::QtilitiesCategory to use in the categorized tree through the \p IConfigPage interface.

        \sa Qtilities::CoreGui::Interfaces::IConfigPage

        \section configuration_widget_storage_layout Configuration settings storage in Qtilities

        Throughout %Qtilities classes store settings using \p QSettings using the QSettings::IniFormat.

        The construction of the QSettings object is done as follows everywhere that settings are saved. This example saves settings of a specific ObserverWidget:
\code
QSettings settings(QtilitiesCoreApplication::qtilitiesSettingsPath(),QSettings::IniFormat);
settings.beginGroup("Qtilities");
settings.beginGroup("GUI");
settings.beginGroup(d->global_meta_type);
// .... Stores some ObserverWidget stuff ...
settings.endGroup();
settings.endGroup();
settings.endGroup();
\endcode

        An important difference is that the Logger modulde does not depend on the Core module, thus it does not have access to QtilitiesCoreApplication. However the logger stores the session path
        it uses as well (see Qtilities::Logging::Logger::setLoggerSessionConfigPath()) and it will save its settings under that path in a file called \p qtilities.ini which by default points to the
        same files as the rest of Qtilities. If you however changes the session path used by your application through QtilitiesCoreApplication::setApplicationSessionPath(), the Logger will automatically
        be updated to use the same settings path, thus your settings will all be saved in the same ini file accross all %Qtilities modules. The same counts for disabling settings using
        QtilitiesCoreApplication::setQtilitiesSettingsEnabled().

        For more information see the Qtilities::Core::QtilitiesCoreApplication::qtilitiesSettingsPath() documentation.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ConfigurationWidget : public QWidget {
            Q_OBJECT

        public:
            ConfigurationWidget(DisplayMode display_mode = TableView, QWidget *parent = 0);
            ~ConfigurationWidget();

            //! Initializes the config widget with the given set of config pages.
            void initialize(QList<IConfigPage*> config_pages, QList<IGroupedConfigPageInfoProvider*> grouped_page_info_providers);
            //! Initializes the widget with a list of QObjects. All objects in the list which implements the IConfigPage interface will be added.
            /*!
              You can initialize the ConfigurationWidget multiple times and when called more than once it will just rescan the global object pool.

              \param object_list Explicitly specified the list of config pages and grouped config page info providers that must be shown in this widget.

              \note If the list is empty, the function will search the global object pool and automatically add all found config pages and grouped config page info providers.
                    Debug messages with information about the found items will be logged.
              */
            void initialize(QList<QObject*> object_list = QList<QObject*>());

            //! Sets the way the configuration widget handles the \p Apply button.
            /*!
              \param apply_all_pages When true, the apply button will call apply on all the configuration pages. When false, only the active page will be applied.

              Default is true;
              */
            void setApplyAllPages(bool apply_all_pages);
            //! Gets the way the configuration widget handles the \p Apply button.
            bool applyAllPages() const;
            //! Checks if a configuration page with the given name exists.
            bool hasPage(const QString& page_name) const;
            //! Checks if a configuration page with the given name exists and if so returns a pointer to the interface.
            IConfigPage* getPage(const QString& page_name) const;

            //! Sets if the configuration widget groups pages with the same categories under tabs in pages named using the name of the category.
            /*!
              False by default.

              \note You must call this function before initialize().

              \sa categorizedTabDisplay();

              <li>This function was introduced in %Qtilities v1.1.<li>
              */
            void setCategorizedTabDisplay(bool enabled = true);
            //! Gets if the configuration widget groups pages with the same categories under tabs in pages named using the name of the category.
            /*!
              \sa setCategorizedTabDisplay()

              <li>This function was introduced in %Qtilities v1.1.</li>
              */
            bool categorizedTabDisplay() const;

       signals:
            //! Signal emitted whenever a config page is applied.
            void appliedPage(IConfigPage* conig_page);

        public slots:
            //! Handles item changes in the page tree.
            void handleActiveItemChanges(QList<QObject*> active_pages);
            //! Handles changes to active grouped pages.
            void handleActiveGroupedPageChanged(IConfigPage* new_active_grouped_page);
            //! Function to set the active page.
            void setActivePage(const QString& active_page_name);
            //! Function to get the active page name and category.
            QString activePageName() const;
            //! Function to get the reference to the IConfig interface of the active page.
            IConfigPage* activePageIFace() const;

        protected:
            void changeEvent(QEvent *e);

        private slots:
            void on_buttonBox_clicked(QAbstractButton *button);

        private:
            //! Adds icon property to a config page.
            void addPageIconProperty(IConfigPage* config_page);
            //! Adds category property to a config page.
            void addPageCategoryProperty(IConfigPage* config_page);

            Ui::ConfigurationWidget* ui;
            ConfigurationWidgetPrivateData* d;
        };
    }
}

#endif // CONFIGURATION_WIDGET_H
