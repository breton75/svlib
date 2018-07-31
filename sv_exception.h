#ifndef SV_EXCEPTION_H
#define SV_EXCEPTION_H

#include <QString>
#include <QException>

class SvException: public QException
{
public:
    void raise(QString error) { this->error = error; throw *this; }
    SvException *clone() const { return new SvException(*this); }

    QString error;

};



#endif // SV_EXCEPTION_H
