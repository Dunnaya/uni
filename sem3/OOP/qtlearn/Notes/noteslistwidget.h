#ifndef NOTESLISTWIDGET_H
#define NOTESLISTWIDGET_H

#include <QWidget>

namespace Ui {
class NotesListWidget;
}

class NotesListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NotesListWidget(QWidget *parent = nullptr);
    ~NotesListWidget();

private:
    Ui::NotesListWidget *ui;
};

#endif // NOTESLISTWIDGET_H
