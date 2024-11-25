#pragma once

#include <QString>
#include <QColor>

class Tag
{
public:
    QString name;
    QColor color;

    Tag(const QString& name = "", const QColor& color = QColor("#FFB6C1"))
        : name(name), color(color) {}

    bool operator==(const Tag& other) const {
        return name == other.name;
    }
};
