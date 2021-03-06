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

#ifndef TREE_ITEM_BASE_H
#define TREE_ITEM_BASE_H

#include "QtilitiesCoreGui_global.h"
#include "AbstractTreeItem.h"

#include <IModificationNotifier>
#include <IExportable>

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::Core;

        /*!
        \struct TreeItemBasePrivateData
        \brief Structure used by TreeItemBase to store private data.
          */
        struct TreeItemBasePrivateData;

        /*!
          \class TreeItemBase
          \brief The TreeItemBase class is the base class for all tree items, that is items which can be attached to TreeNode instances.

          <i>This class was added in %Qtilities v0.2.</i>
        */
        class QTILITIES_CORE_GUI_SHARED_EXPORT TreeItemBase : public QObject, public AbstractTreeItem,
                                                              public IModificationNotifier, virtual public IExportable
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IModificationNotifier)

        public:
            //! Constructs the base of a tree item.
            /*!
              \param name The name of the item.
              \param parent The parent of the item.
              */
            TreeItemBase(const QString& name = QString(), QObject* parent = 0);
            virtual ~TreeItemBase();

            // --------------------------------
            // AbstractTreeItem virtual overwrites for category functions:
            // --------------------------------
            /*!
              This function will automatically call setModificationState(true) when needed.
              */
            virtual bool setCategory(const QtilitiesCategory& category, TreeNode* tree_node);
            /*!
              This function will automatically call setModificationState(true) when needed.
              */
            virtual bool setCategory(const QtilitiesCategory& category, int observer_id = -1);
            /*!
              This function will automatically call setModificationState(true) when needed.
              */
            virtual bool setCategoryString(const QString& category_string, const QString& sep = "::");
            virtual QtilitiesCategory getCategory(TreeNode* tree_node) const;
            virtual QtilitiesCategory getCategory(int observer_id = -1) const;
            virtual QString getCategoryString(const QString& sep = "::",int observer_id = -1) const;
            virtual bool hasCategory() const;
            /*!
              This function will automatically call setModificationState(true) when needed.
              */
            virtual bool removeCategory(int observer_id = -1);

            // --------------------------------
            // IModificationNotifier Implementation
            // --------------------------------
            virtual bool isModified() const;
        public slots:
            virtual void setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets = IModificationNotifier::NotifyListeners, bool force_notifications = false);
        signals:
            void modificationStateChanged(bool is_modified) const;

        protected:
            TreeItemBasePrivateData* itemBaseData;
        };
    }
}

#endif //  TREE_ITEM_BASE_H
