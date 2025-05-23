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
    int insertPos = findInsertPosition(note);
    ui->noteListWidget->insertItem(insertPos, item);
    itemToNoteId[item] = note.index;

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
    auto* currentItem = ui->noteListWidget->currentItem();
    if (!currentItem) return;

    auto* widget = static_cast<NoteWidget*>(ui->noteListWidget->itemWidget(currentItem));

    bool needsReposition =
        (widget->getIsPinned() != note.isPinned) ||
        (!note.isPinned &&
         (note.lastModified != QDateTime::fromString(widget->getLastModified(),
                                                     "dd.MM.yyyy hh:mm:ss")));

    if (needsReposition)
    {
        int row = ui->noteListWidget->row(currentItem);
        currentItem = ui->noteListWidget->takeItem(row);

        int newPos = findInsertPosition(note);
        ui->noteListWidget->insertItem(newPos, currentItem);
        setupNoteItem(note, currentItem);
        ui->noteListWidget->setCurrentItem(currentItem);
    }
    else
        widget->UpdateContent(note);
}

int NotesListWidget::currentNoteId()
{
    auto* currentItem = ui->noteListWidget->currentItem();
    if (!currentItem) return -1;  // safety check

    int noteId = static_cast<NoteWidget*>(ui->noteListWidget->itemWidget(currentItem))->noteId();
    return noteId;
}

void NotesListWidget::togglePinStatus(int index)
{
    for (int i = 0; i < ui->noteListWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->noteListWidget->item(i);
        NoteWidget* widget = static_cast<NoteWidget*>(ui->noteListWidget->itemWidget(item));

        if (widget->noteId() == index)
        {
            emit togglePinNote(index);
            break;
        }
    }
}

void NotesListWidget::filterNotes(const QString &searchText)
{
    for (int i = 0; i < ui->noteListWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->noteListWidget->item(i);
        NoteWidget* widget = static_cast<NoteWidget*>(ui->noteListWidget->itemWidget(item));

        // case insensitive search
        bool matches = widget->getTitle().contains(searchText, Qt::CaseInsensitive);
        item->setHidden(!matches);
    }
}

void NotesListWidget::showAllNotes()
{
    for (int i = 0; i < ui->noteListWidget->count(); ++i)
    {
        ui->noteListWidget->item(i)->setHidden(false);
    }
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

    // find the correct position based on pin status
    int insertPosition = 0;
    if (!note.isPinned)
    {
        // if the note is not pinned, insert it after all pinned notes
        for (int i = 0; i < ui->noteListWidget->count(); ++i)
        {
            auto* widget = static_cast<NoteWidget*>(ui->noteListWidget->itemWidget(ui->noteListWidget->item(i)));
            if (!widget->getIsPinned())
            {
                insertPosition = i;
                break;
            }
            insertPosition = i + 1;
        }
    }

    ui->noteListWidget->insertItem(insertPosition, item);
    setupNoteItem(note, item);

    blockSignals(false);
}

void NotesListWidget::setupNoteItem(const Note &note, QListWidgetItem *item)
{
    NoteWidget* widget = new NoteWidget(note);

    connect(widget, &NoteWidget::removeNote, this, &NotesListWidget::removeNote);
    connect(widget, &NoteWidget::renameNote, this, &NotesListWidget::renameNote);
    connect(widget, &NoteWidget::togglePinNote,
            this, [this, widget]()
            {
                emit togglePinNote(widget->noteId());
            });

    item->setSizeHint(widget->sizeHint());
    ui->noteListWidget->setItemWidget(item, widget);
    ui->noteListWidget->setCurrentItem(item);
}

int NotesListWidget::findInsertPosition(const Note &note)
{
    if (note.isPinned)
    {
        for (int i = 0; i < ui->noteListWidget->count(); ++i)
        {
            auto* item = ui->noteListWidget->item(i);
            auto* widget = static_cast<NoteWidget*>(ui->noteListWidget->itemWidget(item));

            if (!widget->getIsPinned())
                return i;  // insert before first unpinned note
        }
        return ui->noteListWidget->count();  // all notes are pinned, add at end
    }
    else
    {
        // for unpinned notes, first skip all pinned notes
        int firstUnpinnedPos = 0;
        for (; firstUnpinnedPos < ui->noteListWidget->count(); ++firstUnpinnedPos)
        {
            auto* item = ui->noteListWidget->item(firstUnpinnedPos);
            auto* widget = static_cast<NoteWidget*>(ui->noteListWidget->itemWidget(item));

            if (!widget->getIsPinned())
                break;
        }

        for (int i = firstUnpinnedPos; i < ui->noteListWidget->count(); ++i)
        {
            auto* item = ui->noteListWidget->item(i);
            auto* widget = static_cast<NoteWidget*>(ui->noteListWidget->itemWidget(item));
            QDateTime itemDateTime = QDateTime::fromString(widget->getLastModified(),
                                                           "dd.MM.yyyy hh:mm:ss");

            if (note.lastModified > itemDateTime)
                return i;
        }

        return ui->noteListWidget->count();
    }
}
