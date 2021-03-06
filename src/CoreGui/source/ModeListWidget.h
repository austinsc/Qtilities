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

#ifndef MODE_LIST_WIDGET_H
#define MODE_LIST_WIDGET_H

#include <QListWidget>

/*!
\class ModeListWidget
\brief An internal list widget which shows items in a top to bottom view, centered in the middle of the view.
  */
class ModeListWidget : public QListWidget
{
public:
    ModeListWidget(Qt::Orientation orientation, QWidget* parent = 0);
    virtual QSize sizeHint() const;

    void setMinimumItemSize(QSize size) {
        min_size = size;
    }
    void setMaximumItemSize(QSize size) {
        max_size = size;
    }
    QSize minimumItemSize() const {
        return min_size;
    }
    QSize maximumItemSize() const {
        return max_size;
    }

private:
    Qt::Orientation d_orientation;
    QSize min_size;
    QSize max_size;
};

#endif // MODE_LIST_WIDGET_H
