#pragma once
#include <QDialog>
#include <QColor>

namespace Ui
{
    class TagDialog;
}

class TagDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TagDialog(QWidget* parent = nullptr);
    ~TagDialog();

    QString getTagName() const;
    QColor getSelectedColor() const;

private:
    void setupColors();
    void createColorButton(const QColor& color);

    Ui::TagDialog* ui;
    QColor selectedColor;
    static const QVector<QColor> PASTEL_COLORS;
};
