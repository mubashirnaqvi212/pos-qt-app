#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "saleshistory.h"
#include "menumanager.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QTableWidgetItem>
#include <QtPrintSupport/QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QTextDocument>
#include <QDateTime>
#include <QCompleter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPixmap logo(":/images/logo.jpg");
    ui->logoLabel->setPixmap(logo);
    ui->logoLabel->setScaledContents(true);
    ui->logoLabel->setAlignment(Qt::AlignHCenter);

    ui->dashboardFrame->setVisible(false);

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(Qt::white));
    this->setPalette(palette);

    connect(ui->addProductButton, &QPushButton::clicked, this, &MainWindow::onAddProductClicked);
    connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::onClearClicked);
    connect(ui->sellButton, &QPushButton::clicked, this, &MainWindow::onSellButtonClicked);
    connect(ui->historyButton, &QPushButton::clicked, this, &MainWindow::onHistoryButtonClicked);
    connect(ui->historyButton_2, &QPushButton::clicked, this, &MainWindow::onHistoryButtonClicked);
    connect(ui->menuButton, &QPushButton::clicked, this, &MainWindow::toggleDashboard);
    connect(ui->menuManagerButton, &QPushButton::clicked, this, &MainWindow::onMenuManagerButtonClicked);

    ui->salesTable->setColumnCount(4);
    QStringList headers;
    headers << "Product Name" << "Price" << "Quantity" << "Total";
    ui->salesTable->setHorizontalHeaderLabels(headers);

    // SQLite Setup
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("pos_database.db");

    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", db.lastError().text());
    } else {
        QSqlQuery query;

        // Create tables
        query.exec("CREATE TABLE IF NOT EXISTS sales ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "product_name TEXT,"
                   "price REAL,"
                   "quantity INTEGER,"
                   "datetime TEXT)");

        // Optional: Add datetime column if not present (safe fallback)
        query.exec("ALTER TABLE sales ADD COLUMN datetime TEXT");

        query.exec("CREATE TABLE IF NOT EXISTS categories ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "name TEXT UNIQUE)");

        query.exec("CREATE TABLE IF NOT EXISTS menu_items ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "name TEXT,"
                   "price REAL,"
                   "category TEXT)");
    }

    setupItemAutoSuggest();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAddProductClicked()
{
    QString name = ui->productNameLineEdit->text();
    QString priceStr = ui->priceLineEdit->text();
    QString quantityStr = ui->quantityLineEdit->text();

    if (name.isEmpty() || priceStr.isEmpty() || quantityStr.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill all fields.");
        return;
    }

    double price = priceStr.toDouble();
    int quantity = quantityStr.toInt();
    double total = price * quantity;

    int row = ui->salesTable->rowCount();
    ui->salesTable->insertRow(row);
    ui->salesTable->setItem(row, 0, new QTableWidgetItem(name));
    ui->salesTable->setItem(row, 1, new QTableWidgetItem(QString::number(price)));
    ui->salesTable->setItem(row, 2, new QTableWidgetItem(QString::number(quantity)));
    ui->salesTable->setItem(row, 3, new QTableWidgetItem(QString::number(total)));

    updateTotal();
}

void MainWindow::onClearClicked()
{
    ui->salesTable->setRowCount(0);
    ui->productNameLineEdit->clear();
    ui->priceLineEdit->clear();
    ui->quantityLineEdit->clear();
    updateTotal();
}

void MainWindow::onSellButtonClicked()
{
    double grandTotal = 0.0;
    QList<QList<QString>> items;
    QSqlQuery query;

    for (int row = 0; row < ui->salesTable->rowCount(); ++row) {
        QString name = ui->salesTable->item(row, 0)->text();
        double price = ui->salesTable->item(row, 1)->text().toDouble();
        int quantity = ui->salesTable->item(row, 2)->text().toInt();
        double total = price * quantity;

        grandTotal += total;

        query.prepare("INSERT INTO sales (product_name, price, quantity, datetime) VALUES (?, ?, ?, ?)");
        query.addBindValue(name);
        query.addBindValue(price);
        query.addBindValue(quantity);
        query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

        if (!query.exec()) {
            qDebug() << "Insert error:" << query.lastError().text();
        }

        QList<QString> rowData;
        rowData << name << QString::number(price) << QString::number(quantity) << QString::number(total);
        items.append(rowData);
    }

    printReceipt(items, grandTotal);

    QMessageBox::information(this, "Sale Completed",
                             QString("Products sold and recorded!\nGrand Total: Rs %1").arg(grandTotal));

    ui->salesTable->setRowCount(0);
    updateTotal();
}

void MainWindow::updateTotal()
{
    double total = 0.0;
    for (int row = 0; row < ui->salesTable->rowCount(); ++row) {
        double price = ui->salesTable->item(row, 1)->text().toDouble();
        int quantity = ui->salesTable->item(row, 2)->text().toInt();
        total += price * quantity;
    }

    setWindowTitle(QString("Total: Rs %1").arg(total));
}

void MainWindow::onHistoryButtonClicked()
{
    SalesHistory *history = new SalesHistory();
    history->setAttribute(Qt::WA_DeleteOnClose);
    history->setWindowTitle("Sales History");
    history->showMaximized();
}

void MainWindow::onMenuManagerButtonClicked()
{
    MenuManager *menuPage = new MenuManager();
    menuPage->setWindowTitle("Menu Manager");
    menuPage->setAttribute(Qt::WA_DeleteOnClose);
    menuPage->showMaximized();
}

void MainWindow::printReceipt(const QList<QList<QString>> &items, double grandTotal)
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Receipt", "", "PDF Files (*.pdf)");
    if (filename.isEmpty())
        return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename);

    QString html;
    html += "<h2 style='text-align:center;'>Tangy & Toasted - Receipt</h2>";
    html += "<p style='text-align:right;'>" + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm AP") + "</p>";
    html += "<hr>";

    html += "<table width='100%' border='1' cellspacing='0' cellpadding='4'>";
    html += "<tr style='font-weight:bold; background:#f0f0f0;'>"
            "<td>Product</td><td>Price</td><td>Quantity</td><td>Total</td></tr>";

    for (const QList<QString> &item : items) {
        html += "<tr>";
        html += "<td>" + item[0] + "</td>";
        html += "<td>" + item[1] + "</td>";
        html += "<td>" + item[2] + "</td>";
        html += "<td>" + item[3] + "</td>";
        html += "</tr>";
    }

    html += "</table>";
    html += "<h3 style='text-align:right;'>Grand Total: ₨ " + QString::number(grandTotal) + "</h3>";
    html += "<p style='text-align:center;'>Thank you for your purchase!<br>Visit Again!</p>";

    QTextDocument doc;
    doc.setHtml(html);
    doc.setPageSize(printer.pageRect(QPrinter::Point).size());

    doc.print(&printer);

    QMessageBox::information(this, "PDF Saved", "Receipt saved successfully.");
}

void MainWindow::toggleDashboard()
{
    bool visible = ui->dashboardFrame->isVisible();
    ui->dashboardFrame->setVisible(!visible);
}

void MainWindow::setupItemAutoSuggest()
{
    itemPriceMap.clear();

    QSqlQuery query("SELECT name, price FROM menu_items");
    QStringList itemNames;

    while (query.next()) {
        QString name = query.value(0).toString();
        double price = query.value(1).toDouble();
        itemNames << name;
        itemPriceMap[name] = price;
    }

    QCompleter *completer = new QCompleter(itemNames, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->productNameLineEdit->setCompleter(completer);

    connect(ui->productNameLineEdit, &QLineEdit::editingFinished, this, [=]() {
        QString selectedName = ui->productNameLineEdit->text().trimmed();
        if (itemPriceMap.contains(selectedName)) {
            ui->priceLineEdit->setText(QString::number(itemPriceMap[selectedName]));
        }
    });
}
