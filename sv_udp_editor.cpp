#include "sv_udp_editor.h"
#include "ui_sv_udp_editor.h"

using namespace sv;

UdpEditor* UDPEDITOR_UI = nullptr;
UdpEditor* UdpEditor::_instance = nullptr;

UdpEditor::UdpEditor(UdpParams params, const QString& label, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SvUdpEditorDialog)
{
  ui->setupUi(this);
  _params = params;
  
  init(label);
  
}

UdpEditor::UdpEditor(const QString& params, const QString& label, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SvUdpEditorDialog)
{
  ui->setupUi(this);
  
  _params = UdpParams::fromJsonString(params);
    
  init(label);
  
}

void UdpEditor::init(const QString& label)
{
  ui->lineHost->setText(_params.host);
  ui->spinListenPort->setValue(_params.recv_port);
  ui->spinRemotePort->setValue(_params.send_port);
  
  ui->lblCaption->setText(label);
    
  connect(ui->bnSave, SIGNAL(clicked()), this, SLOT(accept()));
  connect(ui->bnCancel, SIGNAL(clicked()), this, SLOT(reject()));
  
  setModal(true);
  
}

UdpEditor::~UdpEditor()
{
  delete ui;
}

void UdpEditor::accept()
{
  if(QHostAddress(ui->lineHost->text()).isNull())
  {
    QMessageBox::critical(this, "Ошибка", "Неверный адрес хоста");
    return;
  }

  _params.host = ui->lineHost->text();
  _params.recv_port = ui->spinListenPort->value();
  _params.send_port = ui->spinRemotePort->value();
  
  QDialog::accept();
    
}

int UdpEditor::showDialog(UdpParams params, const QString& label, QWidget *parent)
{
  _instance = new UdpEditor(params, label, parent);
  return  _instance->exec();
}

int UdpEditor::showDialog(const QString &params, const QString &label, QWidget *parent)
{
  _instance = new UdpEditor(params, label, parent);
  return  _instance->exec();
}

void UdpEditor::deleteDialog()
{
  if(_instance)
    delete _instance;
  
  _instance = nullptr;
  
}

UdpParams UdpEditor::params()
{
  UdpParams p;

  if(_instance)
    p = _instance->_params;

  return p;
}

