#pragma once

#include <QWidget>

namespace Ui {
class NotesListWidget;
}

class Note;
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

signals:
    void selectedNoteChanged(int index);

private:
    Ui::NotesListWidget *ui;

    void onItemSelectionChanged();

    void moveCurrentItemToTop(const Note& note);
    void setupNoteItem(const Note& note, QListWidgetItem* item);
};
