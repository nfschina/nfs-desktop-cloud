#include "detaildialog.h"
#include "ui_detaildialog.h"

DetailDialog::DetailDialog(VM_CONFIG vm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DetailDialog)
{
    ui->setupUi(this);
    setStyleSheet("QPushButton { \
                  border: 1px solid white;\
                  border-radius: 0px;\
                  color:white;\
                  font-weight:bold;\
                  background-color:#50a495;\
                  min-width:70px;\
              }\
              QPushButton:hover {\
                  border: 1px solid white;\
                  border-radius: 0px;\
                  color:white;\
                  font-weight:normal;\
                  background-color:#2a564e;\
              }\
              QPushButton:pressed {\
                  border: 1px solid white;\
                  border-radius: 0px;\
                  color:white;\
                  font-weight:bold;\
                  background-color:#50a495;\
              }");
    ui->nameEdit->setText(vm.name);
    ui->vIDEdit->setText(vm.vid);
    ui->IPEdit->setText(vm.ip);
    ui->portEdit->setText(QString::number(vm.port));
    ui->statusEdit->setText(QString::number(vm.status));
    ui->createEdit->setText(vm.created);
}

DetailDialog::~DetailDialog()
{
    delete ui;
}

void DetailDialog::on_okButton_clicked()
{
    accept();
}
