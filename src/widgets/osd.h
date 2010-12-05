/* This file is part of Clementine.
   Copyright 2010, David Sansome <me@davidsansome.com>

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OSD_H
#define OSD_H

#include <QDateTime>
#include <QImage>
#include <QObject>

#include "engines/engine_fwd.h"
#include "core/albumcoverloader.h"
#include "core/backgroundthread.h"
#include "core/song.h"

class OrgFreedesktopNotificationsInterface;
class OSDPretty;
class SystemTrayIcon;

class QDBusPendingCallWatcher;

#ifdef Q_WS_X11
# include <QDBusArgument>
# include <boost/scoped_ptr.hpp>

  QDBusArgument& operator<< (QDBusArgument& arg, const QImage& image);
  const QDBusArgument& operator>> (const QDBusArgument& arg, QImage& image);
#endif

class OSD : public QObject {
  Q_OBJECT

 public:
  OSD(SystemTrayIcon* tray_icon, QObject* parent = 0);
  ~OSD();

  static const char* kSettingsGroup;

  enum Behaviour {
    Disabled = 0,
    Native,
    TrayPopup,
    Pretty,
  };

  // Implemented in the OS-specific files
  static bool SupportsNativeNotifications();
  static bool SupportsTrayPopups();

 public slots:
  void ReloadSettings();

  void ForceShowNextNotification() { force_show_next_ = true; }
  void SongChanged(const Song& song);
  void Paused();
  void Stopped();
  void PlaylistFinished();
  void VolumeChanged(int value);
  void MagnatuneDownloadFinished(const QStringList& albums);

#ifdef ENABLE_WIIMOTEDEV
  void WiiremoteActived(int id);
  void WiiremoteDeactived(int id);
  void WiiremoteConnected(int id);
  void WiiremoteDisconnected(int id);
  void WiiremoteLowBattery(int id, int live);
  void WiiremoteCriticalBattery(int id, int live);
#endif

 private:
  struct WaitingForAlbumArt {
    QString summary;
    QString message;
    QString icon;
  };

  void ShowMessage(const QString& summary,
                   const QString& message = QString(),
                   const QString& icon = QString(),
                   const QImage& image = QImage());

  // These are implemented in the OS-specific files
  void Init();
  void ShowMessageNative(const QString& summary,
                         const QString& message,
                         const QString& icon = QString(),
                         const QImage& image = QImage());

 private slots:
  void CallFinished(QDBusPendingCallWatcher* watcher);
  void CoverLoaderInitialised();
  void CoverArtPathReady(const Song& song, const QString& image_path);
  void AlbumArtLoaded(quint64 id, const QImage& image);
  void AlbumArtLoaded(const WaitingForAlbumArt info, const QImage& image);

 private:
  SystemTrayIcon* tray_icon_;
  int timeout_msec_;
  Behaviour behaviour_;
  bool show_on_volume_change_;
  bool show_art_;

  bool force_show_next_;
  bool ignore_next_stopped_;

  OSDPretty* pretty_popup_;

  BackgroundThread<AlbumCoverLoader>* cover_loader_;
  QMap<quint64, WaitingForAlbumArt> waiting_for_album_art_;

#ifdef Q_OS_DARWIN
  class GrowlNotificationWrapper;
  GrowlNotificationWrapper* wrapper_;
#endif  // Q_OS_DARWIN

#ifdef Q_WS_X11
  boost::scoped_ptr<OrgFreedesktopNotificationsInterface> interface_;
  uint notification_id_;
  QDateTime last_notification_time_;
#endif
};

#endif // OSD_H
