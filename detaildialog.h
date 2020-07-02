#ifndef DETAILDIALOG_H
#define DETAILDIALOG_H

#include <QDialog>
#include "common.h"

namespace Ui {
class DetailDialog;
}

class DetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DetailDialog(VM_CONFIG vm, QWidget *parent = 0);
    ~DetailDialog();

private slots:
    void on_okButton_clicked();

private:
    Ui::DetailDialog *ui;
};

#endif // DETAILDIALOG_H
