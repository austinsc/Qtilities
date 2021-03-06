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

#ifndef SUBJECT_ITERATOR_H
#define SUBJECT_ITERATOR_H

#include <QObject>
#include <QString>

#include <QtilitiesLogging>

#include "IIterator.h"
#include "QtilitiesCore_global.h"
#include "Observer.h"

namespace Qtilities {
    namespace Core {
            /*!
            \class SubjectIterator
            \brief An non-const iterator which iterates throught the subjects of an Observer.

            The SubjectIterator allows you to easily iterate over the subjects in an observer in the order shown below:

            \image html trees_subject_iteration.jpg "Subject Iteration Order"

            Lets build this example tree:

\code
TreeNode node;
TreeItem* item1 = node.addItem("1");
TreeItem* item2 = node.addItem("2");
node.addItem("3");
node.addItem("4");

SubjectIterator<Qtilities::CoreGui::TreeItem> itr(item1);

qDebug() << itr.current()->objectName();
while (itr.hasNext()) {
    qDebug() << itr.next()->objectName();
}

// In this case the result would be:
// >> 1
// >> 2
// >> 3
// >> 4
\endcode

            It is also possible to specify the current location of your iterator:

\code
SubjectIterator<Qtilities::CoreGui::TreeItem> itr(item2);

// In this case item1 will be skipped:
qDebug() << itr.current()->objectName();
while (itr.hasNext()) {
    qDebug() << itr.next()->objectName();
}

// In this case the result would be:
// >> 2
// >> 3
// >> 4
\endcode

            In the simple examples above we didn't need to worry about cases where tree items can have multiple parents. If they do have multiple parents, we must specify the Observer that we are interested in:

\code
TreeNode node;
TreeNode* nodeA = node.addNode("A");
TreeNode* nodeB = node.addNode("B");
nodeA->addItem("1");
TreeItem* shared_item = nodeA->addItem("2");
nodeA->addItem("3");
nodeB->addItem("4");
nodeB->addItem("5");
nodeB->attachSubject(shared_item);
nodeB->addItem("6");

// If we want to iterate through the subjects in nodeA:
SubjectIterator<QObject> itrA(nodeA,SubjectIterator<QObject>::IterateChildren);

// In this case item1 will not be skipped (approach 1):
while (itrA.hasNext()) {
    qDebug() << itrA.current()->objectName();
    itrA.next();
}

// In this case the result would be:
// >> 1
// >> 2
// >> 3

// If we want to iterate through the subjects in nodeB:
SubjectIterator<QObject> itrB(nodeB,SubjectIterator<QObject>::IterateChildren);

// In this case item1 will be skipped (approach 2):
qDebug() << itrB.current()->objectName();
while (itrB.hasNext()) {
    qDebug() << itrB.next()->objectName();
}

// In this case the result would be:
// >> 4
// >> 5
// >> 2
// >> 6
\endcode

            \sa ConstSubjectIterator, TreeIterator

            <i>This class was added in %Qtilities v1.0.</i>
        */
        template <class T>
        class SubjectIterator : public Interfaces::IIterator<T>
        {
        public:
            //! The level to use when passing only an observer in your constructor.
            enum ObserverIterationLevel {
                IterateChildren,    /*!< Iterate on the observer's children. */
                IterateSiblings     /*!< Iterate on the observer's siblings. In this case T must be a subclass of Observer. */
            };

            //! Default constructor.
            /*!
              \param subject The current subject where your iterator must start.
              \param observer This is an optional parameter which is needed when your subject is observed in multiple contexts. In that case, the observer must be specified in order to know which observer parent to use.
              */
            SubjectIterator(const T* subject, Observer* observer = 0) :
                d_root(subject),
                d_parent_observer(observer)
            {
                d_current = subject;
            }
            //! Observer based constructor.
            /*!
              \param observer The observer that must be used in cases where multiple subjects have multiple parents.
              \param iteration_level Indicates on which level the observer must be interated.
              */
            SubjectIterator(const Observer* observer, ObserverIterationLevel iteration_level) :
                d_root(0),
                d_parent_observer(observer)
            {
                if (iteration_level == IterateChildren) {
                    QList<QObject*> subjects = observer->subjectReferences();
                    if (subjects.count() > 0) {
                        d_current = qobject_cast<T*>(subjects[0]);
                        d_root = qobject_cast<T*>(subjects[0]);
                    } else
                        d_current = 0;
                } else if (iteration_level == IterateSiblings) {
                    d_current = observer;
                    d_parent_observer = 0;
                    d_root = observer;
                }
            }

            T* first() {
                QList<QObject*> subjects = getSubjects(getParent());
                if(subjects.count() != 0)
                    d_current = qobject_cast<T*>(subjects[0]);
                else
                    d_current = NULL;
                return const_cast<T*> (d_current);
            }

            T* last() {
                QList<QObject*> subjects = getSubjects(getParent());
                if(subjects.count() != 0)
                    d_current = qobject_cast<T*>(subjects[subjects.count() - 1]);
                else
                    d_current = NULL;
                return const_cast<T*> (d_current);
            }

            T* current() const {
                return const_cast<T*> (d_current);
            }

            void setCurrent(const T* current) {
                d_current = current;
            }

            T* next() {
                QList<QObject*> subjects = getSubjects(getParent());
                int current_index = getCurrentIndex(subjects);

//                for (int i = 0; i < subjects.count(); i++) {
//                    qDebug() << "Siblings(" << QString::number(i) << "): " << subjects.at(i)->objectName();
//                }
                // The subject was found:
                if (current_index != -1) {
                    // Check if there is another subject after this one :
                    if (current_index < (subjects.count() - 1)) {
                        d_current = qobject_cast<T*>(subjects[current_index + 1]);
//                        qDebug() << "Next sibling: " << d_current->objectName();
                        return const_cast<T*> (d_current);
                    }
                }

                d_current = 0;
                return const_cast<T*> (d_current);
            }

            T* previous() {
                QList<QObject*> subjects = getSubjects(getParent());
                int current_index = getCurrentIndex(subjects);
                if(current_index > 0) {
                    d_current = qobject_cast<T*>(subjects[current_index - 1]);
                    return const_cast<T*> (d_current);
                }

                d_current = NULL;
                return const_cast<T*> (d_current);
            }

            Observer* iterationContext() const {
                return d_parent_observer;
            }

        protected:
            const Observer* getParent() {
                QList<Observer*> parents = Observer::parentReferences(d_root);
                if (parents.count() > 1) {
                    if (d_parent_observer)
                        return d_parent_observer;
                    else {
                        // In here we set the observer ID of obs on the last and the first subjects in the observer context.
                        SharedProperty prop = ObjectManager::getSharedProperty(d_root,qti_prop_TREE_ITERATOR_SOURCE_OBS);
                        if (prop.isValid()) {
                            int obs_id = prop.value().toInt();
                            Observer* parent_obs = OBJECT_MANAGER->observerReference(obs_id);
                            if (parent_obs) {
                                return parent_obs;
                            } else {
                                qWarning() << "SubjectIterator: Invalid observer set through qti_prop_TREE_ITERATOR_SOURCE_OBS";
                                LOG_FATAL("SubjectIterator: Invalid observer set through qti_prop_TREE_ITERATOR_SOURCE_OBS");
                                Q_ASSERT(parent_obs);
                            }
                        }
                    }
                    return 0;
                } else if (parents.count() == 1)
                    return parents.front();
                else
                    return 0;
            }

            QList<QObject*> getSubjects(const Observer* parent)
            {
                QList<QObject*> subjects;
                if (parent)
                    subjects = parent->subjectReferences();

                return subjects;
            }

            int getCurrentIndex(QList<QObject*> subjects)
            {
                int current_index = -1;
                for(int subject = 0; subject < subjects.count(); subject++)
                {
                    if(subjects[subject] == d_current)
                        current_index = subject;
                }
                return current_index;
            }

        private:
            const T* d_current;
            const T* d_root;
            const Observer* d_parent_observer;
        };

        // -----------------------------------------------
        // ConstSubjectIterator
        // -----------------------------------------------
        /*!
          \class ConstSubjectIterator
          \brief An non-const iterator which iterates throught the subjects of an Observer.

           \sa SubjectIterator, TreeIterator

           <i>This class was added in %Qtilities v1.0.</i>
         */
        template <class T>
        class ConstSubjectIterator : public Interfaces::IConstIterator<T>
        {
        public:
            //! The level to use when passing only an observer in your constructor.
            enum ObserverIterationLevel {
                IterateChildren,    /*!< Iterate on the observer's children. */
                IterateSiblings     /*!< Iterate on the observer's siblings. In this case T must be a subclass of Observer. */
            };

            //! Default constructor.
            /*!
              \param subject The current subject where your iterator must start.
              \param observer This is an optional parameter which is needed when your subject is observed in multiple
              contexts. In that case, the observer must be specified in order to know which observer parent to use.
              */
            ConstSubjectIterator(const T* subject, Observer* observer = 0) :
                d_root(subject),
                d_parent_observer(observer)
            {
                d_current = subject;
            }
            //! Observer based constructor.
            /*!
              \param observer The observer that must be used in cases where multiple subjects have multiple parents.
              \param iteration_level Indicates on which level the observer must be interated.
              */
            ConstSubjectIterator(const Observer* observer, ObserverIterationLevel iteration_level) :
                d_root(0),
                d_parent_observer(observer)
            {
                if (iteration_level == IterateChildren) {
                    QList<QObject*> subjects = observer->subjectReferences();
                    if (subjects.count() > 0) {
                        d_current = qobject_cast<T*>(subjects[0]);
                        d_root = qobject_cast<T*>(subjects[0]);
                    } else
                        d_current = 0;
                } else if (iteration_level == IterateSiblings) {
                    d_current = observer;
                    d_parent_observer = 0;
                    d_root = observer;
                }
            }

            const T* first()
            {
                QList<QObject*> subjects = getSubjects(getParent());
                if(subjects.count() != 0)
                    d_current = qobject_cast<T*>(subjects[0]);
                else
                    d_current = NULL;
                return d_current;
            }

            const T* last()
            {
                QList<QObject*> subjects = getSubjects(getParent());
                if(subjects.count() != 0)
                    d_current = qobject_cast<T*>(subjects[subjects.count() - 1]);
                else
                    d_current = NULL;
                return d_current;
            }

            const T* current() const
            {
                return d_current;
            }

            void setCurrent(const T* current)
            {
                d_current = current;
            }

            const T* next()
            {
                QList<QObject*> subjects = getSubjects(getParent());
                int current_index = getCurrentIndex(subjects);

                /* the subject was found*/
                if(current_index != -1)
                {
                    /* Check if there is another subject after this one */
                    if(current_index < (subjects.count() - 1))
                    {
                        d_current = qobject_cast<T*>(subjects[current_index + 1]);
                        return d_current;
                    }
                }

                d_current = NULL;
                return d_current;
            }

            const T* previous()
            {
                QList<QObject*> subjects = getSubjects(getParent());
                int current_index = getCurrentIndex(subjects);
                if(current_index > 0)
                {
                    d_current = qobject_cast<T*>(subjects[current_index - 1]);
                    return d_current;
                }

                d_current = NULL;
                return d_current;
            }


            Observer* iterationContext() const {
                return d_parent_observer;
            }

        protected:
            Observer* getParent()
            {
                QList<Observer*> parents = Observer::parentReferences(d_root);
                if (parents.count() > 1) {
                    if (d_parent_observer)
                        return d_parent_observer;
                    else
                        return parents.front();
                } else if (parents.count() == 1)
                    return parents.front();
                else
                    return 0;
            }

            QList<QObject*> getSubjects(Observer* parent)
            {
                QList<QObject*> subjects;
                if (parent)
                    subjects = parent->subjectReferences();

                return subjects;
            }

            int getCurrentIndex(QList<QObject*> subjects)
            {
                int current_index = -1;
                for(int subject = 0; subject < subjects.count(); subject++)
                {
                    if(subjects[subject] == d_current)
                        current_index = subject;
                }
                return current_index;
            }

        private:
            const T* d_current;
            const T* d_root;
            Observer* d_parent_observer;
        };
    }
}
#endif // SUBJECT_ITERATOR_H

