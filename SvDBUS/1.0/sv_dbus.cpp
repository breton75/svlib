#include "sv_dbus.h"

QMutex sv::SvDBus::mutex;

sv::SvDBus::SvDBus(const sv::log::Options options, const sv::log::Flags flags, const QString &branch, QObject *parent):
  sv::SvAbstractLogger(options, flags, parent)
{
  m_branch = branch;
}

void sv::SvDBus::init(const QString& branch)
{
  m_branch = branch;

  QDBusConnection::sessionBus().registerObject(m_branch, DBUS_SERVER_NAME, this);

  new proj::modus::modus(DBUS_SERVER_NAME, m_branch, QDBusConnection::sessionBus(), this);

//  ModusDBusAdaptor *mdba = new ModusDBusAdaptor(this);

}

void sv::SvDBus::log(sv::log::Level level, sv::log::MessageTypes type, const QString text, sv::log::sender sender, bool newline)
{
  if(p_options.enable && (level <= p_options.level))
  {

    QString msg = QString("%1").arg(text); //.arg(newline ? "\n" : "");
    sendmsg(sender, sv::log::typeToString(type), msg);

    if(newline)
      p_current_line_num++;

    if(m_debug_mode)
      qDebug() << msg;

  }

  resetCurrentData();

//  mutex.unlock();
}

void sv::SvDBus::sendmsg(const log::sender &sender, const QString &type, const QString& message)
{
  // при создании лочится, при завершении функции - locker удаляется, и разлочивается
  QMutexLocker locker(&mutex);

  QDBusMessage msg = QDBusMessage::createSignal(QString("/%1").arg(sender.entity()), DBUS_SERVER_NAME, "message");

  msg << sender.entity() << sender.id() << type << message;

  QDBusConnection::sessionBus().send(msg);
}

//sv::SvDBus &sv::SvDBus::instance()
//{
//  if(!_instance)
//    _instance = new sv::SvDBus();

//  return *_instance;
//}
