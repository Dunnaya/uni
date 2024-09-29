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

        //methods for formatting text before saving 

        //a method for searching for keywords
};

class MarkdownNote : public Note
{
    private:
        
        std::string markdownVer;

    public:

        MarkdownNote(const std::string& text, const std::string& version = "1.0")
            : Note(text), markdownVer(version) {}

        std::string previewAsMarkdown()
        {
            return "Markdown preview: " + content;
        }

        std::string exportToFile()
        {
            return "Exporting Markdown to file: " + content;

            //
        }

        std::string exportToPDF();

        //method for parsing Markdown into HTML
        std::string exportToHTML();

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

        Note* getNoteByIndex(int index);

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

        //maybe add findNotesByText
        //and sortNotesByTags

        /*void sortNotesByDate() //classique lambda expression (lol it does not work idk why)
        {
            std::sort(notes.begin(), notes.end(), [](Note* a, Note* b)
            {
                return a->getCreatedAt() < b->getCreatedAt();
            });
        } */

        static bool compareNotes(Note* a, Note* b) 
        {
            return a->getCreatedAt() < b->getCreatedAt();
        }

        void sortNotesByDate()
        {
            std::sort(notes.begin(), notes.end(), compareNotes);
        }

        void displayAllNotes()
        {
            for(auto note : notes)
            {
                std::cout << "Note: " << note->getContent() << "\n";
            }
        }
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

class MarkdownFile : public File
{
    private:

        MarkdownNote note;

    public:

        MarkdownFile(const std::string& name, const MarkdownNote& note)
            : File(name), note(note) {}

        void save()
        {
            std::ofstream file(fileName);

            if(file.is_open())
            {
                file << note.getContent();
                lastModified = std::time(nullptr);
                file.close();
            }
        }

        void load()
        {
            std::ifstream file(fileName);

            if(file.is_open())
            {
                std::string content((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>()); //constructor that reads data from a file character by character
                
                MarkdownNote loadedNote = MarkdownNote(content);
                note = loadedNote;
                file.close();
            }
        }

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
        
        void save()
        {
            std::ofstream file(fileName);

            if(file.is_open())
            {
                file << note.getContent();
                lastModified = std::time(nullptr);
                file.close();
            }
        }

        void load()
        {
            std::ifstream file(fileName);

            if(file.is_open())
            {
                std::string content((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());

                PlainTextNote loadedNote = PlainTextNote(content);
                note = loadedNote;
                file.close();
            }
        }

        PlainTextNote getNote() const
        {
            return note;
        }
};

class FileManager //methods for converting notes into different file formats (PDF, txt)
{
    private:

        std::string file_path;
        std::string format;

    public:

        //method to save all notes from notebook to files
        void saveAll(const Notebook& notebook, const std::string directory);

        //method for loading notes from files into notebook
        Notebook loadAll(const std::string& directory);
};

class PDFExporter
{   
    public:

        void exportToPDF(const Note& note, const std::string& fileName)
        {
            //i need to find a library for this shi
            //maybe QtPdf
            std::cout << "Exporting note to PDF: " << fileName << " ..." << "\n";
        }
};

class User
{
    //username, notebooks, etc.
};

class Encryption //maybe...
{
    //methods for text encryption and decryption
};

class Renderer //render of the note to the console or GUI
{

    public:

        static void render(const Note& note);
        //method for rendering notes with different modes
};

int main()
{
    std::cout << "hii :3 wrld!";
}