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

#include "ObserverTreeModelProxyFilter.h"
#include "ObserverTreeModel.h"
#include "QtilitiesCoreGuiConstants.h"

#include <Observer.h>
#include <QtilitiesCoreConstants.h>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::Core::Properties;
using namespace Qtilities::Core;
using namespace Qtilities::Core::Constants;

Qtilities::CoreGui::ObserverTreeModelProxyFilter::ObserverTreeModelProxyFilter(QObject* parent) : QSortFilterProxyModel(parent) {
    row_filter_types = ObserverTreeItem::TreeItem;
}

Qtilities::CoreGui::ObserverTreeModelProxyFilter::~ObserverTreeModelProxyFilter() {

}

bool Qtilities::CoreGui::ObserverTreeModelProxyFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    ObserverTreeModel* tree_model = dynamic_cast<ObserverTreeModel*> (sourceModel());

    if (tree_model) {
        // Get the ObserverTreeItem:
        QModelIndex name_index = sourceModel()->index(sourceRow, tree_model->columnPosition(AbstractObserverItemModel::ColumnName), sourceParent);
        ObserverTreeItem* tree_item = tree_model->getItem(name_index);
        if (tree_item) {
            // Don't ever filter the root item:
            if (tree_item->itemType() == ObserverTreeItem::TreeNode && tree_item->parentItem()) {
                if (tree_item->parentItem()->objectName() == "Root Item")
                    return true;
            }

            // Filter by type:
            if (!(row_filter_types & tree_item->itemType()))
                return true;
        }
    }
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow,sourceParent);
}

void Qtilities::CoreGui::ObserverTreeModelProxyFilter::setRowFilterTypes(ObserverTreeItem::TreeItemTypeFlags type_flags) {
    row_filter_types = type_flags;
}

Qtilities::CoreGui::ObserverTreeItem::TreeItemTypeFlags Qtilities::CoreGui::ObserverTreeModelProxyFilter::rowFilterTypes() const {
    return row_filter_types;
}

bool Qtilities::CoreGui::ObserverTreeModelProxyFilter::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    ObserverTreeModel* tree_model = dynamic_cast<ObserverTreeModel*> (sourceModel());

    if (tree_model) {
        QModelIndex left_name_index = sourceModel()->index(left.row(), tree_model->columnPosition(AbstractObserverItemModel::ColumnName), left.parent());
        QModelIndex right_name_index = sourceModel()->index(right.row(), tree_model->columnPosition(AbstractObserverItemModel::ColumnName), right.parent());
        ObserverTreeItem* left_item = tree_model->getItem(left_name_index);
        ObserverTreeItem* right_item = tree_model->getItem(right_name_index);

        if (left_item && right_item) {
            // We check according to the following criteria:
            // Categories are smaller than normal items:
            if (left_item->itemType() == ObserverTreeItem::CategoryItem && right_item->itemType() == ObserverTreeItem::CategoryItem) {
                // Check the names of the category
                return QString::localeAwareCompare(left_item->category().categoryTop(), right_item->category().categoryTop()) < 0;;
            } else if (left_item->itemType() == ObserverTreeItem::CategoryItem && right_item->itemType() == ObserverTreeItem::TreeItem)
                return true;
            else if (left_item->itemType() == ObserverTreeItem::TreeItem && right_item->itemType() == ObserverTreeItem::CategoryItem)
                return false;
            else if (left_item->itemType() == ObserverTreeItem::TreeNode && right_item->itemType() == ObserverTreeItem::CategoryItem)
                return true;
            else if (left_item->itemType() == ObserverTreeItem::CategoryItem && right_item->itemType() == ObserverTreeItem::TreeNode)
                return false;
            else if (left_item->itemType() == ObserverTreeItem::TreeNode && right_item->itemType() == ObserverTreeItem::TreeItem)
                return true;
            else if (left_item->itemType() == ObserverTreeItem::TreeItem && right_item->itemType() == ObserverTreeItem::TreeNode)
                return false;
        }
    }

    return QSortFilterProxyModel::lessThan(left,right);
}
