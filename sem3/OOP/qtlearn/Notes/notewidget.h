#pragma once

#include <QWidget>

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

signals:
    void removeNote(int index);
    void renameNote(int index, const QString& newTitle);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void setupGui();
    void updateLabels();

    void setElidedText(QLabel* label, const QString& text);

    void showContextMenu(const QPoint& pos);
    void onRemoveActionTriggered();
    void onRenameActionTriggered();

    QLabel* titleLbl;
    QLabel* lastModifiedLbl;

    int index;
    QString title;
    QString lastModified;

    const QString dateTimeFormat = "dd/MM/yyyy - hh:mm:ss";
};
