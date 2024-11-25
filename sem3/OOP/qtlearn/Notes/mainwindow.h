#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class Notebook;
class Note;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(Notebook& notebook, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //handle signals from ui
    void onNewNoteBtnClicked();
    void onRemoveBtnClicked();

    //handle Notebook signals
    void onNewNoteCreated(int index);
    void onNoteContentChanged(int index);

    //handle NotesListWidget signals
    void onSelectedNoteChanged(int index);
    void onRemoveNote(int index);
    void onRenameNote(int index, const QString& newTitle);
    void onTogglePinNote();
    void onTogglePinRequested(int index);

    void onSearchTextChanged(const QString& text);

private:
    void addNoteToList(const Note& note);
    void removeNote(int index);

    void setTheme(bool isDark);
    bool isDarkTheme = false;
    QAction* themeToggleAction;
    void setupThemeButton();

    void changeFontFamily();
    void changeFontColor();

    void init();
    void makeConnections();

    Ui::MainWindow *ui;
    Notebook& notesManager;
};
#endif // MAINWINDOW_H
