#include "noteslistwidget.h"
#include "ui_noteslistwidget.h"

NotesListWidget::NotesListWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NotesListWidget)
{
    ui->setupUi(this);
}

NotesListWidget::~NotesListWidget()
{
    delete ui;
}
