/********************************************************************************
** Form generated from reading UI file 'csfwin.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CSFWIN_H
#define UI_CSFWIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_csfwin
{
public:
    QPushButton *para_push;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *i_rigidness;
    QLineEdit *i_timestep;
    QLabel *label_3;
    QLineEdit *i_iter;

    void setupUi(QWidget *csfwin)
    {
        if (csfwin->objectName().isEmpty())
            csfwin->setObjectName(QStringLiteral("csfwin"));
        csfwin->resize(342, 230);
        para_push = new QPushButton(csfwin);
        para_push->setObjectName(QStringLiteral("para_push"));
        para_push->setGeometry(QRect(120, 170, 111, 21));
        label = new QLabel(csfwin);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(43, 40, 61, 20));
        QFont font;
        font.setFamily(QStringLiteral("Algerian"));
        font.setPointSize(11);
        label->setFont(font);
        label_2 = new QLabel(csfwin);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(43, 80, 61, 20));
        label_2->setFont(font);
        i_rigidness = new QLineEdit(csfwin);
        i_rigidness->setObjectName(QStringLiteral("i_rigidness"));
        i_rigidness->setGeometry(QRect(120, 40, 113, 20));
        i_timestep = new QLineEdit(csfwin);
        i_timestep->setObjectName(QStringLiteral("i_timestep"));
        i_timestep->setGeometry(QRect(120, 80, 113, 20));
        label_3 = new QLabel(csfwin);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(40, 120, 61, 20));
        label_3->setFont(font);
        i_iter = new QLineEdit(csfwin);
        i_iter->setObjectName(QStringLiteral("i_iter"));
        i_iter->setGeometry(QRect(120, 120, 113, 20));

        retranslateUi(csfwin);

        QMetaObject::connectSlotsByName(csfwin);
    } // setupUi

    void retranslateUi(QWidget *csfwin)
    {
        csfwin->setWindowTitle(QApplication::translate("csfwin", "\346\217\220\345\217\226\345\234\260\345\275\242\347\202\271", Q_NULLPTR));
        para_push->setText(QApplication::translate("csfwin", "\347\241\256\350\256\244", Q_NULLPTR));
        label->setText(QApplication::translate("csfwin", "\345\270\203\346\226\231\345\210\232\345\272\246", Q_NULLPTR));
        label_2->setText(QApplication::translate("csfwin", "\351\207\215\345\212\233\345\217\202\346\225\260", Q_NULLPTR));
        label_3->setText(QApplication::translate("csfwin", "\350\277\255\344\273\243\346\254\241\346\225\260", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class csfwin: public Ui_csfwin {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CSFWIN_H
