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

        void addTag();
        void removeTag();
        bool hasTag();

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

        void deleteNote();
};

class MarkdownNote;
class PlainTextNote;

class Tags
{
    public:
        void addTag();
        void removeTag();
        bool hasTag();
};

class File
{
    protected:
        std::string fileName;

    public:
    //file constructor

    bool exists();
};

class MarkdownFile;
class PlainTextFile;
class FileManager;
class PDFExporter;

int main()
{
    std::cout<<"hii :3 wrld!";
}