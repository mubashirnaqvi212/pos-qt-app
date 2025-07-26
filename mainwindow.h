#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCompleter>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddProductClicked();
    void onClearClicked();
    void onSellButtonClicked();
    void onHistoryButtonClicked();
    void toggleDashboard();
    void onMenuManagerButtonClicked();


private:
    Ui::MainWindow *ui;
    void updateTotal();
    void printReceipt(const QList<QList<QString>> &items, double grandTotal);
    void setupItemAutoSuggest();  // New function to set up auto-suggestion
    QMap<QString, double> itemPriceMap;  // Stores item names with prices
};
#endif // MAINWINDOW_H
