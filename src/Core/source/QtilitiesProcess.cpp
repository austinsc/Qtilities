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

#include "QtilitiesProcess.h"

#include <QCoreApplication>
#include <FileUtils>

#include <Logger>

using namespace Qtilities::Logging;

struct Qtilities::Core::QtilitiesProcessPrivateData {
    QtilitiesProcessPrivateData() : process(0) { }

    QProcess* process;
    QString buffer_std_out;
    QString buffer_std_error;
    QStringList line_break_strings;
};

Qtilities::Core::QtilitiesProcess::QtilitiesProcess(const QString& task_name, bool enable_logging, bool read_process_buffers, QObject* parent) : Task(task_name,enable_logging,parent) {
    d = new QtilitiesProcessPrivateData;
    d->process = new QProcess;

    connect(d->process, SIGNAL(started()), this, SLOT(procStarted()));
    connect(this, SIGNAL(stopTaskRequest()), this, SLOT(stopProcess()));

    connect(d->process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(procError(QProcess::ProcessError)));
    connect(d->process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(procFinished(int, QProcess::ExitStatus)));
    connect(d->process,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(procStateChanged(QProcess::ProcessState)));

    if (read_process_buffers) {
        connect(d->process,SIGNAL(readyReadStandardOutput()), this, SLOT(logProgressOutput()));
        connect(d->process,SIGNAL(readyReadStandardError()), this, SLOT(logProgressError()));
    }

    setCanStop(true);
}

Qtilities::Core::QtilitiesProcess::~QtilitiesProcess() {
    if (d->process) {
        if (state() == ITask::TaskBusy)
            completeTask();
        d->process->kill();
        delete d->process;
    }
    delete d;
}

QProcess* Qtilities::Core::QtilitiesProcess::process() {
    return d->process;
}

void Qtilities::Core::QtilitiesProcess::setLineBreakStrings(const QStringList &line_break_strings) {
    d->line_break_strings = line_break_strings;
}

QStringList Qtilities::Core::QtilitiesProcess::lineBreakStrings() {
    return d->line_break_strings;
}

bool Qtilities::Core::QtilitiesProcess::startProcess(const QString& program, const QStringList& arguments, QProcess::OpenMode mode) {
    if (state() == ITask::TaskPaused)
        return false;

    if (state() != ITask::TaskBusy)
        startTask();

    // Check if program exists:
    QFileInfo fi1(program);
    if (!fi1.exists() && fi1.isAbsolute()) {
        // Check if program.exe exists:
        QFileInfo fi2(program + ".exe");
        if (!fi2.exists() && fi2.isAbsolute()) {
            // Check if program.bat exists:
            QFileInfo fi3(program + ".bat");
            if (!fi3.exists() && fi3.isAbsolute())
                logWarning("Failed to find application \"" + program + "\". An attempt to launch it will still be made.");
        }
    }

    QString native_program = FileUtils::toNativeSeparators(program);
    logMessage("Executing Process: " + native_program + " " + arguments.join(" "));
    if (d->process->workingDirectory().isEmpty())
        logMessage("> working directory of process: " + QDir::current().path());
    else
        logMessage("> working directory of process: " + d->process->workingDirectory());
    QDir dir(d->process->workingDirectory());
    if (!dir.exists())
        logWarning("> working directory does not exist, process might fail to start.");

    logMessage("");
    d->process->start(native_program, arguments, mode);

    if (!d->process->waitForStarted()) {
        logMessage("Failed to start " + native_program + ". Make sure the executable is visible in your system's paths.", Logger::Error);
        completeTask(ITask::TaskFailed);

        d->process->waitForFinished();
        return false;
    }

    return true;
}

void Qtilities::Core::QtilitiesProcess::stopProcess() {
    d->process->terminate();
    d->process->waitForFinished(3000);
    d->process->kill();
    d->process->waitForFinished(3000);

    if (state() == ITask::TaskBusy)
        completeTask();
}

void Qtilities::Core::QtilitiesProcess::procStarted() {

}

void Qtilities::Core::QtilitiesProcess::procFinished(int exit_code, QProcess::ExitStatus exit_status) {
    // Note that we log some empty messages here and when the process was started in order to
    // seperate the process's output and the task messages.
    if (exit_code == 0) {
        //logMessage("Process " + taskName() + " exited normal with code 0.");
    } else {
        if (exit_status == QProcess::NormalExit) {
            logMessage("");
            logMessage("Process " + taskName() + " exited normal with code " + QString::number(exit_code),Logger::Error);
        } else if (exit_status == QProcess::CrashExit) {
            logMessage("");
            logMessage("Process " + taskName() + " crashed with code " + QString::number(exit_code),Logger::Error);
        }
    }

//    bool current_active = true;
//    if (loggerEngine()) {
//        current_active = loggerEngine()->isActive();
//        loggerEngine()->setActive(false);
//    }
    completeTask();
//    if (loggerEngine())
//        loggerEngine()->setActive(current_active);
    Q_UNUSED(exit_status)
}

void Qtilities::Core::QtilitiesProcess::procError(QProcess::ProcessError error) {
    switch (error)
    {
        case QProcess::FailedToStart:
            logMessage("Process " + taskName() + " failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.",Logger::Error);
            break;
        case QProcess::Crashed:
            logMessage("Process " + taskName() + " crashed some time after starting successfully.",Logger::Error);
            break;
        case QProcess::Timedout:
            logMessage("The last waitFor...() function of process " + taskName() + " timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.",Logger::Error);
            break;
        case QProcess::WriteError:
            logMessage("An error occurred when attempting to write to process " + taskName() + ". For example, the process may not be running, or it may have closed its input channel.",Logger::Error);
            break;
        case QProcess::ReadError:
            logMessage("An error occurred when attempting to read from process " + taskName() + ". For example, the process may not be running.",Logger::Error);
            break;
        case QProcess::UnknownError:
            logMessage("Process " + taskName() + " failed with an unknown error.",Logger::Error);
            break;
        default:
            logMessage("Process " + taskName() + " failed with an unknown error.",Logger::Error);
    }
}

void Qtilities::Core::QtilitiesProcess::procStateChanged(QProcess::ProcessState newState) {
    if (newState == QProcess::NotRunning && (state() == ITask::TaskBusy || state() == ITask::TaskPaused)) {
        completeTask();
    }
}

void Qtilities::Core::QtilitiesProcess::logProgressOutput() {
    QString new_output = d->process->readAllStandardOutput();
    emit newStandardOutputMessage(new_output);
    d->buffer_std_out.append(new_output);

    QStringList split_list;
    if (d->line_break_strings.isEmpty()) {
        // We search for \r and split messages up:
        split_list = d->buffer_std_out.split("\r",QString::SkipEmptyParts);
        while (split_list.count() > 1) {
            if (split_list.front().trimmed().startsWith("WARNING",Qt::CaseInsensitive))
                logWarning(split_list.front());
            else if (split_list.front().trimmed().startsWith("ERROR",Qt::CaseInsensitive))
                logError(split_list.front());
            else
                logMessage(split_list.front());
            split_list.pop_front();
        }
    } else {
        // We loop through the string and replace all known break strings with &{_BREAKSTRING and then split
        // it using &{_:
        foreach (QString break_string, d->line_break_strings)
            d->buffer_std_out.replace(break_string,"&{_" + break_string);

        split_list = d->buffer_std_out.split("&{_",QString::SkipEmptyParts);
        while (split_list.count() > 1) {
            QString msg = split_list.front();
            if (msg.startsWith("&{_"))
                msg = msg.remove(0,3);

            if (msg.startsWith("WARNING",Qt::CaseInsensitive))
                logWarning(msg);
            else if (msg.startsWith("ERROR",Qt::CaseInsensitive))
                logError(msg);
            else
                logMessage(msg);
            split_list.pop_front();
        }
    }

    if (split_list.isEmpty())
        d->buffer_std_out.clear();
    else
        d->buffer_std_out = split_list.front();
}

void Qtilities::Core::QtilitiesProcess::logProgressError() {   
    QString new_output = d->process->readAllStandardError();
    emit newStandardErrorMessage(new_output);
    d->buffer_std_error.append(new_output);

    QStringList split_list;
    if (d->line_break_strings.isEmpty()) {
        // We search for \r and split messages up:
        split_list = d->buffer_std_error.split("\r",QString::SkipEmptyParts);
        while (split_list.count() > 1) {
            if (split_list.front().trimmed().startsWith("WARNING:",Qt::CaseInsensitive))
                logWarning(split_list.front());
            else if (split_list.front().trimmed().startsWith("INFO:",Qt::CaseInsensitive))
                logMessage(split_list.front());
            else
                logError(split_list.front());
            split_list.pop_front();
        }   
    } else { 
        // We loop through the string and replace all known break strings with &{_BREAKSTRING and then split
        // it using &{_:
        foreach (QString break_string, d->line_break_strings)
            d->buffer_std_error.replace(break_string,"&{_" + break_string);

        split_list = d->buffer_std_error.split("&{_",QString::SkipEmptyParts);
        while (split_list.count() > 1) {
            QString msg = split_list.front();
            if (msg.startsWith("&{_"))
                msg = msg.remove(0,3);

            if (msg.startsWith("WARNING:",Qt::CaseInsensitive))
                logWarning(msg);
            else if (msg.startsWith("INFO:",Qt::CaseInsensitive))
                logMessage(msg);
            else
                logError(msg);
            split_list.pop_front();
        }
    }

    if (split_list.isEmpty())
        d->buffer_std_error.clear();
    else
        d->buffer_std_error = split_list.front();
}
