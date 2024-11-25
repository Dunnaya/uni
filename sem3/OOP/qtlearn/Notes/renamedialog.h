#pragma once

#include <QDialog>

namespace Ui
{
    class RenameDialog;
}

class RenameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RenameDialog(const QString& oldName, QWidget *parent = nullptr);
    ~RenameDialog();

    QString newName();

private:
    Ui::RenameDialog *ui;

    void onNewNameLEEdited(const QString& text);
};
