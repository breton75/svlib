#ifndef SV_EXCEPTION_H
#define SV_EXCEPTION_H

#include <QString>
#include <QException>

class SvException: public QException
{
public:

  enum ErrorType {
    NoError = 0,
    SomeError = 1
  };

  SvException() { }
  SvException(const QString& error)
  {
    this->error = error;
    this->type = error.isEmpty() ? NoError : SomeError;
  }

  virtual ~SvException() throw() { }

  void raise(QString error) { this->error = error; throw *this; }
  void raise(int code, QString error = "") { this->code = code; this->error = error; throw *this; }
  void raise() { throw *this; }

  SvException& assign(QString error = "") { this->code = code; this->error = error; return *this; }
  SvException& assign(ErrorType type, QString error = "") { this->type = type; this->code = code; this->error = error; return *this; }

  SvException *clone() const { return new SvException(*this); }


  QString error;
  int code;
  ErrorType type;

};



#endif // SV_EXCEPTION_H
