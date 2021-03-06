﻿#ifndef SV_ABSTRACT_LOGGER
#define SV_ABSTRACT_LOGGER

#include <QString>
#include <QStringList>
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include "../../SvException/svexception.h"

#define LOG_DEFAULT_PATH          "log"
#define LOG_DEFAULT_FILENAME      "ddMMyyyy_hhmmss"
#define LOG_DEFAULT_DATE_FORMAT   "dd.MM.yyyy"
#define LOG_DEFAULT_TIME_FORMAT   "hh:mm:ss"
#define LOG_DEFAULT_ROTATION_AGE  3600
#define LOG_DEFAULT_ROTATION_SIZE 10485760

namespace sv
{

  class SvAbstractLogger;

  namespace log
  {

    class sender;

    enum MessageTypes {

      mtSuccess   = 0x00000000,

      // служебные
      mtSimple    = 0x00000001,
      mtInfo      = 0x00000002,
      mtEvent     = 0x00000004,
      mtAction    = 0x00000006,
      mtDuty      = 0x0000000F,

      // ошибки и предпреждения
      mtAttention = 0x00000010,
      mtWarning   = 0x00000020,
      mtFail      = 0x00000040,
      mtCritical  = 0x00000080,
      mtError     = 0x000000F0,

      // данные
      mtNew       = 0x00000100,
      mtChange    = 0x00000200,
      mtDelete    = 0x00000400,
      mtConfirm   = 0x00000800,
      mtData      = 0x00000F00,

      // обмен данными
      mtReceive   = 0x00001000,
      mtSend      = 0x00002000,
      mtReset     = 0x00004000,
      mtParse     = 0x00008000,
      mtExcange   = 0x0000F000,

      // подключение и логин
      mtConnect    = 0x00010000,
      mtLost       = 0x00020000,
      mtLogin      = 0x00040000,
      mtRequest    = 0x00080000,
      mtConnection = 0x000F0000,

      // debug
      mtDebug1     = 0x00100000,
      mtDebug2     = 0x00200000,
      mtDebug3     = 0x00400000,
      mtDebug4     = 0x00800000,
      mtDebug      = 0x00F00000,

      mtAny        = 0xFFFFFFFF
//      mtAll        = 0xFFFFFFFF


    };

    enum Level {
      llNone = 0,
      llError,
      llWarning,
      llInfo,
      llDebug,
      llDebug2,
      llAll,
      llUndefined = 0xFFFF,
    };

    const MessageTypes DEFAULT_MESSAGE_TYPE = sv::log::mtSimple;
    const Level        DEFAULT_LOG_LEVEL    = sv::log::llNone;

    enum MessageBuns {
      LineN = 0,
      Date,
      Time,
      TimeZZZ,
      endl,
      endi,
      out,
      in
    };

    typedef QList<MessageBuns> BunList;

    enum Devices {
      ldUndefined = -1,
      ldConsole,
      ldFile,
      ldWidget
    };

    enum Flags {
      lfNone = 0,
      lfCheckLogLevel = 0x01
    };


    const QMap<QString, Level> LogLevelNames =    {{"none",    sv::log::llNone},
                                                   {"error",   sv::log::llError},
                                                   {"warning", sv::log::llWarning},
                                                   {"info",    sv::log::llInfo},
                                                   {"debug",   sv::log::llDebug},
                                                   {"debug2",  sv::log::llDebug2},
                                                   {"all",     sv::log::llAll}};

    const QMap<QString, Devices> LogDeviceNames = {{"console", sv::log::ldConsole},
                                                   {"file",    sv::log::ldFile},
                                                   {"widget",  sv::log::ldWidget}};

    const QMap<sv::log::MessageTypes, QString> LogMessageTypesNames = {
                                                   {sv::log::MessageTypes::mtSuccess,    "success"},

                                                   {sv::log::MessageTypes::mtSimple,     "simple"},
                                                   {sv::log::MessageTypes::mtInfo,       "info"},
                                                   {sv::log::MessageTypes::mtEvent,      "event"},
                                                   {sv::log::MessageTypes::mtAction,     "action"},
                                                   {sv::log::MessageTypes::mtDuty,       "duty"},

                                                   {sv::log::MessageTypes::mtAttention,  "attention"},
                                                   {sv::log::MessageTypes::mtWarning,    "warning"},
                                                   {sv::log::MessageTypes::mtFail,       "fail"},
                                                   {sv::log::MessageTypes::mtCritical,   "critical"},
                                                   {sv::log::MessageTypes::mtError,      "error"},

                                                   {sv::log::MessageTypes::mtNew,        "new"},
                                                   {sv::log::MessageTypes::mtChange,     "change"},
                                                   {sv::log::MessageTypes::mtDelete,     "delete"},
                                                   {sv::log::MessageTypes::mtConfirm,    "confirm"},
                                                   {sv::log::MessageTypes::mtData,       "data"},

                                                   {sv::log::MessageTypes::mtReceive,    "receive"},
                                                   {sv::log::MessageTypes::mtSend,       "send"},
                                                   {sv::log::MessageTypes::mtReset,      "reset"},
                                                   {sv::log::MessageTypes::mtParse,      "parse"},
                                                   {sv::log::MessageTypes::mtExcange,    "excange"},

                                                   {sv::log::MessageTypes::mtConnect,    "connect"},
                                                   {sv::log::MessageTypes::mtLost,       "lost"},
                                                   {sv::log::MessageTypes::mtLogin,      "login"},
                                                   {sv::log::MessageTypes::mtRequest,    "request"},
                                                   {sv::log::MessageTypes::mtConnection, "connection"},

                                                   {sv::log::MessageTypes::mtDebug1,     "debug1"},
                                                   {sv::log::MessageTypes::mtDebug2,     "debug2"},
                                                   {sv::log::MessageTypes::mtDebug3,     "debug3"},
                                                   {sv::log::MessageTypes::mtDebug4,     "debug4"},
                                                   {sv::log::MessageTypes::mtDebug,      "debug"},

                                                   {sv::log::MessageTypes::mtAny,        "any"}
//                                                   {sv::log::MessageTypes::mtAll,        "all"}

    };

    typedef QList<Devices> LogDeviceList;


    /*
     * следующие функции предназначены для разбора конфигурации логирования
     * и преобразования данных для вывода в лог
     */

    /* преобразует массив байт в последовательность символов,
     * разделенных разделителем splitter для вывода в лог */
    QString bytesToText(const QByteArray* b, QString splitter = "");


    /* преобразует массив байт в последовательность HEX кодов,
     * разделенных разделителем splitter для вывода в лог */
    QString bytesToHex(const QByteArray* b, QString splitter);


    /* преобразует текстовое имя в уровень логирования */
    sv::log::Level stringToLevel(const QString& str, bool* ok = nullptr);

    /* преобразует уровень логирования в текстовое имя */
    QString levelToString(const sv::log::Level level);

    /* преобразует текстовое имя в устройство логирования */
    sv::log::Devices stringToDevice(const QString& str, bool* ok = nullptr);

    /* преобразует текстовое имя в логический тип */
    bool stringToBool(const QString& str);


    /* преобразует текстовое представление времени формата 23h59m59s в секунды */
    quint32 stringToSeconds(const QString& str, bool* ok = nullptr);

    /* преобразует текстовое представление размера файла формата 1023gb1023mb1023kb1023
     * в количество байт */
    quint64 stringToSize(const QString& str, bool* ok = nullptr);

    /* преобразует список устройств логирования в текстовое представление */
    QString deviceListToString(const sv::log::LogDeviceList& list);

    /* преобразует тип сообщения в его текстовое представление */
    QString typeToString(const sv::log::MessageTypes type);

    /* преобразует текстовое представление в тип сообщения */
    sv::log::MessageTypes stringToType(const QString& str);


    struct Options {

        bool            enable                = false;
        sv::log::Level  level                 = llWarning;
        LogDeviceList   devices               = LogDeviceList({ldConsole});
        QString         path                  = LOG_DEFAULT_PATH;
        QString         filename              = LOG_DEFAULT_FILENAME;
        QString         date_format           = LOG_DEFAULT_DATE_FORMAT;
        QString         time_format           = LOG_DEFAULT_TIME_FORMAT;
        bool            truncate_on_rotation  = false;
        quint32         rotation_age          = LOG_DEFAULT_ROTATION_AGE; // в секундах
        qint64          rotation_size         = LOG_DEFAULT_ROTATION_SIZE; // в байтах (10 мб)


        static Options fromJsonString(const QString& json_string)
        {
          QJsonParseError err;
          QJsonDocument jd = QJsonDocument::fromJson(json_string.toUtf8(), &err);

          if(err.error != QJsonParseError::NoError)
            throw SvException(err.errorString());

          try {

            return fromJsonObject(jd.object());

          }
          catch(SvException& e) {
            throw e;
          }
        }

        static Options fromJsonObject(const QJsonObject &object)
        {
          QString P;
          Options p;

          /* enable */
          P = "enable";
          p.enable = object.contains(P) ? object.value(P).toBool(false) : false;

          /* level */
          P = "level";
          p.level = object.contains(P) ? stringToLevel(object.value(P).toString("")) : llWarning;

          /* path */
          P = "path";
          p.path = object.contains(P) ? object.value(P).toString(LOG_DEFAULT_PATH) : LOG_DEFAULT_PATH;

          /* filename */
          P = "path";
          p.filename = object.contains(P) ? object.value(P).toString(LOG_DEFAULT_FILENAME) : LOG_DEFAULT_FILENAME;

          /* date_format */
          P = "date_format";
          p.date_format = object.contains(P) ? object.value(P).toString(LOG_DEFAULT_DATE_FORMAT) : LOG_DEFAULT_DATE_FORMAT;

          /* time_format */
          P = "time_format";
          p.time_format = object.contains(P) ? object.value(P).toString(LOG_DEFAULT_TIME_FORMAT) : LOG_DEFAULT_TIME_FORMAT;

          /* truncate_on_rotation */
          P = "truncate_on_rotation";
          p.truncate_on_rotation = object.contains(P) ? object.value(P).toBool(false) : false;

          /* rotation_age */
          P = "rotation_age";
          p.rotation_age = object.contains(P) ? object.value(P).toInt(LOG_DEFAULT_ROTATION_AGE) : LOG_DEFAULT_ROTATION_AGE;

          /* rotation_size */
          P = "rotation_size";
          p.rotation_size = object.contains(P) ? object.value(P).toInt(LOG_DEFAULT_ROTATION_SIZE) : LOG_DEFAULT_ROTATION_SIZE;

          return p;

        }

        QString toJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const
        {
          QJsonDocument jd;
          jd.setObject(toJsonObject());

          return QString(jd.toJson(format));
        }

        QJsonObject toJsonObject() const
        {
          QJsonObject j;

          j.insert("enable",                QJsonValue(enable).toBool());
          j.insert("level",                 QJsonValue(levelToString(level)).toString());
//          j.insert("devices",      ;
          j.insert("path",                  QJsonValue(path).toString());
          j.insert("filename",              QJsonValue(filename).toString());
          j.insert("date_format",           QJsonValue(date_format).toString());
          j.insert("time_format",           QJsonValue(time_format).toString());
          j.insert("truncate_on_rotation",  QJsonValue(truncate_on_rotation).toBool());
          j.insert("rotation_age",          QJsonValue(int(rotation_age)).toInt());
          j.insert("rotation_size",         QJsonValue(rotation_size).toInt());

          return j;
        }
    };

    class sender
    {
      public:
        explicit sender():
          m_module(QString()),
          m_id(0)
        { }

        explicit sender(const QString& module, int id = 0):
          m_module(module),
          m_id(id)
        { }

        QString module() const { return m_module; }
        int     id()     const { return m_id;   }

        private:
          QString m_module;
          int     m_id;

    };

  }

  class SvAbstractLogger: public QObject
  {
      Q_OBJECT

  protected:
    QObject *p_parent = nullptr;

    QString p_current_line = "";

    sv::log::MessageTypes p_current_msg_type = sv::log::DEFAULT_MESSAGE_TYPE;
    sv::log::Level p_current_log_lvl = sv::log::DEFAULT_LOG_LEVEL;
    sv::log::Options p_options;

    sv::log::sender p_current_sender = sv::log::sender("");

    int p_current_line_num = 1;

    QChar p_separator = ' ';

    quint8 p_check_log_level:1;

  public:
    explicit SvAbstractLogger(const sv::log::Options options,
                                     const sv::log::Flags flags = sv::log::lfNone,
                                     QObject *parent = nullptr) :
      QObject(parent),
      p_parent(parent),
      p_options(options)
    {
      setFlags(flags);
    }

    SvAbstractLogger(SvAbstractLogger &other) :
      QObject(other.p_parent)
    {
      p_parent = other.p_parent;
      p_options = other.p_options;
      p_current_line_num = other.p_current_line_num;
    }

    /** два && !!! **/
    SvAbstractLogger  &operator= (SvAbstractLogger &&other) {

      if(this != &other) {

        setParent(other.p_parent);
        p_options = other.p_options;
        p_current_log_lvl = other.p_current_log_lvl;
      }

      return *this;
    }

    /** один & !!! **/
    SvAbstractLogger &operator= (const SvAbstractLogger &other) {

      if(this != &other) {

        setParent(other.p_parent);
        p_options = other.p_options;
        p_current_line_num = other.p_current_line_num;
      }

      return *this;
    }

    virtual void log(sv::log::Level level,
                     sv::log::MessageTypes type, const QString text,
                     sv::log::sender sender, bool newline = true) = 0;

    virtual void log(bool newline = true) { log(p_current_log_lvl, p_current_msg_type,
                                                p_current_line, p_current_sender, newline); }

    virtual void log(sv::log::Level level, sv::log::MessageTypes type, const QStringList& list, sv::log::sender sender)
    {
      for(QString str: list)
        log(level, type, str, sender);
    }

    const QString currentLine() const                       { return p_current_line;        }

    virtual void setSeparator(QChar separator)              { p_separator = separator;      }
    virtual void setOptions(const sv::log::Options options) { p_options = options;          }
    virtual void setFlags(const sv::log::Flags flags)       { p_check_log_level = flags;    }
    virtual void setSender(const sv::log::sender& sender)   { p_current_sender = sender;    }
    virtual void setEnable(bool enable)                     { p_options.enable = enable;   }

    virtual const sv::log::Options options() const          { return p_options;             }

    virtual void resetCurrentData()
    {
      p_current_line      = QString();
      p_current_msg_type  = sv::log::DEFAULT_MESSAGE_TYPE;
      p_current_log_lvl   = sv::log::DEFAULT_LOG_LEVEL;
    }


    /** operators **/
    sv::SvAbstractLogger &operator<< (sv::log::MessageTypes type) {

        p_current_msg_type = type;
        return *this;
    }

    sv::SvAbstractLogger &operator<< (sv::log::Level level) {

        p_current_log_lvl = level;
        return *this;
    }

    sv::SvAbstractLogger &operator<< (sv::log::MessageBuns bun) {

      switch (bun) {
      case sv::log::LineN:
        p_current_line += QString::number(p_current_line_num) + p_separator;
        break;

      case sv::log::Date:
        p_current_line += QDate::currentDate().toString(p_options.date_format) + p_separator;
        break;

      case sv::log::Time:
        p_current_line += QTime::currentTime().toString(p_options.time_format) + p_separator;
        break;

      case sv::log::TimeZZZ:
      {
        QTime t = QTime::currentTime();
        p_current_line += QString("%1.%2%3")
            .arg(t.toString(p_options.time_format))
            .arg(t.toString("zzz"))
            .arg(p_separator);
        break;
      }

      case sv::log::endl:
        log();
        break;

      case sv::log::endi:
        log(false);
        break;

      case sv::log::in:
        p_current_line += QString(">>%1").arg(p_separator);
        break;

      case sv::log::out:
        p_current_line += QString("<<%1").arg(p_separator);
        break;
      }

      return *this;

    }

    sv::SvAbstractLogger &operator<< (QDate date)
    {

      p_current_line += date.toString(p_options.date_format) + p_separator;

      return *this;

    }

    sv::SvAbstractLogger &operator<< (QTime time)
    {
      p_current_line += QString("%1%2").arg(time.toString(p_options.time_format)).arg(p_separator);

      return *this;

    }

    sv::SvAbstractLogger &operator<< (const QString string)
    {
      p_current_line += string + p_separator;

      return *this;

    }

    sv::SvAbstractLogger &operator<< (qreal f)
    {
      p_current_line += QString::number(f) + p_separator;

      return *this;

    }

    sv::SvAbstractLogger &operator<< (char ch)
    {
      p_current_line += ch;
      p_current_line += p_separator;

      return *this;

    }

    sv::SvAbstractLogger &operator<< (signed int i)
    {
      p_current_line += QString::number(i) + p_separator;

      return *this;

    }

    sv::SvAbstractLogger &operator<< (unsigned int i)
    {
      p_current_line += QString::number(i) + p_separator;

      return *this;

    }

    sv::SvAbstractLogger &operator<< (long long unsigned int i)
    {
      p_current_line += QString::number(i) + p_separator;

      return *this;
    }

    sv::SvAbstractLogger &operator<< (long long signed int i)
    {
      p_current_line += QString::number(i) + p_separator;

      return *this;

    }

    sv::SvAbstractLogger &operator<< (sv::log::sender&& sender) {

      p_current_sender = sender;

      return *this;

    }

    sv::SvAbstractLogger &operator<< (sv::log::sender& sender) {

      p_current_sender = sender;

      return *this;

    }

  };
}


#endif // SV_ABSTRACT_LOGGER

