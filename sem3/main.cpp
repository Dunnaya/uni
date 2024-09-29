/**
 * @file main.cpp
 * @author DS1 (holydunnaya@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-28
 * 
 */

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include <fstream>

class Note
{
    protected:

        std::string content;
        std::time_t createdAt;
        std::vector<std::string> tags;

    public:

        Note(const std::string& text) : content(text) 
        {
            createdAt = std::time(nullptr);
        }

        void addTag(const std::string& tag)
        {
            tags.push_back(tag);
        }

        void removeTag(const std::string& tag)
        {
            tags.erase(std::remove(tags.begin(), tags.end(), tag), tags.end());
        }

        bool hasTag(const std::string& tag) const
        {
            return std::find(tags.begin(), tags.end(), tag) != tags.end();
        }

        void clear()
        {
            content.clear();
        }

        std::time_t getCreatedAt() const 
        {
            return createdAt;
        }

        std::string getContent() const 
        {
            return content;
        }
};

class Notebook
{
    private:

        std::vector<Note*> notes; //like... uhm... the collection of notes...

    public:
        
        void addNote(Note* note)
        {
            notes.push_back(note);
        }

        void deleteNote(int index)
        {
            if(index >= 0 && index < notes.size())
                notes.erase(notes.begin() + index);
        }

        int getNumOfNotes() const
        {
            return notes.size();
        }

        std::vector<Note*> findNotesByTag(const std::string& tag)
        {
            std::vector<Note*> foundNotes;

            for(auto note : notes)
            {
                if(note -> hasTag(tag))
                    foundNotes.push_back(note);
            }

            return foundNotes;
        }

        void sortNotesBy(); //there will be method of sorting notes by date or smth like that
};

class File
{
    protected:

        std::string fileName;
        std::time_t lastModified;

    public:

        File(const std::string& name) : fileName(name)
        {
            lastModified = std::time(nullptr);
        }

        bool exists() const
        {
            std::ifstream file(fileName);
            return file.good(); //returns true if stream is ok, if file was not opened ot if there is an error it will return false
        }

        std::time_t getLastModified() const
        {
            return lastModified;
        }

        std::string getFileName() const
        {
            return fileName;
        }
};

class MarkdownNote : public Note
{
    private:
        
        std::string markdownVer;

    public:

        MarkdownNote(const std::string& text, const std::string& version = "1.0")
            : Note(text), markdownVer(version) {}

        std::string preview();

        std::string exportToFile();

        std::string exportToPDF();

        std::string getMarkdownVer() const
        {
            return markdownVer;
        }
};

class PlainTextNote : public Note
{
    public:

        PlainTextNote(const std::string& text) : Note(text) {}
        
        std::string preview();

        std::string exportToFile();
};

class Tags
{
    public:

        void addTag(const std::string tag);
        void removeTag(const std::string tag);
        bool hasTag(const std::string tag);
};

class MarkdownFile : public File
{
    private:

        MarkdownNote note;

    public:

        MarkdownFile(const std::string& name, const MarkdownNote& note)
            : File(name), note(note) {}

        void save();

        void load();

        MarkdownNote getNote() const
        {
            return note;
        }
};

class PlainTextFile : public File
{
    private:

        PlainTextNote note;
        
    public:

        PlainTextFile(const std::string& name, const PlainTextNote& note)
            : File(name), note(note) {}
        
        void save();

        void load();

        PlainTextNote getNote() const
        {
            return note;
        }
};

class FileManager;

class PDFExporter
{
    void exportToPDF(const Note& note, const std::string& fileName)
    {
        //i need to find a library for this shi
        std::cout << "Exporting note to PDF: " << fileName << " ..." << "\n";
    }
};

int main()
{
    std::cout<<"hii :3 wrld!";
}