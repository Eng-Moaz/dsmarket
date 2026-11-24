#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>
#include "Supermarket.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private:
    Supermarket market;

    // Products tab
    QTableWidget* productTable;
    QSpinBox* productIdInput;
    QLineEdit* productNameInput;
    QDoubleSpinBox* productPriceInput;
    QSpinBox* productQuantityInput;

    // Customers tab
    QListWidget* serviceLineList;
    QLineEdit* serviceNameInput;
    QLabel* serviceStatusLabel;

    QListWidget* checkoutLaneList;
    QLineEdit* checkoutNameInput;
    QComboBox* checkoutPriorityInput;
    QSpinBox* changeIdInput;
    QComboBox* changePriorityInput;
    QLabel* checkoutNextLabel;

    // Cart tab
    QTableWidget* cartTable;
    QSpinBox* cartIdInput;
    QSpinBox* cartQuantityInput;
    QLabel* cartTotalLabel;

    // Restock tab
    QListWidget* restockList;
    QSpinBox* restockIdInput;
    QSpinBox* restockAmountInput;
    QLabel* restockStatusLabel;

    // Report tab
    QLabel* salesLabel;
    QLabel* itemsSoldLabel;
    QLabel* customersServedLabel;
    QTableWidget* lowStockTable;

    QLabel* lastOperationLabel;

    QWidget* createProductsTab();
    QWidget* createCustomersTab();
    QWidget* createCartTab();
    QWidget* createRestockTab();
    QWidget* createReportTab();

    void showMessage(std::string message);
    void fillProductTable(QTableWidget* table, std::vector<Product> list);
    void refreshAll();
};
