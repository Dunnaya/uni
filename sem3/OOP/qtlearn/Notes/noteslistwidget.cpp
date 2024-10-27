#include "noteslistwidget.h"
#include "ui_noteslistwidget.h"

#include "note.h"

NotesListWidget::NotesListWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NotesListWidget)
{
    ui->setupUi(this);

    connect(ui->noteListWidget, &QListWidget::itemSelectionChanged,
            this, &NotesListWidget::onItemSelectionChanged);
}

NotesListWidget::~NotesListWidget()
{
    delete ui;
}

void NotesListWidget::addNote(const Note &note)
{
    auto* item = new QListWidgetItem();
    ui->noteListWidget->insertItem(0, item);

    setupNoteItem(note, item);
}

void NotesListWidget::removeCurrentNote()
{
    auto* currentItem = ui->noteListWidget->currentItem();
    if(currentItem != nullptr)
    {
        delete currentItem;
    }
}

void NotesListWidget::updateCurrentNote(const Note &note)
{
    if(ui->noteListWidget->currentRow() != 0)
    {
        moveCurrentItemToTop(note);
    }
}

int NotesListWidget::currentNoteId()
{
    auto* currentItem = ui->noteListWidget->currentItem();
    int noteId = currentItem->data(Qt::UserRole).toInt();
    return noteId;
}

void NotesListWidget::onItemSelectionChanged()
{
    auto* currentItem = ui->noteListWidget->currentItem();
    if(currentItem)
    {
        int index = currentNoteId();

        emit selectedNoteChanged(index);
    }
}

void NotesListWidget::moveCurrentItemToTop(const Note &note)
{
    blockSignals(true);

    auto item = ui->noteListWidget->takeItem(ui->noteListWidget->currentRow());

    ui->noteListWidget->insertItem(0, item);
    setupNoteItem(note, item);

    blockSignals(false);
}

void NotesListWidget::setupNoteItem(const Note &note, QListWidgetItem *item)
{
    item->setText(note.title);
    item->setData(Qt::UserRole, note.index);

    ui->noteListWidget->setCurrentItem(item);
}
