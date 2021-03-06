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

#ifndef LOGGERENGINES_H
#define LOGGERENGINES_H

#include "Logging_global.h"
#include "AbstractLoggerEngine.h"
#include "LoggingConstants.h"
#include "LoggerFactory.h"
#include "ILoggerExportable.h"

#include <QList>
#include <QVariant>

namespace Qtilities {
    namespace Logging {
        using namespace Qtilities::Logging::Interfaces;
        using namespace Qtilities::Logging::Constants;

        // ------------------------------------
        // File Logger Engine
        // ------------------------------------
        /*!
        \class FileLoggerEngine
        \brief A logger engine which stores the logged messages in a file.

        A logger engine which stores the logged messages in a file.
          */
        class LOGGING_SHARED_EXPORT FileLoggerEngine : public AbstractLoggerEngine, public ILoggerExportable
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Logging::Interfaces::ILoggerExportable)
            Q_PROPERTY(QString FileName READ getFileName)

        public:
            FileLoggerEngine();
            ~FileLoggerEngine();

            // --------------------------------
            // AbstractLoggerEngine Implementation
            // --------------------------------
            bool initialize();
            void finalize();
            QString description() const;
            QString status() const;
            bool isFormattingEngineConstant() const { return true; }
            /*!
              Clearing of FileLoggerEngine was introduced in %Qtilities v1.1.
              */
            void clearLog();

            // --------------------------------
            // ILoggerExportable Implementation
            // --------------------------------
            ExportModeFlags supportedFormats() const;
            bool exportBinary(QDataStream& stream) const;
            bool importBinary(QDataStream& stream);
            QString factoryTag() const { return qti_def_FACTORY_TAG_FILE_LOGGER_ENGINE; }
            QString instanceName() const { return name(); }

            //! Sets the file name to which this engine will write the log output.
            /*!
                Its not possible to change the file name while the logger engine is in a initialized state.
                To change the file name: call finalize(), setFileName() and then call initialize() again.
              */
            void setFileName(const QString& fileName);
            //! Gets the file name to which the logger is currently logging.
            QString getFileName();

            // Make this class a factory item
            static LoggerFactoryItem<AbstractLoggerEngine, FileLoggerEngine> factory;

        public slots:
            void logMessage(const QString& message, Logger::MessageType message_type);

        private:
            QString file_name;
        };

        // ------------------------------------
        // Qt Message Logger Engine
        // ------------------------------------
        /*!
        \class QtMsgLoggerEngine
        \brief A logger engine which pipes messages to the Qt Messaging System as debug messages.

        A logger engine which pipes messages to the Qt Messaging System as debug messages.

        \note Clearing the log through clearLog() is not supported by this logger engine.
        \note QtMsgLoggerEngine engines are not removeable.
          */
        class LOGGING_SHARED_EXPORT QtMsgLoggerEngine : public AbstractLoggerEngine
        {
            Q_OBJECT

        public:
            static QtMsgLoggerEngine* instance();
            ~QtMsgLoggerEngine();

        protected:
            QtMsgLoggerEngine();

        public:
            // AbstractLoggerEngine implementation
            bool initialize();
            void finalize();
            QString description() const;
            QString status() const;
            bool removable() const { return false; }
            bool isFormattingEngineConstant() const { return true; }

        public slots:
            void logMessage(const QString& message, Logger::MessageType message_type);

        private:
            static QtMsgLoggerEngine* m_Instance;
        };

        // ------------------------------------
        // Console Logger Engine
        // ------------------------------------
        /*!
        \class ConsoleLoggerEngine
        \brief A logger engine which pipes messages to a console using the stdio.h fprintf function.

        A logger engine which pipes messages to a console using the stdio.h fprintf function.

        \note Clearing the log through clearLog() is not supported by this logger engine.
        \note ConsoleLoggerEngine engines are not removeable.
          */
        class LOGGING_SHARED_EXPORT ConsoleLoggerEngine : public AbstractLoggerEngine
        {
            Q_OBJECT

        public:
            static ConsoleLoggerEngine* instance();
            ~ConsoleLoggerEngine();

        protected:
            ConsoleLoggerEngine();

        public:
            // AbstractLoggerEngine implementation
            bool initialize();
            void finalize();
            QString description() const;
            QString status() const;
            bool removable() const { return false; }
            bool isFormattingEngineConstant() const { return false; }

        public slots:
            void logMessage(const QString& message, Logger::MessageType message_type);

        private:
            static ConsoleLoggerEngine* m_Instance;
        };
    }
}

#endif // FILELOGGERENGINE_H
