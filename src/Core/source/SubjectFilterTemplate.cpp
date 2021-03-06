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

#include "SubjectFilterTemplate.h"
#include "Observer.h"


namespace Qtilities {
    namespace Core {
        FactoryItem<AbstractSubjectFilter, SubjectFilterTemplate> SubjectFilterTemplate::factory;
    }
}

struct Qtilities::Core::SubjectFilterTemplatePrivateData {
    SubjectFilterTemplatePrivateData() : is_modified(false) {}

    bool is_modified;
};

Qtilities::Core::SubjectFilterTemplate::SubjectFilterTemplate(QObject* parent) : AbstractSubjectFilter(parent) {
    d = new SubjectFilterTemplatePrivateData;
}

Qtilities::Core::SubjectFilterTemplate::~SubjectFilterTemplate() {
    delete d;
}

Qtilities::Core::AbstractSubjectFilter::EvaluationResult Qtilities::Core::SubjectFilterTemplate::evaluateAttachment(QObject* obj, QString* rejectMsg, bool silent) const {
    Q_UNUSED(obj)
    Q_UNUSED(rejectMsg)
    Q_UNUSED(silent)

    return AbstractSubjectFilter::Allowed;
}

bool Qtilities::Core::SubjectFilterTemplate::initializeAttachment(QObject* obj, QString* rejectMsg, bool import_cycle) {
    Q_UNUSED(obj)
    Q_UNUSED(import_cycle)

    #ifndef QT_NO_DEBUG
    Q_ASSERT(observer != 0);
    #endif
    #ifdef QT_NO_DEBUG
    if (!obj) {
        if (rejectMsg)
            *rejectMsg = QString(tr("Subject Filter Template: Invalid object reference received. Attachment cannot be done.."));
        return false;
    }
    #endif

    if (!observer) {
        if (rejectMsg)
            *rejectMsg = QString(tr("Cannot evaluate an attachment in a subject filter without an observer context."));
        LOG_TRACE("Cannot evaluate an attachment in a subject filter without an observer context.");
        return false;
    }

    return true;
}

void Qtilities::Core::SubjectFilterTemplate::finalizeAttachment(QObject* obj, bool attachment_successful, bool import_cycle) {
    Q_UNUSED(obj)
    Q_UNUSED(attachment_successful)
    Q_UNUSED(import_cycle)
}

Qtilities::Core::AbstractSubjectFilter::EvaluationResult Qtilities::Core::SubjectFilterTemplate::evaluateDetachment(QObject* obj, QString* rejectMsg) const {
    Q_UNUSED(obj)
    Q_UNUSED(rejectMsg)

    return AbstractSubjectFilter::Allowed;
}

bool Qtilities::Core::SubjectFilterTemplate::initializeDetachment(QObject* obj, QString* rejectMsg, bool subject_deleted) {
    Q_UNUSED(obj)
    Q_UNUSED(rejectMsg)
    Q_UNUSED(subject_deleted)

    return true;
}

void Qtilities::Core::SubjectFilterTemplate::finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted) {
    Q_UNUSED(obj)
    Q_UNUSED(detachment_successful)
    Q_UNUSED(subject_deleted)
}

QStringList Qtilities::Core::SubjectFilterTemplate::monitoredProperties() const {
    return QStringList();
}

QStringList Qtilities::Core::SubjectFilterTemplate::reservedProperties() const {
    return QStringList();
}

bool Qtilities::Core::SubjectFilterTemplate::handleMonitoredPropertyChange(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent) {
    Q_UNUSED(obj)
    Q_UNUSED(property_name)
    Q_UNUSED(propertyChangeEvent)

    return false;
}

Qtilities::Core::InstanceFactoryInfo Qtilities::Core::SubjectFilterTemplate::instanceFactoryInfo() const {
    InstanceFactoryInfo instanceFactoryInfo(qti_def_FACTORY_QTILITIES,"Subject Filter Template","Subject Filter Template");
    return instanceFactoryInfo;
}

Qtilities::Core::Interfaces::IExportable::ExportModeFlags Qtilities::Core::SubjectFilterTemplate::supportedFormats() const {
    IExportable::ExportModeFlags flags = 0;
    flags |= IExportable::Binary;
    flags |= IExportable::XML;
    return flags;
}

bool Qtilities::Core::SubjectFilterTemplate::isModified() const {
    return d->is_modified;
}

void Qtilities::Core::SubjectFilterTemplate::setModificationState(bool new_state, IModificationNotifier::NotificationTargets notification_targets, bool force_notifications) {
    Q_UNUSED(force_notifications)

    d->is_modified = new_state;
    if (notification_targets & IModificationNotifier::NotifyListeners) {
        emit modificationStateChanged(new_state);
    }
}

QDataStream & operator<< (QDataStream& stream, const Qtilities::Core::SubjectFilterTemplate& stream_obj) {
    stream_obj.exportBinary(stream);
    return stream;
}

QDataStream & operator>> (QDataStream& stream, Qtilities::Core::SubjectFilterTemplate& stream_obj) {
    QList<QPointer<QObject> > import_list;
    stream_obj.importBinary(stream,import_list);
    return stream;
}


