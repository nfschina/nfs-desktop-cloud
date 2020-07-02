#ifndef HELPPAGE_H
#define HELPPAGE_H

#include <QWidget>
#include<QTextBrowser>
#include "common.h"
namespace Ui {
class HelpPage;
}

class HelpPage : public QDialog
{
    Q_OBJECT

public:
    explicit HelpPage(QDialog *parent = 0);
    ~HelpPage();
public slots:
    void setUrl(QUrl);
private:
    Ui::HelpPage *ui;
    QString dir;
};

#endif // HELPPAGE_H
