#pragma once

#include <QWidget>

namespace Ui {
class NotesListWidget;
}

class Note;
//class Notebook;
class QListWidgetItem;

class NotesListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NotesListWidget(QWidget *parent = nullptr);
    ~NotesListWidget();

    void addNote(const Note& note);
    void removeCurrentNote();
    void updateCurrentNote(const Note& note);

    int currentNoteId();

    void togglePinStatus(int index);
    void addNoteToList(const Note& note);

    void filterNotes(const QString& searchText);
    void showAllNotes();

signals:
    void selectedNoteChanged(int index);
    void removeNote(int index);
    void renameNote(int index, const QString& newTitle);
    void togglePinNote(int index);
    void togglePinRequested(int index);

private:
    Ui::NotesListWidget *ui;

    QMap<QListWidgetItem*, int> itemToNoteId;

    void onItemSelectionChanged();

    void refreshNotesList();

    void moveCurrentItemToTop(const Note& note);
    void setupNoteItem(const Note& note, QListWidgetItem* item);

    Note* findNoteById(int noteId);

    int findInsertPosition(const Note& note);
};
