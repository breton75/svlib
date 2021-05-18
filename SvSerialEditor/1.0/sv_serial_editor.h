#ifndef SV_SERIAL_EDITOR_H
#define SV_SERIAL_EDITOR_H

#include <QtWidgets/QDialog>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMap>
#include <QMessageBox>

#include "sv_exception.h"
#include "sv_serial_params.h"


namespace Ui {
class SvSerialEditorDialog;
}


namespace sv {

  class SerialEditor : public QDialog
  {
    Q_OBJECT
    
private:
    SerialEditor(sv::SerialParams params, const QString &label, QWidget *parent = 0);
    SerialEditor(const QString& params, const QString &label, QWidget *parent = 0);
    
    Ui::SvSerialEditorDialog *ui;
    
    SerialParams _params;
    
    void init(const QString &label = "");
    
    void accept();
    
    static SerialEditor* _instance; // = nullptr;
    
  public: 
    
    enum Result { Accepted = QDialog::Accepted, Rejected = QDialog::Rejected, Error = -1 };
    
    ~SerialEditor();
    

    /** static **/
    static int showDialog(SerialParams params, const QString &label, QWidget *parent = 0);
    static int showDialog(const QString& params, const QString &label, QWidget *parent = 0);
    static void deleteDialog();
    
    static sv::SerialParams params();
//    static QString stringParams();
   
  };
}


#endif // SV_SERIAL_EDITOR_H
