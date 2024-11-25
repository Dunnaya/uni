#pragma once

#include <QString>
#include <QDateTime>

class Note
{
public:
    int index;
    QString title;
    QString content;
    QDateTime lastModified;
    bool isPinned = false;

    void setPinned(bool pinned) { isPinned = pinned; }
    bool getPinned() const { return isPinned; }
};
