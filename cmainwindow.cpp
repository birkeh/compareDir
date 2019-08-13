#include "cmainwindow.h"
#include "ui_cmainwindow.h"


cMainWindow::cMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::cMainWindow)
{
	ui->setupUi(this);
}

cMainWindow::~cMainWindow()
{
	delete ui;
}

void cMainWindow::on_m_lpLoadListFromFileLeft_triggered(QAction *arg1)
{
}

void cMainWindow::on_m_lpLoadListFromFileRight_triggered(QAction *arg1)
{
}
