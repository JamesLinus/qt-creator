/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#ifndef DEVICEMANAGERMODEL_H
#define DEVICEMANAGERMODEL_H

#include "../projectexplorer_export.h"
#include "idevice.h"

#include <QAbstractListModel>

namespace ProjectExplorer {
namespace Internal { class DeviceManagerModelPrivate; }
class IDevice;
class DeviceManager;

class PROJECTEXPLORER_EXPORT DeviceManagerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit DeviceManagerModel(const DeviceManager *deviceManager, QObject *parent = 0);
    ~DeviceManagerModel();

    void setFilter(const QList<Core::Id> &filter);
    void setTypeFilter(Core::Id type);

    IDevice::ConstPtr device(int pos) const;
    Core::Id deviceId(int pos) const;
    int indexOf(IDevice::ConstPtr dev) const;
    int indexForId(Core::Id id) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    void updateDevice(Core::Id id);

private slots:
    void handleDeviceAdded(Core::Id id);
    void handleDeviceRemoved(Core::Id id);
    void handleDeviceUpdated(Core::Id id);
    void handleDeviceListChanged();

private:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool matchesTypeFilter(const IDevice::ConstPtr &dev) const;

    Internal::DeviceManagerModelPrivate * const d;
};

} // namespace ProjectExplorer

#endif // DEVICEMANAGERMODEL_H