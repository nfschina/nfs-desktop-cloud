/********************************************************************************
** Form generated from reading UI file 'homepage.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HOMEPAGE_H
#define UI_HOMEPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_helpPage
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QSplitter *splitter;
    QTextBrowser *muluBrowser;
    QTextBrowser *contentBrowser;

    void setupUi(QDialog *helpPage)
    {
        if (helpPage->objectName().isEmpty())
            helpPage->setObjectName(QStringLiteral("helpPage"));
        helpPage->resize(601, 434);
        gridLayout = new QGridLayout(helpPage);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        groupBox = new QGroupBox(helpPage);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        splitter = new QSplitter(groupBox);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        muluBrowser = new QTextBrowser(splitter);
        muluBrowser->setObjectName(QStringLiteral("muluBrowser"));
        muluBrowser->setMaximumSize(QSize(200, 16777215));
        splitter->addWidget(muluBrowser);
        contentBrowser = new QTextBrowser(splitter);
        contentBrowser->setObjectName(QStringLiteral("contentBrowser"));
        splitter->addWidget(contentBrowser);

        gridLayout_2->addWidget(splitter, 0, 0, 1, 1);


        gridLayout->addWidget(groupBox, 0, 0, 1, 1);


        retranslateUi(helpPage);

        QMetaObject::connectSlotsByName(helpPage);
    } // setupUi

    void retranslateUi(QDialog *helpPage)
    {
        helpPage->setWindowTitle(QApplication::translate("helpPage", "Dialog", 0));
        groupBox->setTitle(QApplication::translate("helpPage", "\345\270\256\345\212\251\344\277\241\346\201\257", 0));
    } // retranslateUi

};

namespace Ui {
    class helpPage: public Ui_helpPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HOMEPAGE_H
