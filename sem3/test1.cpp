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

class Notebook;
class MarkdownNote;
class PlainTextNote;
class Tags;
class File;
class MarkdownFile;
class PlainTextFile;
class FileManager;
class PDFExporter;

int main()
{
    std::cout<<"puke";
}