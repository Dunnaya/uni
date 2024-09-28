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