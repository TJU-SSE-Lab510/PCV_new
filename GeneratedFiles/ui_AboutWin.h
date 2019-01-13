/********************************************************************************
** Form generated from reading UI file 'AboutWin.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTWIN_H
#define UI_ABOUTWIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AboutWin
{
public:
    QLabel *label;
    QLabel *label_2;

    void setupUi(QWidget *AboutWin)
    {
        if (AboutWin->objectName().isEmpty())
            AboutWin->setObjectName(QStringLiteral("AboutWin"));
        AboutWin->resize(480, 360);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(AboutWin->sizePolicy().hasHeightForWidth());
        AboutWin->setSizePolicy(sizePolicy);
        AboutWin->setMinimumSize(QSize(480, 360));
        AboutWin->setMaximumSize(QSize(480, 360));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        AboutWin->setFont(font);
        AboutWin->setMouseTracking(false);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/images/about.png"), QSize(), QIcon::Normal, QIcon::Off);
        AboutWin->setWindowIcon(icon);
        label = new QLabel(AboutWin);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 80, 72, 72));
        label->setPixmap(QPixmap(QString::fromUtf8(":/Resources/images/icon.png")));
        label_2 = new QLabel(AboutWin);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(150, 110, 141, 41));
        label_2->setTextFormat(Qt::AutoText);

        retranslateUi(AboutWin);

        QMetaObject::connectSlotsByName(AboutWin);
    } // setupUi

    void retranslateUi(QWidget *AboutWin)
    {
        AboutWin->setWindowTitle(QApplication::translate("AboutWin", "About CloudViewer", Q_NULLPTR));
        label->setText(QString());
        label_2->setText(QApplication::translate("AboutWin", "<html><head/><body><p><span style=\" font-size:14pt; font-weight:600;\">CloudViewer</span></p></body></html>", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class AboutWin: public Ui_AboutWin {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTWIN_H
