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

#ifndef TREE_ITERATOR_H
#define TREE_ITERATOR_H

#include <QObject>
#include <QString>

#include <QtilitiesLogging>

#include "IIterator.h"
#include "Observer.h"
#include "QtilitiesCore_global.h"
#include "SubjectIterator.h"

namespace Qtilities {
    namespace Core {
        /*!
        \class TreeIterator
        \brief An iterator which iterates through an Observer tree (thus also Qtilities::CoreGui::TreeNode).

        The TreeIterator allows you to easily iterate over the items in a tree. Take the tree shown below as an example:

        \image html trees_full_iteration.jpg "Tree Iteration Order"

        Lets build this example tree and iterate over it:

\code
TreeNode* rootNode = new TreeNode("1");
TreeNode* nodeA = rootNode->addNode("2");
nodeA->addItem("3");
nodeA->addItem("4");
TreeNode* nodeB = rootNode->addNode("5");
nodeB->addItem("6");
nodeB->addItem("7");

TreeIterator itr(rootNode);
qDebug() <<  itr.current()->objectName();
while (itr.hasNext()) {
    qDebug() <<  itr.next()->objectName();
}

// In this case the result would be:
// >> 1
// >> 2
// >> 3
// >> 4
// >> 5
// >> 6
// >> 7

// We can also iterate backwards:
qDebug() << itr.current()->objectName();
while (itr.hasPrevious()) {
    qDebug() << itr.previous()->objectName();
}

// In this case the result would be:
// >> 7
// >> 6
// >> 5
// >> 4
// >> 3
// >> 2
// >> 1
\endcode

        \section tree_iterator_multiple_parents When subjects are attached to multiple parents

        Because %Qtilities allows tree items to be attached to multiple trees, TreeIterator needs the ability to iterate through trees where subjects can appear more than once. Lets look at an example:

        \image html trees_multiple_parent_iteration.jpg "Advanced Tree Iteration"

        The above tree is constructed using the code snippet below:

\code
TreeNode* rootTop = new TreeNode("Root Node");

// This is the first tree (A):
TreeNode* rootNodeA = new TreeNode("A1");
rootNodeA->addItem("A2");
TreeItem* shared_item = rootNodeA->addItem("A3"); // This is the shared item.
QObject* lastA = rootNodeA->addItem("A4");

// This is the second tree (B):
TreeNode* rootNodeB = new TreeNode("B1");
rootNodeB->addItem("B2");
rootNodeB->attachSubject(shared_item); // Here we attach the shared item to another tree's node.
rootNodeB->addItem("B3");

rootTop->attachSubject(rootNodeA);
rootTop->attachSubject(rootNodeB);
\endcode

        When TreeIterator iterates through this tree two things can happen:
        - When you iterate forward in the tree it will remember the path it has taken to get to any point and you don't have to worry about it. TreeIterator will be able
          to iterate through the tree regardless of any multiple parents that it might find on its way.
        - When iterating backwards through the tree, the iterator will always iterate through the tree from front to back to get to the last value which you want to start
          with. Thus, the path information will also be stored for you and TreeIterator will be able to iterate through the tree regardless of any multiple parents that
          it might find on its way.

        \sa SubjectIterator, ConstSubjectIterator

        <i>This class was added in %Qtilities v1.0.</i>
        */
        class TreeIterator : public Interfaces::IIterator<QObject>
        {
        public:
            TreeIterator(const Observer* top_node = 0)
                : d_top_node(top_node)
            {
                d_current = top_node;
            }

            QObject* first()
            {
                d_current = d_top_node;
                return const_cast<QObject*> (d_current);
            }

            QObject* last()
            {
                d_current = d_top_node->treeChildren().last();
                return const_cast<QObject*> (d_current);
            }

            QObject* current() const
            {
                return const_cast<QObject*> (d_current);
            }

            void setCurrent(const QObject* current)
            {
                d_current = current;
            }

            QObject* next()
            {
                Observer* obs = qobject_cast<Observer*> (const_cast<QObject*> (d_current));
                if (obs) {
                    if (obs->subjectCount() > 0) {
                        d_current = obs->subjectAt(0);

                        // Check if any subjects under this observer has multiple parents. If so, we need to set the qti_prop_TREE_ITERATOR_SOURCE_OBS on all
                        // subjects in this context since SubjectIterator needs it on all subjects. TreeIterator only needs it on the first and last subject.
                        bool add_prop = false;
                        for (int i = 0; i < obs->subjectCount(); i++) {
                            if (Observer::parentCount(obs->subjectAt(i)) > 1) {
                                add_prop = true;
                                continue;
                            }
                        }

                        if (add_prop) {
                            for (int i = 0; i < obs->subjectCount(); i++) {
                                ObjectManager::setSharedProperty(obs->subjectAt(i),qti_prop_TREE_ITERATOR_SOURCE_OBS,obs->observerID());
                                //qDebug() << "TreeIterator: Setting qti_prop_TREE_ITERATOR_SOURCE_OBS on subject" << obs->subjectAt(i) << "with ID of parent" << obs->observerName();
                            }
                        }

                        return const_cast<QObject*> (d_current);
                    } else {
                        if (d_current == d_top_node)
                            return 0;
                        else {
                            SubjectIterator<QObject> sibling_itr(obs,SubjectIterator<QObject>::IterateSiblings);
                            if (sibling_itr.hasNext()) {
                                d_current = sibling_itr.next();
                                return const_cast<QObject*> (d_current);
                            } else {
                                const QObject* obj = findParentNext(d_current);
                                if (obj) {
                                    d_current = obj;
                                    return const_cast<QObject*> (d_current);
                                } else
                                    return 0;
                            }
                        }
                    }
                } else {
                    SubjectIterator<QObject> sibling_itr(d_current);
                    if (sibling_itr.hasNext()) {
                        d_current = sibling_itr.next();
                        return const_cast<QObject*> (d_current);
                    } else {
                        const QObject* obj = findParentNext(d_current);
                        if (obj) {
                            d_current = obj;
                            return const_cast<QObject*> (d_current);
                        } else
                            return 0;
                    }
                }
            }

            QObject* previous()
            {
                if (d_current == d_top_node)
                    return 0;

                Observer* obs = qobject_cast<Observer*> (const_cast<QObject*> (d_current));
                if (obs) {
                    SubjectIterator<Observer> sibling_itr(obs,SubjectIterator<Observer>::IterateSiblings);
                    if (sibling_itr.hasPrevious()) {
                        Observer* previous_obs = sibling_itr.previous();

                        // If previous observer has children, take the last child, otherwise
                        // take previous observer.
                        QList<QObject*> previous_obs_children = previous_obs->treeChildren();
                        if (previous_obs_children.count() > 0) {
                            d_current = previous_obs_children.last();

                            // Check if any subjects under this observer has multiple parents. If so, we need to set the qti_prop_TREE_ITERATOR_SOURCE_OBS on all
                            // subjects in this context since SubjectIterator needs it on all subjects. TreeIterator only needs it on the first and last subject.
                            bool add_prop = false;
                            for (int i = 0; i < previous_obs->subjectCount(); i++) {
                                if (Observer::parentCount(previous_obs->subjectAt(i)) > 1) {
                                    add_prop = true;
                                    continue;
                                }
                            }
                            if (add_prop) {
                                for (int i = 0; i < previous_obs->subjectCount(); i++) {
                                    ObjectManager::setSharedProperty(previous_obs->subjectAt(i),qti_prop_TREE_ITERATOR_SOURCE_OBS,previous_obs->observerID());
                                    //qDebug() << "TreeIterator: Setting qti_prop_TREE_ITERATOR_SOURCE_OBS in previous() on subject" << previous_obs->subjectAt(i) << "with ID of parent" << previous_obs->observerName();
                                }
                            }

                            return const_cast<QObject*> (d_current);
                        } else {
                            d_current = previous_obs;
                            return const_cast<QObject*> (d_current);
                        }
                    } else {
                        const QObject* obj = findParentPrevious(d_current);
                        if (obj) {
                            d_current = obj;
                            return const_cast<QObject*> (d_current);
                        } else
                            return 0;
                    }
                } else {
                    SubjectIterator<QObject> sibling_itr(d_current);
                    if (sibling_itr.hasPrevious()) {
                        d_current = sibling_itr.previous();
                        return const_cast<QObject*> (d_current);
                    } else {
                        const QObject* obj = findParentPrevious(d_current);
                        if (obj) {
                            d_current = obj;
                            return const_cast<QObject*> (d_current);
                        } else
                            return 0;
                    }
                }
            }

            Observer* topNode() const {
                return const_cast<Observer*> (d_top_node);
            }

        protected:
            //! Finds the next parent of an object.
            const QObject* findParentNext(const QObject* obj) {
                if (!obj)
                    return 0;

                QList<Observer*> parents = Observer::parentReferences(obj);
                if (parents.count() == 1) {
                    if (parents.front() == d_top_node)
                        return 0;
                    else {
                        SubjectIterator<QObject> parent_itr(parents.front(),SubjectIterator<QObject>::IterateSiblings);
                        if (parent_itr.hasNext()) {
                            return parent_itr.next();
                        } else {
                            return findParentNext(parents.front());
                        }
                    }
                } else if (parents.count() > 1) {
                    // In here we set the observer ID of obs on the last and the first subjects in the observer context.
                    SharedProperty prop = ObjectManager::getSharedProperty(obj,qti_prop_TREE_ITERATOR_SOURCE_OBS);
//                    QObject* non_const_obj = const_cast<QObject*> (obj);
//                    non_const_obj->setProperty(qti_prop_TREE_ITERATOR_SOURCE_OBS,QVariant());
                    if (prop.isValid()) {
                        int obs_id = prop.value().toInt();
                        Observer* parent_obs = OBJECT_MANAGER->observerReference(obs_id);
                        if (parent_obs) {
                            if (parent_obs == d_top_node)
                                return 0;

                            SubjectIterator<QObject> parent_itr(parent_obs,SubjectIterator<QObject>::IterateSiblings);
                            if (parent_itr.hasNext()) {
                                return parent_itr.next();
                            } else {
                                return findParentNext(parents.front());
                            }
                        } else {
                            qWarning() << "TreeIterator: Invalid observer set through qti_prop_TREE_ITERATOR_SOURCE_OBS";
                            LOG_FATAL("TreeIterator: Invalid observer set through qti_prop_TREE_ITERATOR_SOURCE_OBS");
                            Q_ASSERT(parent_obs);
                        }
                    }
                }
                return 0;
            }
            //! Finds the previous parent of an object.
            const QObject* findParentPrevious(const QObject* obj) {
                QList<Observer*> parents = Observer::parentReferences(obj);
                if (parents.count() == 1)
                    return parents.front();
                else if (parents.count() > 1) {
                    // In here we set the observer ID of obs on the last and the first subjects in the observer context.
                    SharedProperty prop = ObjectManager::getSharedProperty(obj,qti_prop_TREE_ITERATOR_SOURCE_OBS);
//                    QObject* non_const_obj = const_cast<QObject*> (obj);
//                    non_const_obj->setProperty(qti_prop_TREE_ITERATOR_SOURCE_OBS,QVariant());
                    if (prop.isValid()) {
                        int obs_id = prop.value().toInt();
                        Observer* parent_obs = OBJECT_MANAGER->observerReference(obs_id);
                        if (parent_obs) {
                            return parent_obs;
                        } else {
                            qWarning() << "TreeIterator: Invalid observer set through qti_prop_TREE_ITERATOR_SOURCE_OBS";
                            LOG_FATAL("TreeIterator: Invalid observer set through qti_prop_TREE_ITERATOR_SOURCE_OBS");
                            Q_ASSERT(parent_obs);
                        }
                    }
                }
                return 0;
            }

        private:
            const QObject* d_current;
            const Observer* const d_top_node;
        };
    }
}
#endif // TREE_ITERATOR_H
