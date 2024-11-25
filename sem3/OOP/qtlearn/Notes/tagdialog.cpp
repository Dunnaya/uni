#include "tagdialog.h"
#include "ui_tagdialog.h"
#include <QPushButton>
#include <QHBoxLayout>

const QVector<QColor> TagDialog::PASTEL_COLORS = {
    QColor("#FFB6C1"),  // Light Pink
    QColor("#DDA0DD"),  // Plum
    QColor("#B0E0E6"),  // Powder Blue
    QColor("#98FB98"),  // Pale Green
    QColor("#DEB887"),  // Burlywood
    QColor("#F0E68C"),  // Khaki
    QColor("#E6E6FA"),  // Lavender
    QColor("#FFE4B5")   // Moccasin
};

TagDialog::TagDialog(QWidget* parent) : QDialog(parent), ui(new Ui::TagDialog) {
    ui->setupUi(this);
    setWindowTitle("Add Tag");
    setupColors();
    selectedColor = PASTEL_COLORS[0];
}

TagDialog::~TagDialog() {
    delete ui;
}

void TagDialog::setupColors() {
    QHBoxLayout* colorLayout = new QHBoxLayout();

    for (const QColor& color : PASTEL_COLORS) {
        createColorButton(color);
    }

    ui->colourGroupBox->setLayout(colorLayout);
}

void TagDialog::createColorButton(const QColor& color) {
    QPushButton* btn = new QPushButton();
    btn->setFixedSize(30, 30);
    btn->setStyleSheet(QString("background-color: %1; border: 1px solid gray; border-radius: 15px;")
                           .arg(color.name()));

    connect(btn, &QPushButton::clicked, this, [this, color]() {
        selectedColor = color;
        // Update all buttons to show selection
        for (auto* btn : ui->colourGroupBox->findChildren<QPushButton*>()) {
            btn->setStyleSheet(QString("background-color: %1; border: %2; border-radius: 15px;")
                                   .arg(btn->property("color").toString())
                                   .arg(btn->property("color") == color.name() ? "3px solid black" : "1px solid gray"));
        }
    });

    btn->setProperty("color", color.name());
    ui->colourGroupBox->layout()->addWidget(btn);
}

QString TagDialog::getTagName() const {
    return ui->tagNameEdit->text().trimmed();
}

QColor TagDialog::getSelectedColor() const {
    return selectedColor;
}
