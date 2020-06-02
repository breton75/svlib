﻿#ifndef SVDBUS_H
#define SVDBUS_H

#include <QObject>
#include <QtDBus>
#include <QDebug>

#include "sv_abstract_logger.h"

#define DBUS_SERVER_NAME "ame.proj12700.ksuts"

namespace sv {

  class SvDBus;
//  class sender;

  class SvDBusAdaptor: public QDBusAbstractAdaptor
  {
      Q_OBJECT
      Q_CLASSINFO("D-Bus Interface", DBUS_SERVER_NAME)
      Q_CLASSINFO("D-Bus Introspection", ""\
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
      SvDBusAdaptor(QObject *parent) : QDBusAbstractAdaptor(parent)
      {
          // constructor
          setAutoRelaySignals(true);
      }

      virtual ~SvDBusAdaptor() { }

  public: // PROPERTIES
  public Q_SLOTS: // METHODS
  Q_SIGNALS: // SIGNALS
      void action(const QString &nickname, const QString &text);
      void message(const QString &nickname, const QString &text, const QString &type);
  };

  class OrgExampleChatInterface: public QDBusAbstractInterface
  {
      Q_OBJECT
  public:
      static inline const char *staticInterfaceName()
      { return "org.example.chat"; }

  public:
      OrgExampleChatInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0)
      : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
      {

      }

      ~OrgExampleChatInterface() { }

  public Q_SLOTS: // METHODS
  Q_SIGNALS: // SIGNALS
      void action(const QString &sender, const QString &text);
      void message(const QString &sender, const QString &text, const QString &type);
  };


//  SvDBus& operator<< (SvDBus& logger, const sender& sender);

//  class sender
//  {
//    public:
//      explicit sender(const QString& senderName): name(senderName) { }
//      QString name;

////      friend sv::SvDBus& operator<< (sv::SvDBus& logger, const sv::sender& sender);

//  };


  class SvDBus : public sv::SvAbstractLogger
  {
    Q_OBJECT

    OrgExampleChatInterface* _iface = nullptr;

//    QString _current_sender = "";
//    QString _current_type = "";

//    static SvDBus* _instance;
  public:

    explicit SvDBus(const sv::log::Options options = sv::log::Options(),
                    const sv::log::Flags flags = sv::log::lfNone,
                    QObject *parent = nullptr);

//    static SvDBus& instance();

    void init();

    void log(sv::log::Level level, log::MessageTypes type, const QString& text, bool newline = true);

    void sendmsg(const QString &sender, const QString& message, const QString &type);

//    void setSender(const sv::sender& sender)   { _current_sender = sender; }

//  protected:
//    sv::SvDBus &operator<< (sv::sender& sender) override {
//qDebug() << sender.name;
//      _current_sender = sender;

//      return *this;

//    }

  private:
//    sv::sender _current_sender = sv::sender("");

  signals:

  public slots:
  };
}


//  sv::SvDBus& operator<< (sv::SvDBus& logger, const sv::sender& sender)
//  {
//    logger.setSender(sender);
//    return logger;
//  }

#endif // SVDBUS_H
