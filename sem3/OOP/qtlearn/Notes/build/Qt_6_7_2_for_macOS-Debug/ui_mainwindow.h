/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "noteslistwidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *newNoteBtn;
    QPushButton *removeNote;
    QSpacerItem *horizontalSpacer;
    QSplitter *splitter;
    NotesListWidget *notesListWidget;
    QTextEdit *textEdit;

    void setupUi(QWidget *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        verticalLayout = new QVBoxLayout(MainWindow);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(10, 5, 10, 10);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(-1, -1, -1, 0);
        newNoteBtn = new QPushButton(MainWindow);
        newNoteBtn->setObjectName("newNoteBtn");
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/create.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        newNoteBtn->setIcon(icon);

        horizontalLayout->addWidget(newNoteBtn);

        removeNote = new QPushButton(MainWindow);
        removeNote->setObjectName("removeNote");
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/bin.svg"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        removeNote->setIcon(icon1);

        horizontalLayout->addWidget(removeNote);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        splitter = new QSplitter(MainWindow);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Orientation::Horizontal);
        splitter->setChildrenCollapsible(false);
        notesListWidget = new NotesListWidget(splitter);
        notesListWidget->setObjectName("notesListWidget");
        notesListWidget->setMinimumSize(QSize(200, 0));
        splitter->addWidget(notesListWidget);
        textEdit = new QTextEdit(splitter);
        textEdit->setObjectName("textEdit");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
        textEdit->setSizePolicy(sizePolicy);
        textEdit->setMinimumSize(QSize(360, 0));
        textEdit->setFrameShape(QFrame::Shape::NoFrame);
        textEdit->setFrameShadow(QFrame::Shadow::Sunken);
        splitter->addWidget(textEdit);

        verticalLayout->addWidget(splitter);

        verticalLayout->setStretch(1, 1);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QWidget *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        newNoteBtn->setText(QString());
        removeNote->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
