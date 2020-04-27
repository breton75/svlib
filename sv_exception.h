#ifndef SV_EXCEPTION_H
#define SV_EXCEPTION_H

#include <QString>
#include <QException>

class SvException: public QException
{
public:
    void raise(QString error) { this->error = error; throw *this; }
    void raise(int code, QString error = "") { this->code = code; this->error = error; throw *this; }
    SvException& assign(QString error = "") { this->code = code; this->error = error; return *this; }
    SvException *clone() const { return new SvException(*this); }
    virtual ~SvException() throw() { }
    
    QString error;
    int code;

};



#endif // SV_EXCEPTION_H
