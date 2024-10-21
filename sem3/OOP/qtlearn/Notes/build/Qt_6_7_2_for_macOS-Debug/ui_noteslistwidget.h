/********************************************************************************
** Form generated from reading UI file 'noteslistwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NOTESLISTWIDGET_H
#define UI_NOTESLISTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NotesListWidget
{
public:
    QHBoxLayout *horizontalLayout;
    QListWidget *listWidget;

    void setupUi(QWidget *NotesListWidget)
    {
        if (NotesListWidget->objectName().isEmpty())
            NotesListWidget->setObjectName("NotesListWidget");
        NotesListWidget->resize(302, 524);
        horizontalLayout = new QHBoxLayout(NotesListWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        listWidget = new QListWidget(NotesListWidget);
        listWidget->setObjectName("listWidget");
        listWidget->setFrameShape(QFrame::Shape::NoFrame);

        horizontalLayout->addWidget(listWidget);


        retranslateUi(NotesListWidget);

        QMetaObject::connectSlotsByName(NotesListWidget);
    } // setupUi

    void retranslateUi(QWidget *NotesListWidget)
    {
        NotesListWidget->setWindowTitle(QCoreApplication::translate("NotesListWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NotesListWidget: public Ui_NotesListWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NOTESLISTWIDGET_H
