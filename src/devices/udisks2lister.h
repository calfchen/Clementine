#pragma once

#include <memory>

#include "devicelister.h"

#include "dbus/objectmanager.h"

class Udisks2Lister : public DeviceLister {
  Q_OBJECT

public:
  Udisks2Lister();
  ~Udisks2Lister();

  QStringList DeviceUniqueIDs();
  QVariantList DeviceIcons(const QString &id);
  QString DeviceManufacturer(const QString &id);
  QString DeviceModel(const QString &id);
  quint64 DeviceCapacity(const QString &id);
  quint64 DeviceFreeSpace(const QString &id);
  QVariantMap DeviceHardwareInfo(const QString &id);

  QString MakeFriendlyName(const QString &id);
  QList<QUrl> MakeDeviceUrls(const QString &id);

  void UnmountDevice(const QString &id);

public slots:
  void UpdateDeviceFreeSpace(const QString &id);

protected:
  void Init();

private slots:
  void DBusInterfaceAdded(const QDBusObjectPath &path, const InterfacesAndProperties &ifaces);
  void DBusInterfaceRemoved(const QDBusObjectPath &path, const QStringList &ifaces);

private:
  bool isPendingJob(const QDBusObjectPath &path);
  void RemoveDevice(const QDBusObjectPath &path);

  QMutex jobs_lock_;
  QMap<QString, QList<QDBusObjectPath>> mounting_jobs_;

private:
  class PartitionData {
  public:
    QString unique_id() const;

    QString dbus_path;
    QString friendly_name;

    // Device
    QString serial;
    QString vendor;
    QString model;
    quint64 capacity = 0;
    QString dbus_drive_path;

    // Paritition
    QString label;
    QString uuid;
    quint64 free_space = 0;
    QStringList mount_paths;
  };

  PartitionData ReadPartitionData(const QDBusObjectPath &path, bool beingMounted);

  QReadWriteLock device_data_lock_;
  QMap<QString, PartitionData> device_data_;

private:
  std::unique_ptr<OrgFreedesktopDBusObjectManagerInterface> udisks2_interface_;

  static const QString udisks2service_;
};
