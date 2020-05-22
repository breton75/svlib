/**********************************************************************
 *  константы и структуры для работы с последовательным портом.
 *  форма для редактирования находится в файлах sv_serial_editor.*
 *  ВАЖНО! решено разнести парметры и редактор по разным фалам,
 *  чтобы не было проблемы при работе с приложениями без GUI
 *
 *  автор Свиридов С.А. Авиационные и Морская Электроника
 * *********************************************************************/

#ifndef SV_UDP_PARAMS
#define SV_UDP_PARAMS

#include <QtGlobal>
#include <QtCore/QCommandLineParser>
#include <QUdpSocket>
#include <QHostAddress>

#include <QJsonDocument>
#include <QJsonObject>

#include "sv_exception.h"

// имена параметров для UDP
#define P_UDP_HOST        "host"
#define P_UDP_LISTEN_PORT "listen_port"
#define P_UDP_REMOTE_PORT "remote_port"

namespace sv {

  /** структура для хранения параметров последовательного порта **/
  struct UdpParams {

    QString host        = "";
    quint16 listen_port = 5300;
    quint16 remote_port = 5300;

    static UdpParams fromJsonString(const QString& json_string)
    {
      QJsonDocument jd = QJsonDocument::fromJson(json_string.toUtf8());
      return fromJsonObject(jd.object());
    }

    static UdpParams fromJsonObject(const QJsonObject &object)
    {
      UdpParams p;

      if(object.contains(P_UDP_HOST))
        p.host = object.value(P_UDP_HOST).toString("");

      if(object.contains(P_UDP_LISTEN_PORT))
        p.listen_port = object.value(P_UDP_LISTEN_PORT).toInt(5300);

      if(object.contains(P_UDP_REMOTE_PORT))
        p.remote_port = object.value(P_UDP_REMOTE_PORT).toInt(5300);

      return p;

    }

    QString toJsonString() const
    {
      QJsonDocument jd;
      jd.setObject(toJsonObject());

      return QString(jd.toJson(QJsonDocument::Indented));
    }

    QJsonObject toJsonObject() const
    {
      QJsonObject j;

      j.insert(P_UDP_HOST, QJsonValue(host).toString());
      j.insert(P_UDP_LISTEN_PORT, QJsonValue(static_cast<int>(listen_port)).toInt());
      j.insert(P_UDP_REMOTE_PORT, QJsonValue(static_cast<int>(remote_port)).toInt());

      return j;

    }
  };
}

#endif // SV_UDP_PARAMS

