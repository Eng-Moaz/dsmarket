#include "MainWindow.h"
#include <QTabWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QStatusBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Supermarket Management System");
    resize(900, 600);

    market.loadSampleData();

    QToolBar* toolbar = addToolBar("Main");
    toolbar->setMovable(false);
    QPushButton* undoButton = new QPushButton("Undo");
    lastOperationLabel = new QLabel;
    toolbar->addWidget(undoButton);
    toolbar->addSeparator();
    toolbar->addWidget(new QLabel(" Last operation: "));
    toolbar->addWidget(lastOperationLabel);

    connect(undoButton, &QPushButton::clicked, this, [this]() {
        showMessage(market.undo());
    });

    QTabWidget* tabs = new QTabWidget;
    tabs->addTab(createProductsTab(), "Products");
    tabs->addTab(createCustomersTab(), "Customers");
    tabs->addTab(createCartTab(), "Shopping Cart");
    tabs->addTab(createRestockTab(), "Restocking");
    tabs->addTab(createReportTab(), "Daily Report");
    setCentralWidget(tabs);

    refreshAll();
}

void MainWindow::showMessage(std::string message) {
    statusBar()->showMessage(QString::fromStdString(message));
    refreshAll();
}

void MainWindow::fillProductTable(QTableWidget* table, std::vector<Product> list) {
    table->setRowCount(list.size());
    for (int i = 0; i < (int)list.size(); i++) {
        table->setItem(i, 0, new QTableWidgetItem(QString::number(list[i].id)));
        table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(list[i].name)));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(list[i].price, 'f', 2)));
        table->setItem(i, 3, new QTableWidgetItem(QString::number(list[i].quantity)));
    }
}

static QTableWidget* createTable(QStringList headers) {
    QTableWidget* table = new QTableWidget;
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    return table;
}

static QComboBox* createPriorityBox() {
    QComboBox* box = new QComboBox;
    box->addItem("Normal", 1);
    box->addItem("Express", 2);
    box->addItem("VIP", 3);
    return box;
}

// ---------------- Products ----------------

QWidget* MainWindow::createProductsTab() {
    QWidget* tab = new QWidget;

    productTable = createTable({"ID", "Name", "Price", "Quantity"});

    productIdInput = new QSpinBox;
    productIdInput->setRange(1, 99999);
    productNameInput = new QLineEdit;
    productPriceInput = new QDoubleSpinBox;
    productPriceInput->setRange(0, 100000);
    productQuantityInput = new QSpinBox;
    productQuantityInput->setRange(0, 100000);

    QFormLayout* form = new QFormLayout;
    form->addRow("ID:", productIdInput);
    form->addRow("Name:", productNameInput);
    form->addRow("Price:", productPriceInput);
    form->addRow("Quantity:", productQuantityInput);

    QPushButton* addButton = new QPushButton("Add");
    QPushButton* searchButton = new QPushButton("Search (Hash Table)");
    QPushButton* searchTreeButton = new QPushButton("Search (BST)");
    QPushButton* priceButton = new QPushButton("Update Price");
    QPushButton* quantityButton = new QPushButton("Update Quantity");
    QPushButton* deleteButton = new QPushButton("Delete");

    QVBoxLayout* side = new QVBoxLayout;
    side->addLayout(form);
    side->addWidget(addButton);
    side->addWidget(searchButton);
    side->addWidget(searchTreeButton);
    side->addWidget(priceButton);
    side->addWidget(quantityButton);
    side->addWidget(deleteButton);
    side->addStretch();

    QVBoxLayout* left = new QVBoxLayout;
    left->addWidget(new QLabel("Products sorted by ID (BST inorder traversal):"));
    left->addWidget(productTable);

    QHBoxLayout* layout = new QHBoxLayout(tab);
    layout->addLayout(left, 3);
    layout->addLayout(side, 1);

    connect(addButton, &QPushButton::clicked, this, [this]() {
        std::string name = productNameInput->text().trimmed().toStdString();
        if (name.empty()) {
            showMessage("Please enter a product name.");
            return;
        }
        showMessage(market.addProduct(productIdInput->value(), name,
                                      productPriceInput->value(), productQuantityInput->value()));
    });

    connect(searchButton, &QPushButton::clicked, this, [this]() {
        Product* p = market.findProduct(productIdInput->value());
        if (p == nullptr) {
            showMessage("Product not found.");
            return;
        }
        productNameInput->setText(QString::fromStdString(p->name));
        productPriceInput->setValue(p->price);
        productQuantityInput->setValue(p->quantity);
        showMessage("Found '" + p->name + "' in the hash table.");
    });

    connect(searchTreeButton, &QPushButton::clicked, this, [this]() {
        Product* p = market.findProductInTree(productIdInput->value());
        if (p == nullptr) {
            showMessage("Product not found.");
            return;
        }
        productNameInput->setText(QString::fromStdString(p->name));
        productPriceInput->setValue(p->price);
        productQuantityInput->setValue(p->quantity);
        showMessage("Found '" + p->name + "' in the BST.");
    });

    connect(priceButton, &QPushButton::clicked, this, [this]() {
        showMessage(market.updatePrice(productIdInput->value(), productPriceInput->value()));
    });

    connect(quantityButton, &QPushButton::clicked, this, [this]() {
        showMessage(market.updateQuantity(productIdInput->value(), productQuantityInput->value()));
    });

    connect(deleteButton, &QPushButton::clicked, this, [this]() {
        showMessage(market.deleteProduct(productIdInput->value()));
    });

    // clicking a row fills the form
    connect(productTable, &QTableWidget::cellClicked, this, [this](int row, int) {
        productIdInput->setValue(productTable->item(row, 0)->text().toInt());
        productNameInput->setText(productTable->item(row, 1)->text());
        productPriceInput->setValue(productTable->item(row, 2)->text().toDouble());
        productQuantityInput->setValue(productTable->item(row, 3)->text().toInt());
    });

    return tab;
}

// ---------------- Customers ----------------

QWidget* MainWindow::createCustomersTab() {
    QWidget* tab = new QWidget;

    // Service line (Queue)
    QGroupBox* serviceBox = new QGroupBox("Customer Service Line (Queue)");
    serviceLineList = new QListWidget;
    serviceNameInput = new QLineEdit;
    serviceNameInput->setPlaceholderText("Customer name");
    serviceStatusLabel = new QLabel;
    QPushButton* enqueueButton = new QPushButton("Enqueue");
    QPushButton* dequeueButton = new QPushButton("Serve Next");

    QHBoxLayout* serviceInputs = new QHBoxLayout;
    serviceInputs->addWidget(serviceNameInput);
    serviceInputs->addWidget(enqueueButton);

    QVBoxLayout* serviceLayout = new QVBoxLayout(serviceBox);
    serviceLayout->addWidget(serviceStatusLabel);
    serviceLayout->addWidget(serviceLineList);
    serviceLayout->addLayout(serviceInputs);
    serviceLayout->addWidget(dequeueButton);

    // Checkout lane (Priority Queue)
    QGroupBox* checkoutBox = new QGroupBox("Express && VIP Checkout Lane (Priority Queue)");
    checkoutLaneList = new QListWidget;
    checkoutNameInput = new QLineEdit;
    checkoutNameInput->setPlaceholderText("Customer name");
    checkoutPriorityInput = createPriorityBox();
    checkoutNextLabel = new QLabel;
    QPushButton* insertButton = new QPushButton("Add");
    QPushButton* serveButton = new QPushButton("Serve Next");

    changeIdInput = new QSpinBox;
    changeIdInput->setRange(1, 99999);
    changeIdInput->setPrefix("ID ");
    changePriorityInput = createPriorityBox();
    QPushButton* changeButton = new QPushButton("Change Priority");

    QHBoxLayout* checkoutInputs = new QHBoxLayout;
    checkoutInputs->addWidget(checkoutNameInput);
    checkoutInputs->addWidget(checkoutPriorityInput);
    checkoutInputs->addWidget(insertButton);

    QHBoxLayout* changeInputs = new QHBoxLayout;
    changeInputs->addWidget(changeIdInput);
    changeInputs->addWidget(changePriorityInput);
    changeInputs->addWidget(changeButton);

    QVBoxLayout* checkoutLayout = new QVBoxLayout(checkoutBox);
    checkoutLayout->addWidget(checkoutNextLabel);
    checkoutLayout->addWidget(checkoutLaneList);
    checkoutLayout->addLayout(checkoutInputs);
    checkoutLayout->addWidget(serveButton);
    checkoutLayout->addLayout(changeInputs);

    QHBoxLayout* layout = new QHBoxLayout(tab);
    layout->addWidget(serviceBox);
    layout->addWidget(checkoutBox);

    connect(enqueueButton, &QPushButton::clicked, this, [this]() {
        std::string name = serviceNameInput->text().trimmed().toStdString();
        if (name.empty()) {
            showMessage("Please enter a customer name.");
            return;
        }
        serviceNameInput->clear();
        showMessage(market.addToServiceLine(name));
    });

    connect(dequeueButton, &QPushButton::clicked, this, [this]() {
        showMessage(market.serveServiceLine());
    });

    connect(insertButton, &QPushButton::clicked, this, [this]() {
        std::string name = checkoutNameInput->text().trimmed().toStdString();
        if (name.empty()) {
            showMessage("Please enter a customer name.");
            return;
        }
        checkoutNameInput->clear();
        showMessage(market.addToCheckoutLane(name, checkoutPriorityInput->currentData().toInt()));
    });

    connect(serveButton, &QPushButton::clicked, this, [this]() {
        showMessage(market.serveCheckoutLane());
    });

    connect(changeButton, &QPushButton::clicked, this, [this]() {
        showMessage(market.changePriority(changeIdInput->value(), changePriorityInput->currentData().toInt()));
    });

    return tab;
}

// ---------------- Cart ----------------

QWidget* MainWindow::createCartTab() {
    QWidget* tab = new QWidget;

    cartTable = createTable({"ID", "Name", "Price", "Quantity", "Subtotal"});
    cartTotalLabel = new QLabel;
    cartIdInput = new QSpinBox;
    cartIdInput->setRange(1, 99999);
    cartIdInput->setPrefix("Product ID ");
    cartQuantityInput = new QSpinBox;
    cartQuantityInput->setRange(1, 1000);
    cartQuantityInput->setPrefix("Qty ");

    QPushButton* addButton = new QPushButton("Add to Cart");
    QPushButton* removeButton = new QPushButton("Remove from Cart");
    QPushButton* checkoutButton = new QPushButton("Checkout");

    QHBoxLayout* inputs = new QHBoxLayout;
    inputs->addWidget(cartIdInput);
    inputs->addWidget(cartQuantityInput);
    inputs->addWidget(addButton);
    inputs->addWidget(removeButton);

    QHBoxLayout* bottom = new QHBoxLayout;
    bottom->addWidget(cartTotalLabel);
    bottom->addStretch();
    bottom->addWidget(checkoutButton);

    QVBoxLayout* layout = new QVBoxLayout(tab);
    layout->addLayout(inputs);
    layout->addWidget(cartTable);
    layout->addLayout(bottom);

    connect(addButton, &QPushButton::clicked, this, [this]() {
        showMessage(market.addToCart(cartIdInput->value(), cartQuantityInput->value()));
    });

    connect(removeButton, &QPushButton::clicked, this, [this]() {
        showMessage(market.removeFromCart(cartIdInput->value()));
    });

    connect(checkoutButton, &QPushButton::clicked, this, [this]() {
        showMessage(market.checkout());
    });

    connect(cartTable, &QTableWidget::cellClicked, this, [this](int row, int) {
        cartIdInput->setValue(cartTable->item(row, 0)->text().toInt());
    });

    return tab;
}

// ---------------- Restock ----------------

QWidget* MainWindow::createRestockTab() {
    QWidget* tab = new QWidget;

    restockList = new QListWidget;
    restockStatusLabel = new QLabel;
    restockIdInput = new QSpinBox;
    restockIdInput->setRange(1, 99999);
    restockIdInput->setPrefix("Product ID ");
    restockAmountInput = new QSpinBox;
    restockAmountInput->setRange(1, 10000);
    restockAmountInput->setPrefix("Amount ");

    QPushButton* requestButton = new QPushButton("Request Restock");
    QPushButton* processButton = new QPushButton("Process Next");

    QHBoxLayout* inputs = new QHBoxLayout;
    inputs->addWidget(restockIdInput);
    inputs->addWidget(restockAmountInput);
    inputs->addWidget(requestButton);
    inputs->addWidget(processButton);

    QVBoxLayout* layout = new QVBoxLayout(tab);
    layout->addLayout(inputs);
    layout->addWidget(restockStatusLabel);
    layout->addWidget(restockList);

    connect(requestButton, &QPushButton::clicked, this, [this]() {
        showMessage(market.requestRestock(restockIdInput->value(), restockAmountInput->value()));
    });

    connect(processButton, &QPushButton::clicked, this, [this]() {
        showMessage(market.processRestock());
    });

    return tab;
}

// ---------------- Report ----------------

QWidget* MainWindow::createReportTab() {
    QWidget* tab = new QWidget;

    salesLabel = new QLabel;
    itemsSoldLabel = new QLabel;
    customersServedLabel = new QLabel;
    lowStockTable = createTable({"ID", "Name", "Price", "Quantity"});

    QFormLayout* form = new QFormLayout;
    form->addRow("Total sales:", salesLabel);
    form->addRow("Items sold:", itemsSoldLabel);
    form->addRow("Customers served:", customersServedLabel);

    QVBoxLayout* layout = new QVBoxLayout(tab);
    layout->addLayout(form);
    layout->addWidget(new QLabel("Low stock (5 or less):"));
    layout->addWidget(lowStockTable);

    return tab;
}

// ---------------- Refresh ----------------

void MainWindow::refreshAll() {
    fillProductTable(productTable, market.getSortedProducts());

    serviceLineList->clear();
    std::vector<Customer> line = market.getServiceLine();
    for (int i = 0; i < (int)line.size(); i++)
        serviceLineList->addItem(QString("%1. %2 (ID %3)").arg(i + 1).arg(QString::fromStdString(line[i].name)).arg(line[i].id));
    serviceStatusLabel->setText(QString::fromStdString(market.serviceLineStatus()));

    checkoutLaneList->clear();
    std::vector<Customer> lane = market.getCheckoutLane();
    for (int i = 0; i < (int)lane.size(); i++) {
        checkoutLaneList->addItem(QString("%1. %2 (ID %3) [%4]")
                                      .arg(i + 1)
                                      .arg(QString::fromStdString(lane[i].name))
                                      .arg(lane[i].id)
                                      .arg(QString::fromStdString(priorityName(lane[i].priority))));
    }
    checkoutNextLabel->setText(QString::fromStdString(market.peekCheckoutLane()));

    std::vector<CartItem> items = market.getCart();
    cartTable->setRowCount(items.size());
    for (int i = 0; i < (int)items.size(); i++) {
        cartTable->setItem(i, 0, new QTableWidgetItem(QString::number(items[i].productId)));
        cartTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(items[i].name)));
        cartTable->setItem(i, 2, new QTableWidgetItem(QString::number(items[i].price, 'f', 2)));
        cartTable->setItem(i, 3, new QTableWidgetItem(QString::number(items[i].quantity)));
        cartTable->setItem(i, 4, new QTableWidgetItem(QString::number(items[i].price * items[i].quantity, 'f', 2)));
    }
    cartTotalLabel->setText("Total: " + QString::number(market.cartTotal(), 'f', 2));

    restockList->clear();
    std::vector<RestockRequest> requests = market.getRestockQueue();
    for (int i = 0; i < (int)requests.size(); i++) {
        Product* p = market.findProduct(requests[i].productId);
        QString name = (p != nullptr) ? QString::fromStdString(p->name) : "(deleted)";
        restockList->addItem(QString("%1. %2  +%3").arg(i + 1).arg(name).arg(requests[i].amount));
    }
    restockStatusLabel->setText(QString::fromStdString(market.restockStatus()));

    salesLabel->setText(QString::number(market.getTotalSales(), 'f', 2));
    itemsSoldLabel->setText(QString::number(market.getItemsSold()));
    customersServedLabel->setText(QString::number(market.getCustomersServed()));
    fillProductTable(lowStockTable, market.getLowStock(5));

    lastOperationLabel->setText(QString::fromStdString(market.lastOperation()));
}
