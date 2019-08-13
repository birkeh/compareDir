#include "cmainwindow.h"
#include "ui_cmainwindow.h"


bool fileSort(cFile* file1, cFile* file2)
{
	if(file1->dir().compare(file2->dir(), Qt::CaseInsensitive) < 0)
		return(true);
	if(file1->file().compare(file2->file(), Qt::CaseInsensitive) < 0)
		return(true);
	if(file1->dateTime() < file2->dateTime())
		return(true);
	if(file1->size() < file2->size())
		return(true);

	return(false);
}

cMainWindow::cMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::cMainWindow)
{
	ui->setupUi(this);

	m_lpLeftListModel	= new QStandardItemModel(0, 1);
	ui->m_lpLeftList->setModel(m_lpLeftListModel);

	m_lpRightListModel	= new QStandardItemModel(0, 1);
	ui->m_lpRightList->setModel(m_lpRightListModel);
}

cMainWindow::~cMainWindow()
{
	delete ui;
}

void cMainWindow::on_m_lpLoadListFromFileLeft_clicked()
{
	m_listLeft.load("C:\\Users\\VET0572\\Documents\\github\\build-dir2csv-Desktop_Qt_5_13_0_MinGW_64_bit-Release\\deploy\\photos.txt");
//	std::sort(m_listLeft.begin(), m_listRight.end(), fileSort);
	displayList(m_lpLeftListModel, m_listLeft);
}

void cMainWindow::on_m_lpLoadListFromFileRight_clicked()
{
	m_listRight.load("C:\\Users\\VET0572\\Documents\\github\\build-dir2csv-Desktop_Qt_5_13_0_MinGW_64_bit-Release\\deploy\\photos1.txt");
//	std::sort(m_listRight.begin(), m_listRight.end(), fileSort);
	displayList(m_lpRightListModel, m_listRight);
}

void cMainWindow::displayList(QStandardItemModel* lpModel, cFileList& fileList)
{
	lpModel->clear();

	QStringList				header;
	header << "directory" << "file" << "date/time" << "size";
	lpModel->setHorizontalHeaderLabels(header);

	for(cFileList::iterator i = fileList.begin();i != fileList.end();i++)
	{
		QList<QStandardItem*>	items;
		cFile*					lpFile	= *i;

		items.append(new QStandardItem(lpFile->dir()));
		items.append(new QStandardItem(lpFile->file()));
		items.append(new QStandardItem(lpFile->dateTime().toString()));
		items.append(new QStandardItem(QString::number(lpFile->size())));

		lpModel->appendRow(items);
	}
}
