#include "categorymanager.h"
#include "ui_categorymanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

CategoryManager::CategoryManager(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CategoryManager)
{
    ui->setupUi(this);
    loadCategories();
}

CategoryManager::~CategoryManager()
{
    delete ui;
}

void CategoryManager::loadCategories()
{
    ui->categoryListWidget->clear();
    QSqlQuery query("SELECT name FROM categories ORDER BY name ASC");
    while (query.next()) {
        ui->categoryListWidget->addItem(query.value(0).toString());
    }
}

void CategoryManager::on_addCategoryButton_clicked()
{
    QString name = ui->categoryLineEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Category name cannot be empty.");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO categories (name) VALUES (?)");
    query.addBindValue(name);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
    } else {
        loadCategories();
        ui->categoryLineEdit->clear();
    }
}

void CategoryManager::on_deleteCategoryButton_clicked()
{
    QListWidgetItem *item = ui->categoryListWidget->currentItem();
    if (!item) {
        QMessageBox::warning(this, "No Selection", "Please select a category to delete.");
        return;
    }

    QString name = item->text();
    QSqlQuery query;
    query.prepare("DELETE FROM categories WHERE name = ?");
    query.addBindValue(name);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
    } else {
        loadCategories();
    }
}

void CategoryManager::on_closeButton_clicked()
{
    this->close();
}
