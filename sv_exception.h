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

  void raise(QString error) { this->error = error; throw *this; }
  void raise(int code, QString error = "") { this->code = code; this->error = error; throw *this; }
  void raise() { throw *this; }

  SvException& assign(QString error = "") { this->code = code; this->error = error; return *this; }
  SvException& assign(ErrorType type, QString error = "") { this->type = type; this->code = code; this->error = error; return *this; }

  SvException *clone() const { return new SvException(*this); }

  virtual ~SvException() throw() { }

  QString error;
  int code;
  ErrorType type;

};



#endif // SV_EXCEPTION_H
