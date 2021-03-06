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

#ifndef PROJECT_MANAGEMENT_PLUGIN_CORE_H
#define PROJECT_MANAGEMENT_PLUGIN_CORE_H

#include "ProjectManagementPlugin_global.h"

#include <IPlugin>
#include <QtilitiesCategory>
#include <IProject>
#include <QObject>

namespace Qtilities {
    namespace Plugins {
        namespace ProjectManagement {
            using namespace Qtilities::ExtensionSystem::Interfaces;
            using namespace Qtilities::ProjectManagement::Interfaces;
            using namespace Qtilities::Core;

            /*!
              \struct ProjectManagementPluginPrivateData
              \brief The ProjectManagementPluginPrivateData struct stores private data used by the ProjectManagementPlugin class.
             */
            struct ProjectManagementPluginPrivateData;

            /*!
              \class ProjectManagementPlugin
              \brief A plugin which adds project management capabilities to the application.
             */
            class PROJECT_MANAGEMENT_PLUGIN_SHARED_EXPORT ProjectManagementPlugin : public QObject, public IPlugin
            {
                Q_OBJECT
                Q_INTERFACES(Qtilities::ExtensionSystem::Interfaces::IPlugin)

            public:
                ProjectManagementPlugin(QObject* parent = 0);
                ~ProjectManagementPlugin();

                // --------------------------------
                // IObjectBase Implementation
                // --------------------------------
                QObject* objectBase() { return this; }
                const QObject* objectBase() const { return this; }

                // --------------------------------------------
                // IPlugin Implementation
                // --------------------------------------------
                bool initialize(const QStringList &arguments, QStringList *error_strings);
                bool initializeDependencies(QStringList *error_strings);
                void finalize();
                QString pluginName() const;
                QtilitiesCategory pluginCategory() const;
                VersionInformation pluginVersionInformation() const;
                QString pluginPublisher() const;
                QString pluginPublisherWebsite() const;
                QString pluginPublisherContact() const;
                QString pluginDescription() const;
                QString pluginCopyright() const;
                QString pluginLicense() const;

            public slots:
                void handle_actionProjectNew();
                void handle_actionProjectOpen();
                void handle_actionProjectClose();
                void handle_actionProjectSave();
                void handle_actionProjectSaveAs();
                void handle_projectStateChanged();
                void handleRecentProjectChanged();

            private slots:
                void handleRecentProjectActionTriggered();
                void handleApplicationBusyStateChanged();

            private:
                ProjectManagementPluginPrivateData* d;
            };
        }
    }
}

#endif // PROJECT_MANAGEMENT_PLUGIN_CORE_H
