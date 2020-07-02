#include "updatedialog.h"
#include "ui_updatedialog.h"

updateDialog::updateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::updateDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint);
    movie = new QMovie(":/new/index/image/loading_update.gif");
    movie->setScaledSize(QSize(240,240));
    ui->update_label->setMovie(movie);
    movie->start();
    m_pTimer = new QTimer(this);
    m_pTimer->setSingleShot(true);
    connect(m_pTimer, &QTimer::timeout, this, [=](){this->close();});
}

updateDialog::~updateDialog()
{
    delete ui;
}

void updateDialog::update_start_timer()
{
    m_pTimer->start(5000);

}
