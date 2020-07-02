#include "helppage.h"
#include "ui_helppage.h"
#include <QHBoxLayout>
#include<QtDebug>
HelpPage::HelpPage(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::HelpPage)
{
    ui->setupUi(this);
   // dir = "/home/nfschina/cdos/cdos-desktop-cloud-0.1/src/help/";
    dir = "/etc/cdos-desktop-cloud.d/help/";
    QString urlName=dir+"cdos-desktop-helper_a.htm";
    QString urlName1=dir+"cdos-desktop-helper.htm";
    ui->muluBrowser->setOpenLinks(false);
    ui->muluBrowser->setSource(urlName);
    ui->contentBrowser->setSource(urlName1);
    QStringList paths;
    paths<<dir;
    ui->contentBrowser->setSearchPaths(paths);
    connect(ui->muluBrowser, SIGNAL(anchorClicked(const QUrl &)),ui->contentBrowser, SLOT(setSource(QUrl)));
  //  connect(ui->muluBrowser, SIGNAL(anchorClicked(const QUrl &)),this, SLOT(setUrl(QUrl)));
}

void HelpPage::setUrl(QUrl url)
{
    //qDebug()<<url.url();
    QString urlName =dir+url.url();
    ui->contentBrowser->setSource(QUrl(urlName));
}

HelpPage::~HelpPage()
{
    delete ui;
}
