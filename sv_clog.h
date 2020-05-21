#ifndef SVCLOG_H
#define SVCLOG_H


#include <QString>
#include <QStringList>
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QMutex>
//#include <QTextStream>
#include <iostream>
#include <QTextCodec>
#include <QFile>
#include <QDir>
#include <QFileInfo>
//#include <QCoreApplication>

#include <sys/stat.h>
//#include <sys/types.h>
#include <unistd.h>

#include "sv_abstract_logger.h"
#include "sv_fnt.h"
#include "sv_exception.h"

namespace sv
{
  class SvFileLogger;
  class SvConcoleLogger;
}

class sv::SvFileLogger: public sv::SvAbstractLogger {

  Q_OBJECT

  QString p_file_name_prefix = "";

  FILE* p_log_file = nullptr;
  svfnt::SvRE p_re;
  QTime p_file_time_watcher;
  QFileInfo p_file_size_watcher;

  QString _last_error = "";

public:
  explicit SvFileLogger(const sv::log::Options options = sv::log::Options(),
                     const sv::log::Flags flags = sv::log::lfNone,
                     QObject *parent = nullptr):
    sv::SvAbstractLogger(options, flags, parent)
  {

  }

  void log(sv::log::Level level, log::MessageTypes type, const QString& text, bool newline = true);

  bool checkFile();

  void setFileNamePrefix(const QString& prefix) { p_file_name_prefix = prefix; }

};

class sv::SvConcoleLogger: public SvAbstractLogger {

  Q_OBJECT

public:
  explicit SvConcoleLogger(const sv::log::Options options = sv::log::Options(),
                           const sv::log::Flags flags = sv::log::lfNone,
                           QObject *parent = nullptr):
    sv::SvAbstractLogger(options, flags, parent)
  {

  }

  void log(sv::log::Level level, log::MessageTypes type, const QString& text, bool newline = true);

};

#endif // SVCLOG_H
