/**
 * @file main.cpp
 * @author DS1 (holydunnaya@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-28
 * 
 */

/*
Не менше 9 класів (або інших подібних типів); /10
Не менше 15 полів (в сумі в усіх реалізованих класах); /14
Не менше 25 нетривіальних методів або функцій. /26
Не менше 2 ієрархій успадкування, хоча б одна з який містить не менше 3 класів;
Не менше 3 незалежних випадків поліморфізму(статичного та динамічного)
Також має бути правильно реалізовано інкапсуляцію – усі реалізовані поля та методи повинні мати доцільний рівень доступу.

Необхідно вказати скільки елементів кожного типу реалізовано в коді – точна кількість, бажано з переліком відповідних елементів.
*/

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <filesystem> //for directory_iterator()

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

        virtual std::string preview() const = 0;

        //virtual std::string exportToFile(const std::string& fileName) const = 0;

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

        std::vector<size_t> searchKeywords(const std::string& keyword) const
        {
            std::vector<size_t> positions;
            size_t pos = content.find(keyword);

            while(pos != std::string::npos)
            {
                positions.push_back(pos);
                pos = content.find(keyword, pos + keyword.length());
            }

            return positions;
        }

        //setContent to change the text of the note
};

class MarkdownNote : public Note
{
    private:
        
        std::string markdownVer;

    public:

        MarkdownNote(const std::string& text, const std::string& version = "1.0")
            : Note(text), markdownVer(version) {}

        std::string preview() const override
        {
            return "Markdown preview:\n" + content + "\n";
        }

        /*std::string exportToFile(const std::string& fileName = "note.md") const override
        {
            std::ofstream file(fileName);

            if(file.is_open())
            {
                file << content;
                file.close();
                return "Markdown note exported to " + fileName;
            } 
            else
                return "Failed to open file " + fileName;
        }*/ 
        
        //DONT REPEAT URSELF -???

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
        
        std::string preview() const override
        {
            return "Plain Text Note Preview:\n" + content + "\n";
        }

       // std::string exportToFile(const std::string& fileName = "note.txt") const override; !!! DRY !!!
};

class Tags
{
    public:

        virtual void addTag(const std::string tag) = 0;
        virtual void removeTag(const std::string tag) = 0;
        virtual bool hasTag(const std::string tag) = 0;
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

        Note* getNoteByIndex(int index) const
        {
            if(index >= 0 && index < notes.size())
                return notes[index];
            return nullptr; //if index is incorrect
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

        virtual void save() const = 0;

        virtual void load() = 0;

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

        void save() const override
        {
            std::ofstream file(fileName);

            if(file.is_open())
            {
                file << note.getContent();
                file.close();
            }
        } //need to change the lastModified

        void load() override
        {
            std::ifstream file(fileName);

            if(file.is_open())
            {
                /*
                std::string content((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>()); //constructor that reads data from a file character by character
                    */
                std::string line, content;

                while(std::getline(file, line))
                {
                    content += line + "\n";
                }
                
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
        
        void save() const override
        {
            std::ofstream file(fileName);

            if(file.is_open())
            {
                file << note.getContent();
                file.close();
            }
        } //need to change the lastModified

        void load() override
        {
            std::ifstream file(fileName);

            if(file.is_open())
            {
                /*
                std::string content((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
                    */
                std::string line, content;

                while(std::getline(file, line))
                {
                    content += line + "\n";
                }

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
        void saveAll(const Notebook& notebook, const std::string directory)
        {
            for(int i = 0; i < notebook.getNumOfNotes(); ++i)
            {
                Note* note = notebook.getNoteByIndex(i);
                std::string fileName = directory + "/note" + std::to_string(i) + ".txt";

                std::ofstream file(fileName);

                if(file.is_open())
                {
                    file << note->getContent();
                    file.close();
                }
            }
        }

        //method for loading notes from files into notebook
        Notebook loadAll(const std::string& directory)
        {
            Notebook notebook;
            for(const auto& entry : std::filesystem::directory_iterator(directory))
            {
                std::ifstream file(entry.path());

                if(file.is_open())
                {
                    std::string line, content;
                    while(std::getline(file, line))
                    {
                        content += line + "\n";
                    }

                    notebook.addNote(new PlainTextNote(content));
                    file.close();
                }   
            }
            return notebook;
        }
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
    private:

        std::string username;
        std::vector<Notebook> notebooks;
        int activeNotebookIndex;

    public:

        User(const std::string name) : username(name), activeNotebookIndex(0) {}

        std::string getUsername() const
        {
            return username;
        }

        void addNotebook(const Notebook& notebook)
        {
            notebooks.push_back(notebook);
        }

        void deleteNotebook(int index)
        {
            if(index >= 0 && index < notebooks.size())
            {
                notebooks.erase(notebooks.begin() + index);
            }
            else
                std::cout << "Invalid notebook index.\n";
        }

        Notebook getActiveNotebook()
        {
            return notebooks[activeNotebookIndex];
        }

        void setActiveNotebook(int index);

        void listNotebooks() const
        {
            std::cout << "User " << username << " has the following notebooks:\n";

            for(int i = 0; i < notebooks.size(); ++i)
            {
                std::cout << i+1 << ". Notebook: " << notebooks[i].getNumOfNotes() << " notes.\n";
            }
        }

        void saveNotebooksToFiles(const std::string& directory)
        {
            FileManager fileManager;

            for(int i = 0; i < notebooks.size(); ++i)
            {
                fileManager.saveAll(notebooks[i], directory + "/notebook" + std::to_string(i));
            }
            std::cout << "All notebooks saved to " << directory << "\n";
        }

        void loadNotebooksFromFiles(const std::string& directory)
        {
            FileManager fileManager;
            notebooks.clear();

            for(const auto& entry : std::filesystem::directory_iterator(directory))
            {
                Notebook notebook = fileManager.loadAll(entry.path().string());
                notebooks.push_back(notebook);
            }

            std::cout << "All notebooks loaded from " << directory << "\n";
        }

        int getNumOfNotebooks()
        {
            return notebooks.size();
        }

        void userMenu()
        {
            int choice;

            do
            {
                std::cout << "\n--- Notebook Menu ---\n";
                std::cout << "1. Create a new Notebook\n";
                std::cout << "2. Add a note to active Notebook\n";
                std::cout << "3. Display all notes in active Notebook\n";
                std::cout << "4. Save all Notebooks to files\n";
                std::cout << "5. Load Notebooks from files\n";
                std::cout << "6. List Notebooks\n";
                std::cout << "0. Exit\n";
                std::cout << "Enter your choice: ";
                std::cin >> choice;

                switch (choice)
                {
                    case 1:
                    {
                        std::string name;
                        std::cout << "Enter Notebook name: ";
                        std::cin >> name;
                        addNotebook(Notebook());
                        std::cout << "Notebook created.\n";

                        break;
                    }

                    case 2:
                    {
                        if (notebooks.empty())
                        {
                            std::cout << "No notebooks available. Please create one first.\n";
                            break;
                        }

                        std::string noteText;
                        std::cout << "Enter Note text: ";
                        std::cin.ignore(); // Clear the input buffer
                        std::getline(std::cin, noteText);
                        notebooks[activeNotebookIndex].addNote(new PlainTextNote(noteText));
                        std::cout << "Note added to active notebook.\n";

                        break;
                    }

                    case 3:
                    {
                        if (notebooks.empty())
                        {
                            std::cout << "No notebooks available.\n";
                            break;
                        }
                        notebooks[activeNotebookIndex].displayAllNotes();

                        break;
                    }

                    case 4:
                    {
                        std::string directory;
                        std::cout << "Enter directory to save Notebooks: ";
                        std::cin >> directory;
                        saveNotebooksToFiles(directory);

                        break;
                    }

                    case 5:
                    {
                        std::string directory;
                        std::cout << "Enter directory to load Notebooks from: ";
                        std::cin >> directory;
                        loadNotebooksFromFiles(directory);

                        break;
                    }

                    case 6:
                    {
                        listNotebooks();
                        break;
                    }

                    case 0:
                        std::cout << "Exiting.........\n";
                        break;

                    default:
                        std::cout << "Invalid choice.\n";
                }
            } while (choice != 0);
        }
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
    User user("Jane Doe");

    PlainTextNote note1("This is a plain text note.");
    
    MarkdownNote note2("This is a **markdown** note.");
    
    note1.addTag("personal");
    note2.addTag("work");
    
    Notebook notebook;
    notebook.addNote(&note1);
    notebook.addNote(&note2);
    
    notebook.displayAllNotes();
    
    FileManager fileManager;
    fileManager.saveAll(notebook, "./notes");

    Notebook loadedNotebook = fileManager.loadAll("./notes");
    loadedNotebook.displayAllNotes();
    
    PDFExporter exporter;
    exporter.exportToPDF(note1, "note1.pdf");

    return 0;
}