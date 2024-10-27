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
};
