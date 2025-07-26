#ifndef ADDITEMDIALOG_H
#define ADDITEMDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQueryModel>

namespace Ui {
class AddItemDialog;
}

class AddItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddItemDialog(QWidget *parent = nullptr);
    ~AddItemDialog();

private slots:
    void on_addButton_clicked();
    void on_cancelButton_clicked();
    void on_editCategoriesButton_clicked();

private:
    Ui::AddItemDialog *ui;
    void loadCategories();
};

#endif // ADDITEMDIALOG_H
