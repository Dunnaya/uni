#pragma once

#include <QWidget>

#include <QFileDialog>
#include <QTextStream>

class QLabel;
class QPoint;

class Note;

class NoteWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NoteWidget(const Note& note, QWidget *parent = nullptr);

    int noteId() const;
    void UpdateContent(const Note& note);

    bool getIsPinned() const { return isPinned; }

    QString getTitle() const { return title; }

    QString getLastModified() const { return lastModified; }

signals:
    void removeNote(int index);
    void renameNote(int index, const QString& newTitle);
    void togglePinNote();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void updatePinDisplay();
    void setupGui();
    void updateLabels();

    void setElidedText(QLabel* label, const QString& text);

    void showContextMenu(const QPoint& pos);
    void onRemoveActionTriggered();
    void onExportActionTriggered();
    void onRenameActionTriggered();

    QLabel* titleLbl;
    QLabel* lastModifiedLbl;

    int index;
    QString title;
    QString lastModified;
    bool isPinned;
    QString content;

    const QString dateTimeFormat = "dd/MM/yyyy - hh:mm:ss";
};
