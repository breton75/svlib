#ifndef SV_SERVER_LOGGER_H
#define SV_SERVER_LOGGER_H

#include <QObject>
#include <QTcpServer>

#include "sv_abstract_logger.h"

namespace sv {
  class SvServerLogger;
}

class sv::SvServerLogger : public sv::SvAbstractLogger
{
public:
  SvServerLogger();

  void init(const QString& ip, quint16 port);
  void start();

private:
  QTcpServer _server;

};

#endif // SV_SERVER_LOGGER_H
