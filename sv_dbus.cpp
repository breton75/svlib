#include "sv_dbus.h"


//sv::SvDBus* sv::SvDBus::_instance = nullptr;

sv::SvDBus::SvDBus(const sv::log::Options options, const sv::log::Flags flags, QObject *parent):
  sv::SvAbstractLogger(options, flags, parent)
{
  new SvDBusAdaptor(this);
  QDBusConnection::sessionBus().registerObject("/", this);

  _iface = new OrgExampleChatInterface(QString(), QString(), QDBusConnection::sessionBus(), 0);

}

void sv::SvDBus::log(sv::log::Level level, sv::log::MessageTypes type, const QString &text, bool newline)
{
  if(p_options.logging && (level <= p_options.log_level))
  {

    QString msg = QString("%1").arg(text); //.arg(newline ? "\n" : "");
    sendmsg(p_current_sender, msg, sv::log::typeToString(type));

    if(newline)
      p_current_line_num++;

  }

  resetCurrentData();

}

void sv::SvDBus::sendmsg(const QString &sender, const QString& message, const QString &type)
{
  QDBusMessage msg = QDBusMessage::createSignal("/", DBUS_SERVER_NAME, "message");
  msg << sender << message << type;
  QDBusConnection::sessionBus().send(msg);
}

//sv::SvDBus &sv::SvDBus::instance()
//{
//  if(!_instance)
//    _instance = new sv::SvDBus();

//  return *_instance;
//}
