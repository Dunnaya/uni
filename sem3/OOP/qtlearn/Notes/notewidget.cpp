#include "notewidget.h"
#include "note.h"
#include "renamedialog.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QContextMenuEvent>
#include <QMenu>

NoteWidget::NoteWidget(const Note& note, QWidget *parent)
    : QWidget{parent}
{
    setupGui();

    index = note.index;
    title = note.title;
    lastModified = note.lastModified.toString(dateTimeFormat);
    isPinned = note.isPinned;
    content = note.content;

    updateLabels();
    updatePinDisplay();
    setToolTip(QString("%0\n%1").arg(title).arg(lastModified));
}

int NoteWidget::noteId() const
{
    return index;
}

void NoteWidget::UpdateContent(const Note &note)
{
    title = note.title;
    lastModified = note.lastModified.toString(dateTimeFormat);
    isPinned = note.isPinned;

    updateLabels();
    updatePinDisplay();
    setToolTip(QString("%0\n%1").arg(title).arg(lastModified));
}

void NoteWidget::updatePinDisplay()
{
    if (isPinned)
        titleLbl->setStyleSheet("font-weight: bold; color: #c24887;");
    else
        titleLbl->setStyleSheet("");
}

void NoteWidget::resizeEvent(QResizeEvent *event)
{
    updateLabels();
    QWidget::resizeEvent(event);
}

void NoteWidget::contextMenuEvent(QContextMenuEvent *event)
{
    showContextMenu(event->globalPos());
}

void NoteWidget::setupGui()
{
    titleLbl = new QLabel();
    lastModifiedLbl = new QLabel();

    titleLbl->setAttribute(Qt::WA_TranslucentBackground);
    lastModifiedLbl->setAttribute(Qt::WA_TranslucentBackground);

    auto layout = new QVBoxLayout(this);
    layout->setSpacing(2);
    layout->addWidget(titleLbl);
    layout->addWidget(lastModifiedLbl);
}

void NoteWidget::updateLabels()
{
    setElidedText(titleLbl, title);
    setElidedText(lastModifiedLbl, lastModified);
}

void NoteWidget::setElidedText(QLabel *label, const QString &text)
{
    auto parentObj = parent();
    if(parentObj)
    {
        auto parentWidget =qobject_cast<QWidget*>(parentObj);
        if(parentWidget)
        {
            QFontMetrics fm(label->font());
            QString elidedText = fm.elidedText(text, Qt::ElideRight, parentWidget->width() - 20);
            label->setText(elidedText);
        }
    }
}

void NoteWidget::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu(this);

    QAction* addTagAction = contextMenu.addAction("Add Tag");
    addTagAction->setIcon(QIcon(":/icons/tag.svg"));
    //maybe will be implemented in the near future

    QAction* pinAction = contextMenu.addAction(isPinned ? "Unpin Note" : "Pin Note");
    pinAction->setIcon(QIcon(":/icons/pin.svg"));
    connect(pinAction, &QAction::triggered, this, &NoteWidget::togglePinNote);

    QAction* exportAction = contextMenu.addAction("Export to .txt");
    exportAction->setIcon(QIcon(":/icons/export.svg"));
    connect(exportAction, &QAction::triggered, this, &NoteWidget::onExportActionTriggered);

    contextMenu.addSeparator();

    QAction* renameAction = contextMenu.addAction("Rename Note");
    renameAction->setIcon(QIcon(":/icons/rename.svg"));
    connect(renameAction, &QAction::triggered, this, &NoteWidget::onRenameActionTriggered);

    QAction* removeAction = contextMenu.addAction("Remove Note");
    removeAction->setIcon(QIcon(":/icons/remove.svg"));
    connect(removeAction, &QAction::triggered, this, &NoteWidget::onRemoveActionTriggered);

    contextMenu.exec(pos);
}

void NoteWidget::onRemoveActionTriggered()
{
    emit removeNote(index);
}

void NoteWidget::onExportActionTriggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Export Note", QString("%1.txt").arg(title), "Text Files (*.txt)");

    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << title << "\n\n" << content;
            file.close();
        }
    }
}

void NoteWidget::onRenameActionTriggered()
{
    RenameDialog renameDialog(title, this);
    if(renameDialog.exec() == QDialog::Accepted)
    {
        emit renameNote(index, renameDialog.newName());
    }
}
