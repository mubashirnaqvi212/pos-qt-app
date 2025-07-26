#ifndef CATEGORYMANAGER_H
#define CATEGORYMANAGER_H

#include <QDialog>

namespace Ui {
class CategoryManager;
}

class CategoryManager : public QDialog
{
    Q_OBJECT

public:
    explicit CategoryManager(QWidget *parent = nullptr);
    ~CategoryManager();

private slots:
    void loadCategories();
    void on_addCategoryButton_clicked();
    void on_deleteCategoryButton_clicked();
    void on_closeButton_clicked();

private:
    Ui::CategoryManager *ui;
};

#endif // CATEGORYMANAGER_H
