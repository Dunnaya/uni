#include "notebook.h"
#include "note.h"
#include "xmlstorage.h"

#include <QTextDocument>
#include <QSignalMapper>

int nextNoteId();

Notebook::Notebook(QObject *parent) : QObject{parent}
{
    mapChangedSignalToNoteId = new QSignalMapper(this);
    connect(mapChangedSignalToNoteId, &QSignalMapper::mappedInt, this, &Notebook::onNoteContentChanged);

    readNotes();

    if(notes.size() == 0)
        addNewNote();
}

Notebook::~Notebook()
{
    writeNotes();
}

void Notebook::addNewNote()
{
    int index = nextNoteId();

    auto& [note, textDocument] = notes[index];

    note.index = index;
    note.title = "New note";
    note.lastModified = QDateTime::currentDateTime();

    textDocument = createNewTextDocument(note);

    emit newNoteCreated(index);
}

void Notebook::removeNote(int index)
{
    notes.erase(index);

    if(notes.empty())
        addNewNote();
}

void Notebook::renameNote(int index, const QString &newTitle)
{
    auto found = notes.find(index);
    if(found != notes.end())
    {
        auto& [note, textDocument] = found->second;
        note.title = newTitle;
        note.lastModified = QDateTime::currentDateTime();
    }
}

void Notebook::toggleNotePin(int index)
{
    auto found = notes.find(index);
    if(found != notes.end())
    {
        auto& [note, textDocument] = found->second;
        note.isPinned = !note.isPinned;

        emit notePinToggled(index);
        emit noteContentChanged(index);
    }
}

const Note &Notebook::note(int index) const
{
    return notes.at(index).first;
}

QTextDocument *Notebook::noteDocument(int index) const
{
    auto found = notes.find(index);

    if(found != notes.end())
    {
        return found->second.second.get();
    }

    return nullptr;
}

std::vector<std::reference_wrapper<Note> > Notebook::notebook()
{
    std::vector<std::reference_wrapper<Note> > out;

    for(auto& i : notes)
    {
        auto& [note, textDocument] = i.second;
        note.content = textDocument->toPlainText();
        out.push_back(note);
    }

    return out;
}

size_t Notebook::countNotes() const
{
    return notes.size();
}

void Notebook::onNoteContentChanged(int index)
{
    notes.at(index).first.lastModified = QDateTime::currentDateTime();

    emit noteContentChanged(index);
}

void Notebook::readNotes()
{
    XmlStorage storage;

    auto savedNotes = storage.read();

    for(auto n : savedNotes)
    {
        n.index = nextNoteId();
        auto& [note, textDocument] = notes[n.index];
        note = n;
        textDocument = createNewTextDocument(note);
    }
}

void Notebook::writeNotes()
{
    XmlStorage storage;
    storage.write(notebook());
}

std::unique_ptr<QTextDocument> Notebook::createNewTextDocument(const Note& note)
{
    auto textDocument = std::make_unique<QTextDocument>(note.content);

    connect(textDocument.get(), &QTextDocument::contentsChanged,
            mapChangedSignalToNoteId, qOverload<>(&QSignalMapper::map));

    mapChangedSignalToNoteId->setMapping(textDocument.get(), note.index);

    return textDocument;
}

int nextNoteId()
{
    static int index = 0;
    return ++index;
}
