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

#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QKeySequence>
#include <QShortcut>
#include <QAction>
#include <QPointer>
#include <QtilitiesCategory>

#include "QtilitiesCoreGui_global.h"

namespace Qtilities {
    namespace CoreGui {
        /*!
        \struct CommandPrivateData
        \brief A structure storing private data in the Command class.
          */
        struct CommandPrivateData {
            QKeySequence default_key_sequence;
            QKeySequence current_key_sequence;
            QString default_text;
        };

        /*!
        \class Command
        \brief A class which represents a command.

        A command is a class which is a wrapper for either of the following:
        - An application wide shortcut
        - A proxy action

        By registering all the shortcuts and actions in the application in one place, it is easier to manage
        and control them. The command class is also aware of the current context of the application and
        enables/disables shortcuts and actions depending on the context.

        Each command can be used accros multiple contexts. For example, you can present the user with a single command.
        The user will see one command (if the command contains an action) but the result of triggering the command
        will be different depending on the context.

        A command has a key sequence associated with it. If the command represents an application wide shortcut, the key
        sequence will be the sequence that triggers that shortcut. If the command is an action, the key sequence will
        trigger the action.

        The command class is an abstract base class and is reimplemented by the ProxyAction and Shortcut classes.

        For more information see the \ref page_action_management article.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT Command : public QObject
        {
            Q_OBJECT
            Q_PROPERTY(QKeySequence KeySequence READ keySequence WRITE setKeySequence)

        public:
            Command(int category_context, QObject* parent = 0);
            ~Command();

            //! Sets the default key sequence.
            void setDefaultKeySequence(const QKeySequence &key);
            //! Sets the current key sequence.
            void setKeySequence(const QKeySequence &key);
            //! Returns the default key sequence.
            QKeySequence defaultKeySequence() const;
            //! Returns the current key sequence.
            QKeySequence keySequence() const;
            //! Sets the default text (id) associated with this command.
            void setDefaultText(const QString &text);
            //! Gets the default text (id) associated with this command.
            QString defaultText() const;

            //! The action associated with this command. If the command is a wrapper for a shortcut, 0 is returned.
            virtual QAction *action() const = 0;
            //! The shortcut associated with this command. If the command is a wrapper for an action, 0 is returned.
            virtual QShortcut *shortcut() const = 0;
            //! Returns the text which appears as user friendly text to the user.
            virtual QString text() const = 0;
            //! Called when the key sequence changes.
            virtual void handleKeySequenceChange(const QKeySequence& old_key) = 0;
            //! Call to set change the curent context of a command.
            virtual bool setCurrentContext(QList<int> context_ids) = 0;

            //! Sets the command's category.
            void setCategory(Qtilities::Core::QtilitiesCategory category);
            //! Gets the command's category.
            Qtilities::Core::QtilitiesCategory category() const;

        signals:
            void keySequenceChanged();

        protected:
            static int d_category_context;
            CommandPrivateData* c;
        };
		
        /*!
        \struct ProxyActionPrivateData
        \brief A structure storing private data in the ProxyAction class.
          */
        struct ProxyActionPrivateData;

        /*!
        \class ProxyAction
        \brief A class which represents a proxy actionn which triggers different actions for different contexts.
        
        The ProxyAction class represents an action which has a proxy action which is triggered when the shortcut of the ProxyAction is used, and any number of backend actions which each represent a different context. When context changes happen in the application, the multi context action will make the correct backend action for the set of active contexts active. Thus, when the frontend action trigger, the active backend action will also trigger.

        When the active backend action changes, the proxy action inherts the properties of the backend action.

        For more information see the \ref page_action_management article.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ProxyAction : public Command
        {
            Q_OBJECT

        public:
            ProxyAction(QAction* proxy_action, int category_context, QObject* parent = 0);
            virtual ~ProxyAction();

            // --------------------------------
            // Command Implementation
            // --------------------------------
            QAction *action() const;
            QShortcut *shortcut() const;
            QString text() const;
            void handleKeySequenceChange(const QKeySequence& old_key);

            //! Add the action under the specified contexts.
            void addAction(QAction* action, QList<int> context_ids);
            //! Returns true if the action is active in any of the current active contexts.
            bool isActive();
            //! Let this action know what contexts are currently active.
            bool setCurrentContext(QList<int> context_ids);
            //! Provides a list of all the backend actions for this ProxyAction.
            QHash<int, QPointer<QAction> > contextIDActionMap() const;
            //! Provides the active backend action
            QPointer<QAction> activeBackendAction() const;

        private slots:
            void updateFrontendAction();

        private:
            ProxyActionPrivateData* d;
        };

        /*!
        \struct ShortcutCommandPrivateData
        \brief A structure storing private data in the ShortcutCommand class.
          */
        struct ShortcutCommandPrivateData;

        /*!
        \class ShortcutCommand
        \brief A class which represents a shortcut which is enabled depending on the active context(s).

        For more information see the \ref page_action_management article.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ShortcutCommand : public Command
        {
            Q_OBJECT

        public:
            ShortcutCommand(const QString& user_text, QShortcut *shortcut, const QList<int> &active_contexts, int category_context, QObject* parent = 0);
            virtual ~ShortcutCommand();

            // --------------------------------
            // Command Implementation
            // --------------------------------
            QAction *action() const;
            QShortcut *shortcut() const;
            QString text() const;
            void handleKeySequenceChange(const QKeySequence& old_key);

            //! Returns true if the shortcut command is active in any of the current active contexts.
            bool isActive();
            //! Let this shortcut command know what contexts are currently active.
            bool setCurrentContext(QList<int> context_ids);
            //! Returns a list of active contexts for this shortcut.
            QList<int> activeContexts() const;

        private:
            ShortcutCommandPrivateData* d;
        };
    }
}

#endif // COMMAND_H

