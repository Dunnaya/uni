#include "renamedialog.h"
#include "ui_renamedialog.h"

#include <QPushButton>

RenameDialog::RenameDialog(const QString& oldName, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RenameDialog)
{
    ui->setupUi(this);

    connect(ui->newNameLineEdit, &QLineEdit::textChanged, this,  &RenameDialog::onNewNameLEEdited);

    ui->newNameLineEdit->setText(oldName);
    ui->newNameLineEdit->setSelection(0, ui->newNameLineEdit->text().size());
}

RenameDialog::~RenameDialog()
{
    delete ui;
}

QString RenameDialog::newName()
{
    return ui->newNameLineEdit->text();
}

void RenameDialog::onNewNameLEEdited(const QString &text)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(text.size() != 0);
}
