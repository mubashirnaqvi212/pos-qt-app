#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <QWidget>
#include <QFrame>

namespace Ui {
class MenuManager;
}

class MenuManager : public QWidget
{
    Q_OBJECT

public:
    explicit MenuManager(QWidget *parent = nullptr);
    ~MenuManager();

private slots:
    void on_addItemButton_clicked();
    void on_deleteItemButton_clicked();
    void on_backButton_clicked();
    void on_editCategoriesButton_clicked();

private:
    Ui::MenuManager *ui;
    void loadMenu();
    bool eventFilter(QObject *obj, QEvent *event) override;

    QList<QFrame*> selectedItems;
};

#endif // MENUMANAGER_H
