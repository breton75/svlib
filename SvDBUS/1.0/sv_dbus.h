﻿#ifndef SV_DBUS_H
#define SV_DBUS_H

#include <QObject>
#include <QtDBus>
#include <QDebug>

#include "../../SvAbstractLogger/1.2/sv_abstract_logger.h"

#define DBUS_SERVER_NAME "proj.modus"

/*
 * Proxy class for interface org.niirpi.modus

    скопировано из modus_dbus_interface.h,
    который генерируется из файла xml
*/
class DBusModusInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return DBUS_SERVER_NAME; }

public:
    DBusModusInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0):
      QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
    { }

    ~DBusModusInterface() { }

public Q_SLOTS: // METHODS
Q_SIGNALS: // SIGNALS
    void action(const QString &nickname, const QString &text);
    void message(const QString &sender, const QString &text, const QString &type);
};

namespace proj {
  namespace modus {
    typedef ::DBusModusInterface modus;
  }
}

/*
 * Adaptor class for interface org.niirpi.ModusDBus

    скопировано из modus_dbus_adaptor.h,
    который генерируется из файла xml
*/
class ModusDBusAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", DBUS_SERVER_NAME)
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"" DBUS_SERVER_NAME "\">\n"
"    <signal name=\"message\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"sender\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"text\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"type\"/>\n"
"    </signal>\n"
"    <signal name=\"action\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"nickname\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"text\"/>\n"
"    </signal>\n"
"  </interface>\n"
        "")
public:
    ModusDBusAdaptor(QObject *parent) : QDBusAbstractAdaptor(parent)
    {
      setAutoRelaySignals(true);
    }

    virtual ~ModusDBusAdaptor()
    {

    }

public: // PROPERTIES
public Q_SLOTS: // METHODS
Q_SIGNALS: // SIGNALS
    void action(const QString &nickname, const QString &text);
    void message(const QString &sender, const QString &text, const QString &type);
};


namespace sv {

  class SvDBus : public sv::SvAbstractLogger
  {
    Q_OBJECT

  public:

    explicit SvDBus(const sv::log::Options options = sv::log::Options(),
                    const sv::log::Flags flags = sv::log::lfNone, const QString &branch = "/",
                    QObject *parent = nullptr);

    void init(const QString &branch = "/");
    void setDebugMode(bool debug)
    {
      m_debug_mode = debug;
    }

    void log(sv::log::Level level, log::MessageTypes type, const QString text, sv::log::sender sender, bool newline = true);

    void log(sv::log::Level level, sv::log::MessageTypes type, const QStringList& list, sv::log::sender sender)
    {
      for(QString str: list)
        log(level, type, str, sender);
    }

    static void sendmsg(const sv::log::sender &sender, const QString &type, const QString& message);

    static QMutex mutex;

  private:
    QString m_branch = "/";
    bool    m_debug_mode = false;

  };
}



#endif // SV_DBUS_H
