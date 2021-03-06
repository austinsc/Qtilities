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

#include "LoggerEngines.h"
#include "LoggingConstants.h"

#include <QFile>
#include <QTextStream>
#include <QList>
#include <QString>
#include <QMutex>

#include <stdio.h>

using namespace Qtilities::Logging;
using namespace Qtilities::Logging::Constants;
using namespace Qtilities::Logging::Interfaces;

// ------------------------------------
// FileLoggerEngine implementation
// ------------------------------------
namespace Qtilities {
    namespace Logging {
        LoggerFactoryItem<AbstractLoggerEngine, FileLoggerEngine> FileLoggerEngine::factory;
    }
}

Qtilities::Logging::FileLoggerEngine::FileLoggerEngine() : AbstractLoggerEngine()
{
    file_name = QString();
    abstractLoggerEngineData->formatting_engine = 0;
    setName(QObject::tr("File Logger Engine"));
}

Qtilities::Logging::FileLoggerEngine::~FileLoggerEngine()
{
    finalize();
}

bool Qtilities::Logging::FileLoggerEngine::initialize() {
    if (file_name.isEmpty()) {
        LOG_ERROR(QString(tr("Failed to initialize file logger engine (%1): File name is empty...").arg(objectName())));
        return false;
    }

    if (!abstractLoggerEngineData->formatting_engine) {
        // Attempt to get the formatting engine with the specified file format.
        QFileInfo fi(file_name);
        QString extension = fi.fileName().split(".").last();
        AbstractFormattingEngine* formatting_engine_inst = Log->formattingEngineReferenceFromExtension(extension);
        if (!formatting_engine_inst) {
            // We assign a default formatting engine:
            abstractLoggerEngineData->formatting_engine = Log->formattingEngineReference(qti_def_FORMATTING_ENGINE_DEFAULT);
            LOG_INFO(QString(tr("Assigning default formatting engine to file logger engine (%1).").arg(objectName())));
        } else {
            abstractLoggerEngineData->formatting_engine = formatting_engine_inst;
        }
    }

    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOG_ERROR(QString(tr("Failed to initialize file logger engine (%1): Can't open the specified file (%2) for writing...")).arg(objectName()).arg(file_name));
        return false;
    }

    QTextStream out(&file);
    out << abstractLoggerEngineData->formatting_engine->initializeString() << "\n";
    file.close();

    abstractLoggerEngineData->is_initialized = true;
    return true;
}

void Qtilities::Logging::FileLoggerEngine::finalize() {
    if (abstractLoggerEngineData->is_initialized) {
        QFile file(file_name);
        if (!file.open(QIODevice::Append | QIODevice::Text))
            return;

        QTextStream out(&file);
        if (abstractLoggerEngineData->formatting_engine)
            out << abstractLoggerEngineData->formatting_engine->finalizeString();
        out << "\n";
        file.close();
    }
}

QString Qtilities::Logging::FileLoggerEngine::description() const {
    return QObject::tr("Writes log messages to a file.");
}

QString Qtilities::Logging::FileLoggerEngine::status() const {
    if (abstractLoggerEngineData->is_initialized) {
        if (abstractLoggerEngineData->is_enabled)
            return QString(QObject::tr("Logging in progress to output file: %1")).arg(file_name);
        else
            return QObject::tr("Ready but inactive.");
    } else {
        return QObject::tr("Not initialized.");
    }
}

void Qtilities::Logging::FileLoggerEngine::clearLog() {
    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << tr("Failed to clear file logger engine:") << file_name;
        return;
    }
    file.close();
}

void Qtilities::Logging::FileLoggerEngine::logMessage(const QString& message, Logger::MessageType message_type) {
    Q_UNUSED(message_type)

    if (!abstractLoggerEngineData->is_initialized)
        return;

    QFile file(file_name);
    if (!file.open(QIODevice::Append | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << message << "\n";
    file.close();
}


Qtilities::Logging::Interfaces::ILoggerExportable::ExportModeFlags Qtilities::Logging::FileLoggerEngine::supportedFormats() const {
    ILoggerExportable::ExportModeFlags flags = 0;
    flags |= ILoggerExportable::Binary;
    return flags;
}

bool Qtilities::Logging::FileLoggerEngine::exportBinary(QDataStream& stream) const {
    stream << file_name;
    return true;
}

bool Qtilities::Logging::FileLoggerEngine::importBinary(QDataStream& stream) {
    stream >> file_name;
    return true;
}

void Qtilities::Logging::FileLoggerEngine::setFileName(const QString& fileName) {
    if (!abstractLoggerEngineData->is_initialized)
        file_name = fileName;
}

QString Qtilities::Logging::FileLoggerEngine::getFileName() {
    return file_name;
}

// ------------------------------------
// QtMsgLoggerEngine implementation
// ------------------------------------

Qtilities::Logging::QtMsgLoggerEngine* Qtilities::Logging::QtMsgLoggerEngine::m_Instance = 0;

Qtilities::Logging::QtMsgLoggerEngine* Qtilities::Logging::QtMsgLoggerEngine::instance()
{
    static QMutex mutex;
    if (!m_Instance)
    {
      mutex.lock();

      if (!m_Instance)
        m_Instance = new QtMsgLoggerEngine;

      mutex.unlock();
    }

    return m_Instance;
}

Qtilities::Logging::QtMsgLoggerEngine::QtMsgLoggerEngine() : AbstractLoggerEngine()
{
    setName(QObject::tr("Qt Message Logger Engine"));
}

Qtilities::Logging::QtMsgLoggerEngine::~QtMsgLoggerEngine()
{
}

bool Qtilities::Logging::QtMsgLoggerEngine::initialize() {
    abstractLoggerEngineData->is_initialized = true;
    return true;
}

void Qtilities::Logging::QtMsgLoggerEngine::finalize() {
    abstractLoggerEngineData->is_initialized = false;
}

QString Qtilities::Logging::QtMsgLoggerEngine::description() const {
    return QObject::tr("Writes log messages to the Qt message system.");
}

QString Qtilities::Logging::QtMsgLoggerEngine::status() const {
    if (abstractLoggerEngineData->is_initialized) {
        if (abstractLoggerEngineData->is_enabled)
            return QObject::tr("Logging in progress...");
        else
            return QObject::tr("Ready but inactive.");
    } else {
        return QObject::tr("Not initialized.");
    }
}

void Qtilities::Logging::QtMsgLoggerEngine::logMessage(const QString& message, Logger::MessageType message_type) {
    Q_UNUSED(message_type)
    qDebug() << message;
}

// ------------------------------------
// ConsoleLoggerEngine implementation
// ------------------------------------

Qtilities::Logging::ConsoleLoggerEngine* Qtilities::Logging::ConsoleLoggerEngine::m_Instance = 0;

Qtilities::Logging::ConsoleLoggerEngine* Qtilities::Logging::ConsoleLoggerEngine::instance()
{
    static QMutex mutex;
    if (!m_Instance)
    {
      mutex.lock();

      if (!m_Instance)
        m_Instance = new ConsoleLoggerEngine;

      mutex.unlock();
    }

    return m_Instance;
}

Qtilities::Logging::ConsoleLoggerEngine::ConsoleLoggerEngine() : AbstractLoggerEngine() {
    setName(QObject::tr("Console Logger Engine"));
}

Qtilities::Logging::ConsoleLoggerEngine::~ConsoleLoggerEngine() {

}

bool Qtilities::Logging::ConsoleLoggerEngine::initialize() {
    abstractLoggerEngineData->is_initialized = true;
    return true;
}

void Qtilities::Logging::ConsoleLoggerEngine::finalize() {
    abstractLoggerEngineData->is_initialized = false;
}

QString Qtilities::Logging::ConsoleLoggerEngine::description() const {
    return QObject::tr("Writes log messages to the console.");
}

QString Qtilities::Logging::ConsoleLoggerEngine::status() const {
    if (abstractLoggerEngineData->is_initialized) {
        if (abstractLoggerEngineData->is_enabled)
            return QObject::tr("Logging in progress...");
        else
            return QObject::tr("Ready but inactive.");
    } else {
        return QObject::tr("Not initialized.");
    }
}

void Qtilities::Logging::ConsoleLoggerEngine::logMessage(const QString& message, Logger::MessageType message_type) {
    Q_UNUSED(message_type)
    fprintf(stdout, "%s\n", qPrintable(message));
}
