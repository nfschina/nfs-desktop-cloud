#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>
#include <QMovie>
#include <QTimer>
namespace Ui {
class updateDialog;
}

class updateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit updateDialog(QWidget *parent = 0);
    ~updateDialog();
   void update_start_timer();

private:
    Ui::updateDialog *ui;
    QMovie *movie;
    QTimer *m_pTimer;

};

#endif // UPDATEDIALOG_H
