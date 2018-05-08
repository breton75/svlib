#ifndef SVLOG_H
#define SVLOG_H

//#include <QSqlError>

#include <QWidget>
#include <QTextEdit>
#include <QString>
#include <QStringList>
#include <QMainWindow>
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QColor>

#include <QMainWindow>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QMutex>

namespace svlog
{
  enum MessageTypes {
    Simple = 0,
    Data,
    Error,
    Info,
    Duty,
    Attention,
    NewData,
    Fail,
    Debug,
    Success,
    Critical
  };
  
  enum MessageBuns {
    LineN = 0,
    Date,
    Time,
    TimeZZZ,
    endl,
    out,
    in
  };
  
//  void log_SQL_error(QSqlError error, QString noErrorMsg = "OK");
  
  QString bytesToText(const QByteArray* b, QString splitter = "");
  QString bytesToHex(const QByteArray* b, QString splitter = "");

  class SvLog;
      
}

class svlog::SvLog: public QObject
{
    Q_OBJECT
  
public:
  explicit SvLog(QTextEdit* logEdit = nullptr, QObject *parent = nullptr) :
    QObject(parent)
  {
    _parent = parent;
    _log_edit = logEdit;
  }
  
  
  SvLog(svlog::SvLog &other) :
    QObject(other.parent())
  {
    _parent = other.parent();
    _current_line = other.currentLine();
    _current_msg_type = other.currentMsgType();
    _date_format = other.dateFormat();
    _time_format = other.timeFormat();
  }
  
//  QObject *parent() { return _parent; }
  void setTextEdit(QTextEdit *textEdit) { _log_edit = textEdit; }
  QTextEdit *logEdit() { return _log_edit; }
  
  QDockWidget *createLog(QMainWindow *window = nullptr);
  void assignLog(QTextEdit *widget = nullptr);
  
  void log(svlog::MessageTypes type, QString text);
  void log() { log(_current_msg_type, _current_line); }
  void log(svlog::MessageTypes type, QStringList list);
  
  static void log(svlog::MessageTypes type, QString text, QTextEdit *textedit);
  
  QString dateFormat() { return _date_format; }
  void setDateFormat(QString dfmt) { _date_format = dfmt; }
  QString timeFormat() { return _time_format; }
  void setTimeFormat(QString tfmt) { _time_format = tfmt; }
  void setSeparator(QChar separator) { _separator = separator; }  
  
  svlog::MessageTypes currentMsgType() { return _current_msg_type; }
  QString currentLine() { return _current_line; }
  
  QDockWidget* dockWidget() { return _dockWidget; }
  
//  svlog::SvLog *operator= (svlog::SvLog *other) {
//    return other;
//  }
  
  /** два && !!! **/
  svlog::SvLog &operator= (svlog::SvLog &&other) {
    if(this != &other) {
      setParent(other.parent());
      _log_edit = other.logEdit();
      _current_line = other.currentLine();
      _current_msg_type = other.currentMsgType();
      _date_format = other.dateFormat();
      _time_format = other.timeFormat();
    }
    return *this;
  }
  
  /** один & !!! **/
  svlog::SvLog &operator= (svlog::SvLog &other) {
    if(this != &other) {
      setParent(other.parent());
      _log_edit = other.logEdit();
      _current_line = other.currentLine();
      _current_msg_type = other.currentMsgType();
      _date_format = other.dateFormat();
      _time_format = other.timeFormat();
    }
    return *this;
  }

  svlog::SvLog &operator<< (svlog::MessageTypes type) {
    _current_msg_type = type; return *this;
  }
  
  svlog::SvLog &operator<< (svlog::MessageBuns mp) {
    switch (mp) {
      case svlog::LineN:
        _current_line += QString::number(_current_line_num);
        _current_line += _separator;
        break;
        
      case svlog::Date:
        _current_line += QDate::currentDate().toString(_date_format);
        _current_line += _separator;
        break;
        
      case svlog::Time:
        _current_line += QTime::currentTime().toString(_time_format);
        _current_line += _separator;
        break;
        
      case svlog::TimeZZZ:
      {
        QTime t = QTime::currentTime();
        _current_line += QString("%1.%2%3")
                         .arg(t.toString(_time_format))
                         .arg(t.toString("zzz"))
                         .arg(_separator);
        break;
      }
        
      case svlog::endl:
//        _current_line += '\n'; 
        log();
        break;

      case svlog::in:
        _current_line += ">>";
        _current_line += _separator;
        break;

      case svlog::out:
        _current_line += "<<";
        _current_line += _separator;
        break;        
    }
    return *this;
  }
  
  svlog::SvLog &operator<< (QDate date) { 
    _current_line += date.toString(_date_format);
    _current_line += _separator;
    return *this;
  }
  
  svlog::SvLog &operator<< (QTime time) { 
    _current_line += time.toString(_time_format);
    _current_line += _separator;
    return *this;
  }
  
//  QTextStream &operator<< (const QByteArray &array);
  
  svlog::SvLog &operator<< (const QString &string) {
    _current_line += string;
    _current_line += _separator;
    return *this;
  }
  
//  svlog::SvLog &operator<< (float f) {
//    _current_line += QString::number(f);
//    _current_line += _separator;
//    return *this;
//  }
  
//  svlog::SvLog &operator<< (double f) {
//    _current_line += QString::number(f);
//    _current_line += _separator;
//    return *this;
//  }
  
  svlog::SvLog &operator<< (qreal f) {
    _current_line += QString::number(f);
    _current_line += _separator;
    return *this;
  }
  
  svlog::SvLog &operator<< (char ch) {
    _current_line += ch;
    _current_line += _separator;
    return *this;
  }
  
  svlog::SvLog &operator<< (signed int i) {
    _current_line += QString::number(i);
    _current_line += _separator;
    return *this;
  }
  
  svlog::SvLog &operator<< (unsigned int i) {
    _current_line += QString::number(i);
    _current_line += _separator;
    return *this;
  }
  
  
  
private:
  QTextEdit* _log_edit = nullptr;
  QObject *_parent = nullptr;
  
  QString _date_format = "dd.MM.yyyy";
  QString _time_format = "hh:mm:ss";
  
  QString _current_line = "";
  svlog::MessageTypes _current_msg_type = svlog::Simple;
  
  int _current_line_num = 1;
  
  QChar _separator = ' ';
  
  QDockWidget *_dockWidget = nullptr;
  
};
    



#endif // SVLOG_H
