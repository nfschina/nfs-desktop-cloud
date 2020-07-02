#ifndef WAITDIALOG_H
#define WAITDIALOG_H
#include <QDialog>
#include <QLabel>
#include<QPainter>
#include<QMovie>

class WaitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitDialog(QWidget *parent = 0);
    ~WaitDialog();

private:
    QMovie *movie;
    QLabel *label;
};
#endif // WAITDIALOG_H
