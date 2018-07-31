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

namespace clog
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

  class SvCLog;
      
}

class clog::SvCLog: public QObject
{
    Q_OBJECT
  
public:
  explicit SvCLog(QObject *parent = nullptr) :
    QObject(parent)
  {
    _parent = parent;
//    _log_edit = logEdit;
  }
  
  
  SvCLog(clog::SvCLog &other) :
    QObject(other.parent())
  {
    _parent = other.parent();
    _current_line = other.currentLine();
    _current_msg_type = other.currentMsgType();
    _date_format = other.dateFormat();
    _time_format = other.timeFormat();
  }
  
//  QObject *parent() { return _parent; }
//  void setTextEdit(QTextEdit *textEdit) { _log_edit = textEdit; }
//  QTextEdit *logEdit() { return _log_edit; }
  
//  QDockWidget *createLog(QMainWindow *window = nullptr);
//  void assignLog(QTextEdit *widget = nullptr);
  
  void log(clog::MessageTypes type, QString text);
  void log() { log(_current_msg_type, _current_line); }
  void log(clog::MessageTypes type, QStringList list);
  
//  static void log(svclog::MessageTypes type, QString text, QTextEdit *textedit);
  
  QString dateFormat() { return _date_format; }
  void setDateFormat(QString dfmt) { _date_format = dfmt; }
  QString timeFormat() { return _time_format; }
  void setTimeFormat(QString tfmt) { _time_format = tfmt; }
  void setSeparator(QChar separator) { _separator = separator; }  
  
  clog::MessageTypes currentMsgType() { return _current_msg_type; }
  QString currentLine() { return _current_line; }
  
//  QDockWidget* dockWidget() { return _dockWidget; }
  
//  clog::SvCLog *operator= (clog::SvCLog *other) {
//    return other;
//  }
  
  /** два && !!! **/
  clog::SvCLog &operator= (clog::SvCLog &&other) {
    if(this != &other) {
      setParent(other.parent());
//      _log_edit = other.logEdit();
      _current_line = other.currentLine();
      _current_msg_type = other.currentMsgType();
      _date_format = other.dateFormat();
      _time_format = other.timeFormat();
    }
    return *this;
  }
  
  /** один & !!! **/
  clog::SvCLog &operator= (clog::SvCLog &other) {
    if(this != &other) {
      setParent(other.parent());
//      _log_edit = other.logEdit();
      _current_line = other.currentLine();
      _current_msg_type = other.currentMsgType();
      _date_format = other.dateFormat();
      _time_format = other.timeFormat();
    }
    return *this;
  }

  clog::SvCLog &operator<< (clog::MessageTypes type) {
    _current_msg_type = type; return *this;
  }
  
  clog::SvCLog &operator<< (clog::MessageBuns mp) {
    switch (mp) {
      case clog::LineN:
        _current_line += QString::number(_current_line_num);
        _current_line += _separator;
        break;
        
      case clog::Date:
        _current_line += QDate::currentDate().toString(_date_format);
        _current_line += _separator;
        break;
        
      case clog::Time:
        _current_line += QTime::currentTime().toString(_time_format);
        _current_line += _separator;
        break;
        
      case clog::TimeZZZ:
      {
        QTime t = QTime::currentTime();
        _current_line += QString("%1.%2%3")
                         .arg(t.toString(_time_format))
                         .arg(t.toString("zzz"))
                         .arg(_separator);
        break;
      }
        
      case clog::endl:
//        _current_line += ; 
        log();
        break;

      case clog::in:
        _current_line += ">>";
        _current_line += _separator;
        break;

      case clog::out:
        _current_line += "<<";
        _current_line += _separator;
        break;        
    }
    return *this;
  }
  
  clog::SvCLog &operator<< (QDate date) { 
    _current_line += date.toString(_date_format);
    _current_line += _separator;
    return *this;
  }
  
  clog::SvCLog &operator<< (QTime time) { 
    _current_line += time.toString(_time_format);
    _current_line += _separator;
    return *this;
  }
  
//  QTextStream &operator<< (const QByteArray &array);
  
  clog::SvCLog &operator<< (const QString &string) {
    _current_line += string;
    _current_line += _separator;
    return *this;
  }
  
//  clog::SvCLog &operator<< (float f) {
//    _current_line += QString::number(f);
//    _current_line += _separator;
//    return *this;
//  }
  
//  clog::SvCLog &operator<< (double f) {
//    _current_line += QString::number(f);
//    _current_line += _separator;
//    return *this;
//  }
  
  clog::SvCLog &operator<< (qreal f) {
    _current_line += QString::number(f);
    _current_line += _separator;
    return *this;
  }
  
  clog::SvCLog &operator<< (char ch) {
    _current_line += ch;
    _current_line += _separator;
    return *this;
  }
  
  clog::SvCLog &operator<< (signed int i) {
    _current_line += QString::number(i);
    _current_line += _separator;
    return *this;
  }
  
  clog::SvCLog &operator<< (unsigned int i) {
    _current_line += QString::number(i);
    _current_line += _separator;
    return *this;
  }
  
  
  
private:
  QObject *_parent = nullptr;
  
//  QTextStream _cout = QTextStream(stdout);
  
  QString _date_format = "dd.MM.yyyy";
  QString _time_format = "hh:mm:ss";
  
  QString _current_line = "";
  clog::MessageTypes _current_msg_type = clog::Simple;
  
  int _current_line_num = 1;
  
  QChar _separator = ' ';
  
  QTextCodec* _codec = QTextCodec::codecForName("CP866");
  
};
    



#endif // SVCLOG_H
