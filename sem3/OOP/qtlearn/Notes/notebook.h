#pragma once
#include <QObject>

#include <unordered_map>
#include <vector>
#include <utility>
#include <memory>

class Note;

class QTextDocument;
class QSignalMapper;

class Notebook : public QObject
{
    Q_OBJECT

public:

    explicit Notebook(QObject *parent = nullptr);

    ~Notebook();

    void addNewNote();
    void removeNote(int index);
    void renameNote(int index, const QString& newTitle);

    const Note& note(int index) const;

    QTextDocument* noteDocument(int index) const;
    std::vector<std::reference_wrapper<Note>> notebook();

    size_t countNotes() const;

private:

    std::unordered_map<int, std::pair<Note, std::unique_ptr<QTextDocument>>> notes;

    QSignalMapper* mapChangedSignalToNoteId = nullptr;

    void onNoteContentChanged(int index);

    void readNotes();
    void writeNotes();

    std::unique_ptr<QTextDocument> createNewTextDocument(const Note& note);

signals:

    void newNoteCreated(int index);
    void noteContentChanged(int index);
};

