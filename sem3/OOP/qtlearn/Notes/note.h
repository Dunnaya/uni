#pragma once

#include <QString>
#include <QDateTime>
#include <QFont>
#include <QColor>

class Note
{
public:
    int index;
    QString title;
    QString content;
    QDateTime lastModified;
    bool isPinned = false;

    QFont font;
    QColor textColor;

    void setPinned(bool pinned) { isPinned = pinned; }
    bool getPinned() const { return isPinned; }
};
