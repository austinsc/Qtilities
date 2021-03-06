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

#include "PointerList.h"

Qtilities::Core::PointerList::PointerList(bool cleanup_when_done, QObject *parent) : PointerListDeleter() {
    Q_UNUSED(parent)

    cleanup_enabled = cleanup_when_done;
}

Qtilities::Core::PointerList::~PointerList() {
    if (cleanup_enabled)
        list.clear();
}

//! Appends a new instance of T to the PointerList.
void Qtilities::Core::PointerList::append(QObject* object) {
    addThisObject(object);
    list.append(object);
}

void Qtilities::Core::PointerList::deleteAll() {
    int total =list.count();
    for (int i = 0; i < total; i++) {
        delete list.at(0);
    }
    list.clear();
}

int Qtilities::Core::PointerList::count() const {
    return list.count();
}

QObject* Qtilities::Core::PointerList::at(int i) const {
    return list.at(i);
}

void Qtilities::Core::PointerList::removeThisObject(QObject * obj) {
    list.removeOne(obj);
    emit objectDestroyed(obj);
}

void Qtilities::Core::PointerList::removeOne(QObject* obj) {
    QObject::disconnect(obj, SIGNAL(destroyed(QObject *)), this, SLOT(removeSender()));
    list.removeOne(obj);
}

void Qtilities::Core::PointerList::addThisObject(QObject * obj) {
    QObject::connect(obj, SIGNAL(destroyed(QObject *)), this, SLOT(removeSender()));
}

QMutableListIterator<QObject*> Qtilities::Core::PointerList::iterator() {
    QMutableListIterator<QObject*> itr(list);
    return itr;
}

QList<QObject*> Qtilities::Core::PointerList::toQList() const {
    return list;
}
