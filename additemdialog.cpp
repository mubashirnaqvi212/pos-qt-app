#include "additemdialog.h"
#include "ui_additemdialog.h"
#include "categorymanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

AddItemDialog::AddItemDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddItemDialog)
{
    ui->setupUi(this);
    connect(ui->editCategoriesButton, &QPushButton::clicked, this, &AddItemDialog::on_editCategoriesButton_clicked);
    loadCategories();
}

AddItemDialog::~AddItemDialog()
{
    delete ui;
}

void AddItemDialog::loadCategories()
{
    ui->categoryComboBox->clear();

    QSqlQuery query("SELECT name FROM categories");
    while (query.next()) {
        ui->categoryComboBox->addItem(query.value(0).toString());
    }

    if (ui->categoryComboBox->count() == 0) {
        ui->addButton->setEnabled(false);
        QMessageBox::warning(this, "No Categories", "Please add categories before adding items.");
    } else {
        ui->addButton->setEnabled(true);
    }
}

void AddItemDialog::on_addButton_clicked()
{
    QString name = ui->nameLineEdit->text().trimmed();
    QString priceText = ui->priceLineEdit->text().trimmed();
    QString category = ui->categoryComboBox->currentText();

    if (name.isEmpty() || priceText.isEmpty() || category.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill all fields.");
        return;
    }

    bool ok;
    double price = priceText.toDouble(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Input Error", "Price must be a valid number.");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO menu_items (name, price, category) VALUES (?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(price);
    query.addBindValue(category);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
        return;
    }

    QMessageBox::information(this, "Success", "Item added successfully.");
    accept();
}

void AddItemDialog::on_cancelButton_clicked()
{
    reject();
}
void AddItemDialog::on_editCategoriesButton_clicked()
{
    CategoryManager dialog(this);
    dialog.exec();

    // Refresh categories after editing
    ui->categoryComboBox->clear();
    QSqlQuery query("SELECT name FROM categories");
    while (query.next()) {
        ui->categoryComboBox->addItem(query.value(0).toString());
    }
}
