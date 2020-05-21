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

/*
 * для консольных приложений закомментировать !
 */
#define GUI_APP

namespace sv
{

  class SvFileLogger;

  class SvConcoleLogger;
#ifdef GUI_APP
  class SvWidgetLogger;
#endif
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

#ifdef GUI_APP

#include <QtWidgets/QTextEdit>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMessageBox>


class sv::SvWidgetLogger: public sv::SvAbstractLogger
{
    Q_OBJECT

  QTextEdit* _log_edit = nullptr;
  QDockWidget *_dockWidget = Q_NULLPTR;



  QMap<sv::log::MessageTypes, QColor> typesColors = {{sv::log::mtSimple,    Qt::black         },
                                                     {sv::log::mtData,      QColor(223344)    },
                                                     {sv::log::mtError,     Qt::red           },
                                                     {sv::log::mtInfo,      QColor(0x009A8832)},
                                                     {sv::log::mtDuty,      QColor(0x003A6098)},
                                                     {sv::log::mtAttention, Qt::darkMagenta   },
                                                     {sv::log::mtNewData,   QColor(0x00BD6311)},
                                                     {sv::log::mtFail,      QColor(0x00173EF4)},
                                                     {sv::log::mtDebug,     QColor(0x008A8A8A)},
                                                     {sv::log::mtSuccess,   QColor(0x0020A230)},
                                                     {sv::log::mtCritical,  Qt::red           }};

public:
  explicit SvWidgetLogger(const sv::log::Options options = sv::log::Options(),
                          QTextEdit* logEdit = 0,
                          const sv::log::Flags flags = sv::log::lfNone,
                          QObject *parent = nullptr):
    sv::SvAbstractLogger(options, flags, parent),
    _log_edit(logEdit)
  {

  }

  ~SvWidgetLogger();

  void log(sv::log::Level level, log::MessageTypes type, const QString& text, bool newline = true);

  void setTextEdit(QTextEdit *textEdit) { _log_edit = textEdit; }
  QTextEdit *logEdit() const { return _log_edit; }

  QDockWidget *createDockWidgetLog(QMainWindow *window = Q_NULLPTR, const QString& title = "", QDockWidget::DockWidgetFeature feature = QDockWidget::DockWidgetFloatable);
  void bindTo(QTextEdit *textEdit);

};
#endif // GUI_APP


#endif // SVCLOG_H
