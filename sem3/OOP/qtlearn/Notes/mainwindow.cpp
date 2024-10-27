#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "notebook.h"
#include "note.h"
#include "noteslistwidget.h"

#include <QMessageBox>
#include <QPushButton>

#include <algorithm>

MainWindow::MainWindow(Notebook& notebook, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
    , notesManager(notebook)
{
    ui->setupUi(this);

    makeConnections();
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewNoteBtnClicked()
{
    notesManager.addNewNote();
}

void MainWindow::onRemoveBtnClicked()
{
    removeNote(ui->notesListWidget->currentNoteId());
}

void MainWindow::onNewNoteCreated(int index)
{
    addNoteToList(notesManager.note(index));
}

void MainWindow::onNoteContentChanged(int index)
{
    ui->notesListWidget->updateCurrentNote(notesManager.note(index));
}

void MainWindow::onSelectedNoteChanged(int index)
{
    auto* document = notesManager.noteDocument(index);
    if(document)
    {
        ui->textEdit->setDocument(document);

        auto cursor = ui->textEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textEdit->setTextCursor(cursor);
    }
}

void MainWindow::onRemoveNote(int index)
{
    removeNote(index);
}

void MainWindow::onRenameNote(int index, const QString& newTitle)
{
    notesManager.renameNote(index, newTitle);
    ui->notesListWidget->updateCurrentNote(notesManager.note(index));
}

void MainWindow::addNoteToList(const Note &note)
{
    ui->notesListWidget->addNote(note);
}

void MainWindow::removeNote(int index)
{
    QString noteTitle = notesManager.note(index).title;

    auto pressedBtn = QMessageBox::information(this, "Remove note?",
        QString("Are you sure you want to remove %0?").arg(noteTitle),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if(pressedBtn == QMessageBox::Yes)
    {
        if(notesManager.countNotes() == 1)
            ui->textEdit->setDocument(nullptr);

        ui->notesListWidget->removeCurrentNote();
        notesManager.removeNote(index);
    }
}

void MainWindow::init()
{
    auto notesList = notesManager.notebook();
    std::sort(notesList.begin(), notesList.end(),
              [](const Note& left, const Note& right)
              {
                  return left.lastModified < right.lastModified;
              });

    for(auto& note : notesList)
    {
        addNoteToList(note);
    }
}

void MainWindow::makeConnections()
{
    connect(ui->newNoteBtn, &QPushButton::clicked, this, &MainWindow::onNewNoteBtnClicked);
    connect(ui->removeNoteBtn, &QPushButton::clicked, this, &MainWindow::onRemoveBtnClicked);

    connect(&notesManager, &Notebook::newNoteCreated, this, &MainWindow::onNewNoteCreated);
    connect(&notesManager, &Notebook::noteContentChanged, this, &MainWindow::onNoteContentChanged);

    connect(ui->notesListWidget, &NotesListWidget::selectedNoteChanged, this, &MainWindow::onSelectedNoteChanged);

    connect(ui->notesListWidget, &NotesListWidget::removeNote, this, &MainWindow::onRemoveNote);
    connect(ui->notesListWidget, &NotesListWidget::renameNote, this, &MainWindow::onRenameNote);
}
