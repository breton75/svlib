#ifndef SV_UDP_EDITOR_H
#define SV_UDP_EDITOR_H

#include <QtWidgets/QDialog>
#include <QUdpSocket>
#include <QHostAddress>
#include <QMap>
#include <QMessageBox>

#include "sv_exception.h"
#include "sv_udp_params.h"


namespace Ui {
class SvUdpEditorDialog;
}


namespace sv {

  class UdpEditor : public QDialog
  {
    Q_OBJECT
    
private:
    UdpEditor(UdpParams params, const QString &label, QWidget *parent = 0);
    UdpEditor(const QString& params, const QString &label, QWidget *parent = 0);
    
    Ui::SvUdpEditorDialog *ui;
    
    UdpParams _params;
    
    void init(const QString &label = "");
    
    void accept();
    
    static UdpEditor* _instance; // = nullptr;
    
  public: 
    
    enum Result { Accepted = QDialog::Accepted, Rejected = QDialog::Rejected, Error = -1 };
    
    ~UdpEditor();
    

    /** static **/
    static int showDialog(UdpParams params, const QString &label, QWidget *parent = 0);
    static int showDialog(const QString& params, const QString &label, QWidget *parent = 0);
    static void deleteDialog();
    
    static sv::UdpParams params();
//    static QString stringParams();
   
  };
}


#endif // SV_UDP_EDITOR_H
