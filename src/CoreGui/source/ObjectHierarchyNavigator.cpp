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

#include "ObjectHierarchyNavigator.h"
#include "ui_ObjectHierarchyNavigator.h"
#include "QtilitiesApplication.h"

#include <Observer.h>

using namespace Qtilities::Core;

Qtilities::CoreGui::ObjectHierarchyNavigator::ObjectHierarchyNavigator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ObjectHierarchyNavigator)
{
    ui->setupUi(this);
    d_current_obj = 0;
}

Qtilities::CoreGui::ObjectHierarchyNavigator::~ObjectHierarchyNavigator()
{
    delete ui;
}

void Qtilities::CoreGui::ObjectHierarchyNavigator::setCurrentObject(QObject* obj) {
    if (obj) {
        if (d_current_obj)
            d_current_obj->disconnect(this);
        d_current_obj = obj;
        
        // This object will always be an observer.
        // Observer* observer = qobject_cast<Observer*> (d_current_obj);
        //if (observer)
            //connect(observer,SIGNAL(nameChanged(QString)),SLOT(refreshHierarchy()));
    }

    d_navigation_stack.clear();
}

void Qtilities::CoreGui::ObjectHierarchyNavigator::setNavigationStack(QStack<int> navigation_stack) {
    // First disconnect all connections to the current stack
    Observer* observer = 0;
    for (int i = 0; i < d_navigation_stack.count(); i++) {
        observer = OBJECT_MANAGER->observerReference(d_navigation_stack.at(i));
        observer->disconnect(this);
    }

    d_navigation_stack = navigation_stack;
    refreshHierarchy();
}

void Qtilities::CoreGui::ObjectHierarchyNavigator::refreshHierarchy() {
    // For this version, we make it simple. Just display the hierarchy.
    QString label_text = QString("");

    Observer* observer = 0;
    bool read_only = false;
    for (int i = d_navigation_stack.count()-1; i >= 0; i--) {
        observer = OBJECT_MANAGER->observerReference(d_navigation_stack.at(i));
        if (!observer)
            return;

        Q_ASSERT(observer);
        if (observer) {
            if (observer->accessMode() == Observer::ReadOnlyAccess)
                read_only = true;
        }

        if (i > 0) {
            if (read_only)
                label_text.prepend(QString("<font color = 'red'>%1</color> :: ").arg(observer->observerName(d_navigation_stack.at(i-1))));
            else {
                // TODO: This is a bug, the navigation stack is not used correctly, since observer is not contained in the ID used.
                label_text.prepend(QString("%1 :: ").arg(observer->observerName(d_navigation_stack.at(i-1))));
            }
        } else {
            if (read_only)
                label_text.prepend(QString("<font color = 'red'>%1</color> :: ").arg(observer->observerName()));
            else
                label_text.prepend(QString("%1 :: ").arg(observer->observerName()));
        }
    }

    if (d_current_obj) {
        // Get the object name for the current parent context
        if (d_navigation_stack.count() > 0) {
            observer = qobject_cast<Observer*> (d_current_obj);
            if (observer) {
                if (observer->accessMode() == Observer::ReadOnlyAccess)
                    read_only = true;
            }
            observer = OBJECT_MANAGER->observerReference(d_navigation_stack.at(d_navigation_stack.count()-1));
            if (read_only)
                label_text.append(QString("<font color = 'red'>%1</color>").arg(observer->subjectDisplayedNameInContext(d_current_obj)));
            else
                label_text.append(QString("%1").arg(observer->subjectDisplayedNameInContext(d_current_obj)));
        } else {
            observer = qobject_cast<Observer*> (d_current_obj);
            if (observer) {
                if (observer->accessMode() == Observer::ReadOnlyAccess)
                    read_only = true;
            }
            if (read_only)
                label_text.append(QString("<font color = 'red'>%1</color>").arg(d_current_obj->objectName()));
            else
                label_text.append(QString("%1").arg(d_current_obj->objectName()));
        }

    } else
        label_text.append(tr("Current Object"));

    if (label_text.length() == 0)
        label_text = tr("Invalid observer name received.");

    ui->lblHierarchy->setText(label_text);
}

void Qtilities::CoreGui::ObjectHierarchyNavigator::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
