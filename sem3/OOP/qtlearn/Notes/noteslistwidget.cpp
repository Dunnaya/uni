#include "noteslistwidget.h"
#include "ui_noteslistwidget.h"

#include "note.h"
#include "notewidget.h"

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
    else
    {
        auto widget = static_cast<NoteWidget*>(ui->noteListWidget->itemWidget(ui->noteListWidget->currentItem()));
        widget->UpdateContent(note);
    }
}

int NotesListWidget::currentNoteId()
{
    auto* currentItem = ui->noteListWidget->currentItem();
    int noteId = static_cast<NoteWidget*>(ui->noteListWidget->itemWidget(currentItem))->noteId();
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
    NoteWidget* widget = new NoteWidget(note);

    connect(widget, &NoteWidget::removeNote,
            this, &NotesListWidget::removeNote);

    connect(widget, &NoteWidget::renameNote,
            this, &NotesListWidget::renameNote);

    item->setSizeHint(widget->sizeHint());
    ui->noteListWidget->setItemWidget(item, widget);
    ui->noteListWidget->setCurrentItem(item);
}
