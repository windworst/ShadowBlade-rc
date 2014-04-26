#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "strings.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Init UI setting
    this->InitUI();
}

void MainWindow::InitUI()
{
    this->setWindowTitle(QString(PRODUCT_NAME)+" "+QString(PRODUCT_VERSION));
}

MainWindow::~MainWindow()
{
    delete ui;
}
