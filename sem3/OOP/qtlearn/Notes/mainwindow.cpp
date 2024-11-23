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

    setupThemeButton();
    setTheme(false);
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
    if(index >= 0)
    {
        removeNote(index);
    }
}

void MainWindow::onRenameNote(int index, const QString& newTitle)
{
    notesManager.renameNote(index, newTitle);
    ui->notesListWidget->updateCurrentNote(notesManager.note(index));
}

void MainWindow::onTogglePinNote()
{
    int currentId = ui->notesListWidget->currentNoteId();
    if (currentId >= 0)
        ui->notesListWidget->togglePinStatus(currentId);
}

void MainWindow::onTogglePinRequested(int index)
{
    notesManager.toggleNotePin(index);
    ui->notesListWidget->updateCurrentNote(notesManager.note(index));
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    if (text.isEmpty())
        ui->notesListWidget->showAllNotes();
    else
        ui->notesListWidget->filterNotes(text);
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

void MainWindow::setTheme(bool isDark)
{
    QString styleSheet;

    if (isDark) {
        styleSheet = QString(R"(
            QWidget {
                background-color: #2D2D2D;
                color: white;
            }
            QPushButton {
                background-color: #262626;
                border: 1px solid #555555;
                padding: 5px;
                border-radius: 3px;
                color: white;
            }
            QPushButton:hover {
                background-color: #505050;
            }
            QTextEdit {
                background-color: #3D3D3D;
                color: white;
                border: 1px solid #555555;
            }
            QListWidget {
                background-color: #3D3D3D;
                color: white;
                border: 1px solid #555555;
            }
            QMessageBox {
                background-color: #2D2D2D;
                color: white;
            }
            QLineEdit {
                background-color: #3D3D3D;
                color: white;
                border: 1px solid #555555;
                padding: 3px;
                border-radius: 2px;
            }
            QMenu {
                background-color: #2D2D2D;
                color: white;
                border: 1px solid #555555;
            }
            QMenu::item:selected {
                background-color: #404040;
            }
        )");
    } else {
        styleSheet = QString(R"(
            QWidget {
                background-color: #FFF0F5;  /* background */
                color: #333333;
            }
            QPushButton {
                background-color: #FFB6C1; /* buttons */
                border: 1px solid #DDA0DD;
                padding: 5px;
                border-radius: 3px;
                color: #333333;
            }
            QPushButton:hover {
                background-color: #FFC0CB;
            }
            QTextEdit {
                background-color: white;
                color: #333333;
                border: 1px solid #DDA0DD;
            }
            QListWidget {
                background-color: #FFE4E1;  /* list */
                color: #333333;
                border: 1px solid #DDA0DD;
            }
            QMessageBox {
                background-color: #FFF0F5;
                color: #333333;
            }
            QLineEdit {
                background-color: white;
                color: #333333;
                border: 1px solid #DDA0DD;
                padding: 3px;
                border-radius: 2px;
            }
            QMenu {
                background-color: #FFF0F5;
                color: #333333;
                border: 1px solid #DDA0DD;
            }
            QMenu::item:selected {
                background-color: #FFB6C1;
            }
        )");
    }

    qApp->setStyleSheet(styleSheet);
}

void MainWindow::setupThemeButton()
{
    ui->changeThemeBtn->setIcon(QIcon(":/icons/dark.svg"));
    setTheme(isDarkTheme);

    connect(ui->changeThemeBtn, &QPushButton::clicked, this, [this]() {
        isDarkTheme = !isDarkTheme;
        setTheme(isDarkTheme);
        ui->changeThemeBtn->setIcon(QIcon(isDarkTheme ? ":/icons/light.svg" : ":/icons/dark.svg"));
    });
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
    connect(ui->notesListWidget, &NotesListWidget::togglePinNote, this, &MainWindow::onTogglePinRequested);

    connect(ui->serchByNameLine, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
}
