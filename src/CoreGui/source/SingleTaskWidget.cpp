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

#include "SingleTaskWidget.h"
#include "ui_SingleTaskWidget.h"
#include "QtilitiesCoreGuiConstants.h"
#include "WidgetLoggerEngine.h"
#include "TaskManagerGui.h"

#include <TaskManager>
#include <QtilitiesCoreApplication>

#include <QPointer>
#include <QMessageBox>

using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Core::Interfaces;

struct Qtilities::CoreGui::SingleTaskWidgetPrivateData {
    SingleTaskWidgetPrivateData() : task(0),
        pause_button_visible(false),
        stop_button_visible(false),
        start_button_visible(false),
        show_log_button_visible(true) {}

    ITask*              task;
    QPointer<QObject>   task_base;
    bool                pause_button_visible;
    bool                stop_button_visible;
    bool                start_button_visible;
    bool                show_log_button_visible;
};

Qtilities::CoreGui::SingleTaskWidget::SingleTaskWidget(int task_id, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SingleTaskWidget),
    d(0)
{
    ui->setupUi(this);
    d = new SingleTaskWidgetPrivateData;
    ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_NOT_STARTED_22x22));
    ui->btnShowLog->setToolTip(tr("Task Has Not Been Started."));

    ui->progressBar->setValue(0);
    d->task = TASK_MANAGER->hasTask(task_id);
    if (!d->task) {
        qDebug() << Q_FUNC_INFO << "Showing SingleTaskWidget for a task which does not exist with ID:" << task_id;
        return;
    }

    // Pause Button
    setPauseButtonVisible(d->task->canPause());

    // Stop Button
    d->stop_button_visible = d->task->canStop();
    ui->btnStop->setVisible(d->stop_button_visible);
    ui->btnStop->setIcon(QIcon(qti_icon_TASK_STOP_22x22));
    ui->btnStop->setToolTip("Cancel Task");

    // Start Button
    d->start_button_visible = d->task->canStart();
    ui->btnStart->setVisible(d->start_button_visible);
    ui->btnStart->setIcon(QIcon(qti_icon_TASK_START_22x22));
    ui->btnStart->setToolTip(tr("Task Has Not Been Started. Click to start it."));

    // Hide Stop and Pause buttons widget if those buttons are not shown.
    if (!d->task->canStop() && !d->task->canPause()) {
        ui->widgetRightButtonsHolder->setVisible(false);
    } else {
        ui->widgetRightButtonsHolder->setVisible(true);
    }

    // Log Button
    ui->btnShowLog->setVisible(d->task->loggingEnabled());

    if (d->task) {
        d->task_base = d->task->objectBase();
        connect(d->task->objectBase(),SIGNAL(destroyed()),SLOT(handleTaskDeleted()));
        connect(d->task->objectBase(),SIGNAL(taskStarted(int,QString,Logger::MessageType)),SLOT(update()));
        connect(d->task->objectBase(),SIGNAL(subTaskCompleted(int,QString,Logger::MessageType)),SLOT(update()));
        connect(d->task->objectBase(),SIGNAL(taskCompleted(ITask::TaskResult,QString,Logger::MessageType)),SLOT(update()));
        connect(d->task->objectBase(),SIGNAL(taskPaused()),SLOT(update()));
        connect(d->task->objectBase(),SIGNAL(taskResumed()),SLOT(update()));
        connect(d->task->objectBase(),SIGNAL(taskStopped()),SLOT(update()));
        connect(d->task->objectBase(),SIGNAL(busyStateChanged(ITask::TaskBusyState,ITask::TaskBusyState)),SLOT(updateBusyState(ITask::TaskBusyState)));
        connect(d->task->objectBase(),SIGNAL(displayedNameChanged(QString)),SLOT(setDisplayedName(QString)));
        connect(d->task->objectBase(),SIGNAL(canStartChanged(bool)),SLOT(setStartButtonEnabled(bool)));
        connect(d->task->objectBase(),SIGNAL(canStopChanged(bool)),SLOT(setStopButtonEnabled(bool)));
        connect(d->task->objectBase(),SIGNAL(canPauseChanged(bool)),SLOT(setPauseButtonEnabled(bool)));
        connect(d->task->objectBase(),SIGNAL(canStartChanged(bool)),SLOT(setStartButtonVisible(bool)));
        connect(d->task->objectBase(),SIGNAL(canStopChanged(bool)),SLOT(setStopButtonVisible(bool)));
        connect(d->task->objectBase(),SIGNAL(canPauseChanged(bool)),SLOT(setPauseButtonVisible(bool)));
        update();
    }
}

Qtilities::CoreGui::SingleTaskWidget::~SingleTaskWidget() {
    delete ui;
    delete d;
}

ITask* Qtilities::CoreGui::SingleTaskWidget::task() const {
    return d->task;
}

void Qtilities::CoreGui::SingleTaskWidget::setPauseButtonVisible(bool is_visible) {
    d->pause_button_visible = is_visible;

    ui->btnPause->setVisible(is_visible);
    if (is_visible) {
        ui->widgetRightButtonsHolder->setVisible(is_visible);
        ui->btnPause->setIcon(QIcon(qti_icon_TASK_PAUSE_22x22));
        ui->btnPause->setToolTip("Pause Task");
    } else {
        ui->btnPause->setIcon(QIcon());
        if (!d->task->canStop())
            ui->widgetRightButtonsHolder->setVisible(false);
    }
}

void Qtilities::CoreGui::SingleTaskWidget::setPauseButtonEnabled(bool enabled) {
    ui->btnPause->setEnabled(enabled);
}

bool Qtilities::CoreGui::SingleTaskWidget::pauseButtonVisible() const {
    return d->pause_button_visible;
}

void Qtilities::CoreGui::SingleTaskWidget::setStopButtonVisible(bool is_visible) {
    d->stop_button_visible = is_visible;
            ui->btnStop->setVisible(is_visible);
    if (is_visible) {
        ui->widgetRightButtonsHolder->setVisible(is_visible);
    } else {
        if (!d->task->canPause())
            ui->widgetRightButtonsHolder->setVisible(false);
    }
}

void Qtilities::CoreGui::SingleTaskWidget::setStopButtonEnabled(bool enabled) {
    ui->btnStop->setEnabled(enabled);
    if (enabled)
        ui->btnStop->setToolTip("Cancel Task");
    else
        ui->btnStop->setToolTip(tr("This Task Can't Be Stopped Manually Right Now"));
}

bool Qtilities::CoreGui::SingleTaskWidget::stopButtonVisible() const {
    return d->stop_button_visible;
}

void Qtilities::CoreGui::SingleTaskWidget::setStartButtonVisible(bool is_visible) {
    d->start_button_visible = is_visible;
    if (is_visible) {
        ui->widgetLeftButtonsHolder->setVisible(is_visible);
        ui->btnStart->setVisible(is_visible);
    }
}

void Qtilities::CoreGui::SingleTaskWidget::setStartButtonEnabled(bool enabled) {
    ui->btnStart->setEnabled(enabled);
}

bool Qtilities::CoreGui::SingleTaskWidget::startButtonVisible() const {
    return d->start_button_visible;
}

void Qtilities::CoreGui::SingleTaskWidget::setShowLogButtonVisible(bool is_visible) {
    d->show_log_button_visible = is_visible;
    ui->btnShowLog->setVisible(is_visible);
}

bool Qtilities::CoreGui::SingleTaskWidget::showLogButtonVisible() const {
    return d->show_log_button_visible;
}

QProgressBar* Qtilities::CoreGui::SingleTaskWidget::progressBar() {
    return ui->progressBar;
}

void Qtilities::CoreGui::SingleTaskWidget::on_btnShowLog_clicked() {
    if (!d->task)
        return;

    // Check if the task has a log widget:
    if (d->task->loggerEngine()) {
        WidgetLoggerEngine* engine_widget = qobject_cast<WidgetLoggerEngine*> (d->task->loggerEngine());
        if (engine_widget) {
            QWidget* widget = engine_widget->getWidget();
            widget->setAttribute(Qt::WA_QuitOnClose,false);
            widget->resize(QSize(1000,600));
            widget->show();
            widget->activateWindow();
            return;
        }
    }

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(tr("Task Log: ") + d->task->taskName());
    msgBox.setText(tr("The activities of this task are not logged."));
    msgBox.exec();
}

void Qtilities::CoreGui::SingleTaskWidget::setDisplayedName(const QString& name) {
    ui->lblTaskName->clear();
    QFontMetrics fm(ui->lblTaskName->font());
    QString elided_text = fm.elidedText(name,Qt::ElideRight,ui->lblTaskName->width());
    ui->lblTaskName->setText(elided_text);
    ui->lblTaskName->setToolTip(name);
}

void Qtilities::CoreGui::SingleTaskWidget::on_btnPause_clicked() {
    if (!d->task)
        return;

    if (d->task->state() == ITask::TaskBusy)
        d->task->pause();
    else if (d->task->state() == ITask::TaskPaused)
        d->task->resume();
}

void Qtilities::CoreGui::SingleTaskWidget::on_btnStop_clicked() {
    if (!d->task)
        return;

    if (d->task->state() == ITask::TaskBusy) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        d->task->stop();
        QApplication::restoreOverrideCursor();
        if (d->task_base) {
            if (d->task->taskStopAction() == ITask::TaskDeleteWhenStopped)
                d->task->objectBase()->deleteLater();
            else if (d->task->taskStopAction() == ITask::TaskHideWhenStopped)
                setVisible(false);

            // ITask::TaskDoNothingWhenStopped does not need to do anything.
        }
    } else {
        if (d->task->taskRemoveAction() == ITask::TaskDeleteWhenRemoved)
            d->task->objectBase()->deleteLater();
        else if (d->task->taskRemoveAction() == ITask::TaskHideWhenRemoved)
            setVisible(false);
    }
}

void Qtilities::CoreGui::SingleTaskWidget::handleTaskDeleted() {
    //qDebug() << "Task deleted, deleting its SingleTaskWidget";
    deleteLater();
}

void Qtilities::CoreGui::SingleTaskWidget::on_btnStart_clicked() {
    if (!d->task)
        return;

    d->task->start();
}

void Qtilities::CoreGui::SingleTaskWidget::updateBusyState(ITask::TaskBusyState busy_state) {
    if (!d->task)
        return;

    if (d->task->state() == ITask::TaskBusy) {
        if (busy_state == ITask::TaskBusyClean) {
            ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_BUSY_22x22));
            ui->btnShowLog->setToolTip(tr("Task Busy. Click to view the task log"));
        } else if (busy_state == ITask::TaskBusyWithWarnings) {
            ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_BUSY_WITH_WARNINGS_22x22));
            ui->btnShowLog->setToolTip(tr("Task Busy (With Warnings). Click to view the task log"));
        } else if (busy_state == ITask::TaskBusyWithErrors) {
            ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_BUSY_WITH_ERRORS_22x22));
            ui->btnShowLog->setToolTip(tr("Task Busy (With Errors). Click to view the task log"));
        }
    }
}

void Qtilities::CoreGui::SingleTaskWidget::resizeEvent(QResizeEvent * event) {
    if (d) {
        if (d->task && d->task_base)
            setDisplayedName(d->task->displayName());
    }

    QWidget::resizeEvent(event);
}

void Qtilities::CoreGui::SingleTaskWidget::update() {  
    if (!d->task)
        return;

    bool show_progress = TaskManagerGui::instance()->taskProgressUpdatingEnabled();

    setDisplayedName(d->task->displayName());

    if (d->task->state() == ITask::TaskBusy) {
        if (show_progress) {
            ui->progressBar->setEnabled(true);
            if (d->task->numberOfSubTasks() == -1) {
                ui->progressBar->setMinimum(0);
                ui->progressBar->setMaximum(0);
                ui->progressBar->setValue(0);
            } else {
                ui->progressBar->setMinimum(0);
                //qDebug() << "Current progress on task " << d->task->taskName() << " is " << d->task->currentProgress() << "/" << d->task->numberOfSubTasks();
                ui->progressBar->setValue(d->task->currentProgress());
                ui->progressBar->setMaximum(d->task->numberOfSubTasks());
            }
        } else {
            ui->progressBar->setEnabled(false);
        }

        ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_BUSY_22x22));
        ui->btnShowLog->setToolTip(tr("Task Busy. Click to view the task log"));
        ui->btnShowLog->setVisible(d->show_log_button_visible);

        setPauseButtonVisible(d->task->canPause());

        if (d->task->canStop()) {
            ui->btnStop->setEnabled(true);
            ui->btnStop->setToolTip("Cancel Task");
        } else {
            ui->btnStop->setEnabled(false);
            ui->btnStop->setToolTip(tr("This Task Can't Be Stopped Manually Right Now"));
        }

        ui->btnStart->setVisible(false);
    } else if (d->task->state() == ITask::TaskNotStarted) {
        if (show_progress) {
            ui->progressBar->setEnabled(true);
            ui->progressBar->setMinimum(0);
            ui->progressBar->setMaximum(100);
        } else {
            ui->progressBar->setEnabled(false);
        }

        ui->btnStop->setEnabled(false);
        ui->btnStop->setToolTip("Cancel Task");
        ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_NOT_STARTED_22x22));
        ui->btnShowLog->setToolTip(tr("Task Has Not Been Started. Click to view the task log"));

        ui->btnStart->setToolTip("Task Not Started. Click to start it again");
        ui->btnStart->setVisible(d->task->canStart());
        ui->btnShowLog->setVisible(!d->task->canStart() && d->show_log_button_visible);

        setPauseButtonVisible(d->task->canPause());
        ui->btnPause->setEnabled(false);
    } else if (d->task->state() == ITask::TaskPaused) {
        ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_BUSY_22x22));
        ui->btnShowLog->setToolTip(tr("Task Has Been Paused. Click to view the task log"));
        if (d->task->canPause()) {
            ui->btnPause->setEnabled(true);
            ui->btnPause->setIcon(QIcon(qti_icon_TASK_RESUME_22x22));
            ui->btnPause->setToolTip(tr("Resume Task"));
        }
        if (d->task->canStop())
            ui->btnStop->setEnabled(true);

        ui->btnStart->setVisible(false);
    } else if (d->task->state() == ITask::TaskStopped) {
        ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_NOT_STARTED_22x22));
        ui->btnShowLog->setToolTip(tr("Task Has Been Stopped. Click to view the task log"));

        setPauseButtonVisible(d->task->canPause());
        ui->btnPause->setEnabled(false);

        ui->btnStop->setEnabled(true);
        ui->btnStop->setToolTip("Remove Task");
        ui->btnStart->setVisible(d->task->canStart());
    } else if (d->task->state() == ITask::TaskCompleted) {
        if (show_progress) {
            ui->progressBar->setEnabled(true);
            if (d->task->numberOfSubTasks() == -1) {
                // Handle progress bars acting as busy indicators:
                ui->progressBar->setMinimum(0);
                ui->progressBar->setMaximum(100);
                ui->progressBar->setValue(100);
            } else {
                // Handle progress bars showing progress:
                ui->progressBar->setMinimum(0);
                ui->progressBar->setMaximum(100);
                ui->progressBar->setValue(100);
            }
        } else {
            ui->progressBar->setEnabled(false);
        }

        setPauseButtonVisible(d->task->canPause());
        ui->btnPause->setEnabled(false);

        ui->btnStop->setEnabled(true);
        ui->btnStop->setToolTip("Remove Task");
        ui->btnStart->setVisible(d->task->canStart());
        ui->widgetRightButtonsHolder->setVisible(true);

        if (d->task->result() == ITask::TaskFailed) {
            ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_FAILED_22x22));
            ui->btnShowLog->setToolTip(tr("Task Failed. Click to view the task log"));
        } else if (d->task->result() == ITask::TaskSuccessful) {
            ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_DONE_22x22));
            ui->btnShowLog->setToolTip(tr("Task Finished Successfully. Click to view the task log"));
        } else if (d->task->result() == ITask::TaskSuccessfulWithWarnings) {
            ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_DONE_WITH_WARNINGS_22x22));
            ui->btnShowLog->setToolTip(tr("Task Finished Successfully (With Warnings). Click to view the task log"));
        } else if (d->task->result() == ITask::TaskSuccessfulWithErrors) {
            ui->btnShowLog->setIcon(QIcon(qti_icon_TASK_DONE_WITH_ERRORS_22x22));
            ui->btnShowLog->setToolTip(tr("Task Finished Successfully (With errors and possibly warnings). Click to view the task log"));
        }
    }
}
