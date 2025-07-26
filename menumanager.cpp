#include "menumanager.h"
#include "ui_menumanager.h"
#include "additemdialog.h"
#include "categorymanager.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QGridLayout>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QList>
#include <utility>

MenuManager::MenuManager(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MenuManager)
{
    ui->setupUi(this);
    ui->scrollArea->setWidget(ui->menuContainer);
    loadMenu();
}

MenuManager::~MenuManager()
{
    delete ui;
}
void MenuManager::loadMenu()
{
    qDebug() << "Loading Menu...";

    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->menuContainer->layout());
    if (!mainLayout) {
        mainLayout = new QVBoxLayout(ui->menuContainer);
        ui->menuContainer->setLayout(mainLayout);
    }

    // Clear previous
    QLayoutItem *child;
    while ((child = mainLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }

    QSqlQuery query("SELECT DISTINCT category FROM menu_items");
    while (query.next()) {
        QString category = query.value(0).toString();
        qDebug() << "Found category:" << category;

        // Category section
        QWidget *categoryContainer = new QWidget;
        QVBoxLayout *categoryLayout = new QVBoxLayout(categoryContainer);
        categoryLayout->setContentsMargins(0, 10, 0, 0);

        QLabel *catLabel = new QLabel(category);
        QFont font; font.setBold(true); font.setPointSize(12);
        catLabel->setFont(font);
        catLabel->setAlignment(Qt::AlignLeft);
        catLabel->setStyleSheet("color: #E47B20;");
        categoryLayout->addWidget(catLabel);

        // Items grid
        QGridLayout *grid = new QGridLayout;
        grid->setSpacing(10);

        QSqlQuery itemsQuery;
        itemsQuery.prepare("SELECT id, name, price FROM menu_items WHERE category = ?");
        itemsQuery.addBindValue(category);
        itemsQuery.exec();
        qDebug() << "Loading items for category:" << category;

        int row = 0, col = 0;
        while (itemsQuery.next()) {
            int id = itemsQuery.value(0).toInt();
            QString name = itemsQuery.value(1).toString();
            double price = itemsQuery.value(2).toDouble();

            QFrame *itemBox = new QFrame;
            itemBox->setFrameShape(QFrame::Box);
            itemBox->setLineWidth(1);
            itemBox->setFixedSize(150, 90);
            itemBox->setProperty("item_id", id);
            itemBox->setObjectName("itemBox");
            itemBox->setStyleSheet("background:#f9f9f9;");

            QVBoxLayout *layout = new QVBoxLayout(itemBox);
            QLabel *nameLabel = new QLabel(name);
            QLabel *priceLabel = new QLabel(QString("Rs %1").arg(price));
            nameLabel->setWordWrap(true);
            nameLabel->setAlignment(Qt::AlignCenter);
            priceLabel->setAlignment(Qt::AlignCenter);
            layout->addWidget(nameLabel);
            layout->addWidget(priceLabel);

            itemBox->installEventFilter(this);
            grid->addWidget(itemBox, row, col++);
            if (col == 3) { col = 0; row++; }
        }

        categoryLayout->addLayout(grid);
        mainLayout->addWidget(categoryContainer);
    }
}


bool MenuManager::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QFrame *frame = qobject_cast<QFrame*>(obj);
        if (frame && frame->objectName() == "itemBox") {
            for (QFrame *other : std::as_const(selectedItems)) {
                other->setStyleSheet("background:#f9f9f9;");
            }
            selectedItems.clear();

            frame->setStyleSheet("background:#cceeff;");
            selectedItems.append(frame);
        }
    }
    return QWidget::eventFilter(obj, event);
}

void MenuManager::on_addItemButton_clicked()
{
    AddItemDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        loadMenu();
    }
}

void MenuManager::on_deleteItemButton_clicked()
{
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select an item to delete.");
        return;
    }

    QFrame *selected = selectedItems.first();
    int itemId = selected->property("item_id").toInt();

    QSqlQuery query;
    query.prepare("DELETE FROM menu_items WHERE id = ?");
    query.addBindValue(itemId);

    if (query.exec()) {
        QMessageBox::information(this, "Deleted", "Item deleted successfully.");
        selectedItems.clear();
        loadMenu();
    } else {
        QMessageBox::critical(this, "Error", query.lastError().text());
    }
}

void MenuManager::on_backButton_clicked()
{
    this->close();
}

void MenuManager::on_editCategoriesButton_clicked()
{
    CategoryManager catDialog(this);
    catDialog.exec();
    loadMenu();
}
