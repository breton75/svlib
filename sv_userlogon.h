#ifndef SV_USERLOGON_H
#define SV_USERLOGON_H

#include <QDialog>
#include <QDebug>

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

#include <QString>

#include "sv_pgdb.h"
#include "sv_settings.h"
#include "sv_log.h"

namespace Ui {
  class Ui_Dialog;
}

namespace sv {

  class SvUserLogon : public QDialog
  {
      Q_OBJECT
    
      enum DialogCode { Rejected, Error, Accepted };
      
      explicit SvUserLogon(QWidget *parent,
                           bool showHost,
                           bool showPort,
                           bool showDBNamealse,
                           bool showUser,
                           bool showPass,
                           bool showLog,
                           bool showRole,
                           QString host,
                           quint16 port,
                           QString dbName,
                           QString user,
                           QString pass,
                           QString role);
      ~SvUserLogon();
      
      Ui::Ui_Dialog *ui;
      
      int connectionErrorCount = 1;
      int maxConnectionErrorCount = 5;
      
      static SvUserLogon* p_instance;
      
  private slots:
    void on_accept();
    
  public:
    
    static int showDbConnectDialog();
    
    static int showDbConnectDialog(QWidget *parent,
                      bool showHost,
                      bool showPort,
                      bool showDBName,
                      bool showUser,
                      bool showPass,
                      bool showLog,
                      bool showRole,
                      QString host,
                      quint16 port,
                      QString dbName,
                      QString user,
                      QString pass,
                      QString role);
    
    static void deleteDbConnectDialog();
    
  };
}

namespace Ui {
  class Ui_Dialog
  {
    private:
      int wHeight = 210;
      int lHeight = 24;
    
    public:
      QVBoxLayout *layoutMain;
      QGroupBox *groupBox;
      QVBoxLayout *verticalLayout;
      QHBoxLayout *layoutHost;
      QLabel *lblHost;
      QLineEdit *editHost;
      QHBoxLayout *layoutPort;
      QLabel *lblPort;
      QLineEdit *editPort;
      QHBoxLayout *layoutDBName;
      QLabel *lblDbName;
      QLineEdit *editDbName;
      QHBoxLayout *layoutUser;
      QLabel *lblUser;
      QLineEdit *editUser;
      QHBoxLayout *layoutPass;
      QLabel *lblPass;
      QLineEdit *editPass;
      QHBoxLayout *layoutButtons;
      QSpacerItem *horizontalSpacer;
      QPushButton *bnConnect;
      QPushButton *bnCancel;
      QTextEdit *textEdit;
      QHBoxLayout *hlayRole;
      QLabel *lblRole;
      QLineEdit *editRole;
  
      int connectionErrorCount = 0;
      
      void ShowDialog(QDialog *Dialog,
                      bool showHost = true,
                      bool showPort = true,
                      bool showDBName = true,
                      bool showUser = true,
                      bool showPass = true,
                      bool showLog = false,
                      QString host = "localhost",
                      quint16 port = 5432,
                      QString dbName = "postgres",
                      QString user = "postgres",
                      QString pass = "",
                      bool showRole = false,
                      QString role = "postgres")
      {
          if (Dialog->objectName().isEmpty())
              Dialog->setObjectName(QStringLiteral("Dialog"));
  //        Dialog->resize(275, 210);
  //        Dialog->setMinimumSize(QSize(275, 210));
  //        Dialog->setMaximumSize(QSize(275, 16777215));
          QIcon icon;
          icon.addFile(QStringLiteral(":/connection_state/Common/Icons/16x16freeapplicationicons/ico/Database.ico"), QSize(), QIcon::Normal, QIcon::Off);
          Dialog->setWindowIcon(icon);
          layoutMain = new QVBoxLayout(Dialog);
          layoutMain->setObjectName(QStringLiteral("layoutMain"));
          groupBox = new QGroupBox(Dialog);
          groupBox->setObjectName(QStringLiteral("groupBox"));
          verticalLayout = new QVBoxLayout(groupBox);
          verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
  
          
          /* --------- HOST ----------- */        
          layoutHost = new QHBoxLayout();
          layoutHost->setObjectName(QStringLiteral("layoutHost"));
  
          lblHost = new QLabel(groupBox);
          lblHost->setObjectName(QStringLiteral("lblHost"));
          QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
          sizePolicy.setHorizontalStretch(0);
          sizePolicy.setVerticalStretch(0);
          sizePolicy.setHeightForWidth(lblHost->sizePolicy().hasHeightForWidth());
          lblHost->setSizePolicy(sizePolicy);
          lblHost->setMinimumSize(QSize(45, 0));
          QFont font;
          font.setBold(false);
          font.setWeight(50);
          lblHost->setFont(font);
          lblHost->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  
          layoutHost->addWidget(lblHost);
  
          editHost = new QLineEdit(groupBox);
          editHost->setObjectName(QStringLiteral("editHost"));
          QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
          sizePolicy1.setHorizontalStretch(0);
          sizePolicy1.setVerticalStretch(0);
          sizePolicy1.setHeightForWidth(editHost->sizePolicy().hasHeightForWidth());
          editHost->setSizePolicy(sizePolicy1);
          editHost->setFont(font);
  
          layoutHost->addWidget(editHost);
  
          if(showHost) verticalLayout->addLayout(layoutHost);
          else
          {
            lblHost->hide();
            editHost->hide();
            wHeight = wHeight - lHeight;
          }
  
  
          /* ---------- PORT ------------ */
          layoutPort = new QHBoxLayout();
          layoutPort->setObjectName(QStringLiteral("layoutPort"));
  
          lblPort = new QLabel(groupBox);
          lblPort->setObjectName(QStringLiteral("lblPort"));
          sizePolicy.setHeightForWidth(lblPort->sizePolicy().hasHeightForWidth());
          lblPort->setSizePolicy(sizePolicy);
          lblPort->setMinimumSize(QSize(45, 0));
          lblPort->setFont(font);
          lblPort->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  
          layoutPort->addWidget(lblPort);
  
          editPort = new QLineEdit(groupBox);
          editPort->setObjectName(QStringLiteral("editPort"));
          sizePolicy1.setHeightForWidth(editPort->sizePolicy().hasHeightForWidth());
          editPort->setSizePolicy(sizePolicy1);
          editPort->setFont(font);
  
          layoutPort->addWidget(editPort);
  
          if(showPort) verticalLayout->addLayout(layoutPort);
          else
          {
            lblPort->hide();
            editPort->hide();
            wHeight = wHeight - lHeight;
          }
  
          
          /* --------- DB Name -------------- */
          layoutDBName = new QHBoxLayout();
          layoutDBName->setObjectName(QStringLiteral("layoutDBName"));
          
          lblDbName = new QLabel(groupBox);
          lblDbName->setObjectName(QStringLiteral("lblDbName"));
          sizePolicy.setHeightForWidth(lblDbName->sizePolicy().hasHeightForWidth());
          lblDbName->setSizePolicy(sizePolicy);
          lblDbName->setMinimumSize(QSize(45, 0));
          lblDbName->setFont(font);
          lblDbName->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  
          layoutDBName->addWidget(lblDbName);
  
          editDbName = new QLineEdit(groupBox);
          editDbName->setObjectName(QStringLiteral("editDbName"));
          sizePolicy1.setHeightForWidth(editDbName->sizePolicy().hasHeightForWidth());
          editDbName->setSizePolicy(sizePolicy1);
          editDbName->setMinimumSize(QSize(110, 0));
          editDbName->setFont(font);
  
          layoutDBName->addWidget(editDbName);
  
          if(showDBName) verticalLayout->addLayout(layoutDBName);
          else
          {
            lblDbName->hide();
            editDbName->hide();
            wHeight = wHeight - lHeight;
          }
  
          
          /* -------------- USER ------------- */
          layoutUser = new QHBoxLayout();
          layoutUser->setObjectName(QStringLiteral("layoutUser"));
          lblUser = new QLabel(groupBox);
          lblUser->setObjectName(QStringLiteral("lblUser"));
          sizePolicy.setHeightForWidth(lblUser->sizePolicy().hasHeightForWidth());
          lblUser->setSizePolicy(sizePolicy);
          lblUser->setMinimumSize(QSize(45, 0));
          lblUser->setFont(font);
          lblUser->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  
          layoutUser->addWidget(lblUser);
  
          editUser = new QLineEdit(groupBox);
          editUser->setObjectName(QStringLiteral("editUser"));
          sizePolicy1.setHeightForWidth(editUser->sizePolicy().hasHeightForWidth());
          editUser->setSizePolicy(sizePolicy1);
          editUser->setMinimumSize(QSize(110, 0));
          editUser->setFont(font);
  
          layoutUser->addWidget(editUser);
  
          if(showUser) verticalLayout->addLayout(layoutUser);
          else
          {
            lblUser->hide();
            editUser->hide();
            wHeight = wHeight - lHeight;
          }
  
          
          /* --------------- PASS ------------- */
          layoutPass = new QHBoxLayout();
          layoutPass->setObjectName(QStringLiteral("layoutPass"));
          lblPass = new QLabel(groupBox);
          lblPass->setObjectName(QStringLiteral("lblPass"));
          sizePolicy.setHeightForWidth(lblPass->sizePolicy().hasHeightForWidth());
          lblPass->setSizePolicy(sizePolicy);
          lblPass->setMinimumSize(QSize(45, 0));
          lblPass->setFont(font);
          lblPass->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  
          layoutPass->addWidget(lblPass);
  
          editPass = new QLineEdit(groupBox);
          editPass->setObjectName(QStringLiteral("editPass"));
          sizePolicy1.setHeightForWidth(editPass->sizePolicy().hasHeightForWidth());
          editPass->setSizePolicy(sizePolicy1);
          editPass->setFont(font);
          editPass->setEchoMode(QLineEdit::Password);
  
          layoutPass->addWidget(editPass);
  
          if(showPass) verticalLayout->addLayout(layoutPass);
          else
          {
            lblPass->hide();
            editPass->hide();
            wHeight = wHeight - lHeight;
          }
          
          /* --------------- ROLE ------------- */
          hlayRole = new QHBoxLayout();
          hlayRole->setObjectName(QStringLiteral("hlayRole"));
          lblRole = new QLabel(groupBox);
          lblRole->setObjectName(QStringLiteral("lblRole"));
          sizePolicy.setHeightForWidth(lblRole->sizePolicy().hasHeightForWidth());
          lblRole->setSizePolicy(sizePolicy);
          lblRole->setMinimumSize(QSize(45, 0));
          lblRole->setFont(font);
          lblRole->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
  
          hlayRole->addWidget(lblRole);
  
          editRole = new QLineEdit(groupBox);
          editRole->setObjectName(QStringLiteral("editRole"));
          sizePolicy1.setHeightForWidth(editRole->sizePolicy().hasHeightForWidth());
          editRole->setSizePolicy(sizePolicy1);
          editRole->setMinimumSize(QSize(110, 0));
          editRole->setFont(font);
  
          hlayRole->addWidget(editRole);
  
          if(showRole) verticalLayout->addLayout(hlayRole);
          else
          {
            lblRole->hide();
            editRole->hide();
            wHeight = wHeight - lHeight;
          }
 
          
          Dialog->resize(275, wHeight+100);
          Dialog->setMinimumSize(QSize(275, wHeight));
          Dialog->setMaximumSize(QSize(275, wHeight));
  
          /* ---------- BUTTONS ----------- */
          layoutMain->addWidget(groupBox);
  
          layoutButtons = new QHBoxLayout();
          layoutButtons->setObjectName(QStringLiteral("layoutButtons"));
          horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  
          layoutButtons->addItem(horizontalSpacer);
  
          bnConnect = new QPushButton(Dialog);
          bnConnect->setObjectName(QStringLiteral("bnConnect"));
  
          layoutButtons->addWidget(bnConnect);
  
          bnCancel = new QPushButton(Dialog);
          bnCancel->setObjectName(QStringLiteral("bnCancel"));
  
          layoutButtons->addWidget(bnCancel);
          layoutMain->addLayout(layoutButtons);
  
          /* ----------- LOG ---------------*/
          textEdit = new QTextEdit(Dialog);
          textEdit->setObjectName(QStringLiteral("textEdit"));
          QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
          sizePolicy2.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
          textEdit->setSizePolicy(sizePolicy2);
          textEdit->setMinimumSize(QSize(0, 10));
          textEdit->setReadOnly(true);
  
          layoutMain->addWidget(textEdit);
          if(!showLog) textEdit->hide();
          else this->showLog(Dialog);
  
          /* --------------------------------- */
          editHost->setText(host);
          editUser->setText(user);
          editPass->setText(pass);
          editDbName->setText(dbName);
          editPort->setText(QString("%1").arg(port));
          editRole->setText(role);
  
          retranslateUi(Dialog);
  
          QMetaObject::connectSlotsByName(Dialog);
      } // setupUi
  
      void retranslateUi(QDialog *Dialog)
      {
          Dialog->setWindowTitle(QApplication::translate("Dialog", "System logon", 0));
          groupBox->setTitle(QString());
          lblHost->setText(QApplication::translate("Dialog", "Host", 0));
          lblUser->setText(QApplication::translate("Dialog", "User", 0));
          lblPass->setText(QApplication::translate("Dialog", "Pass", 0));
          lblPort->setText(QApplication::translate("Dialog", "Port", 0));
          lblDbName->setText(QApplication::translate("Dialog", "DB name", 0));
          bnConnect->setText(QApplication::translate("Dialog", "Connect", 0));
          bnCancel->setText(QApplication::translate("Dialog", "Cancel", 0));
          lblRole->setText(QApplication::translate("Dialog", "Role", 0));
      } // retranslateUi
      
      void showLog(QDialog *Dialog, int h = 100)
      {
        textEdit->show();
        Dialog->setMaximumSize(QSize(275, 16777215));
        Dialog->setMinimumHeight(wHeight + 10);
        Dialog->resize(275, wHeight + h);
      }
  };
  
}

#endif // USERLOGON_H
