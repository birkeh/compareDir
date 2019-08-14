#include "common.h"

#include "cmainwindow.h"
#include "ui_cmainwindow.h"

#include <QSettings>
#include <QDir>
#include <QFileDialog>

#include <QElapsedTimer>
#include <QDebug>


//#define TREEVIEW

bool	g_bMatchDir			= true;
bool	g_bMatchFile		= true;
bool	g_bMatchDateTime	= true;
bool	g_bMatchSize		= true;


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
	ui(new Ui::cMainWindow),
	m_lpLeftListModel(nullptr),
	m_lpRightListModel(nullptr),
	m_lpLeftListProxyModel(nullptr),
	m_lpRightListProxyModel(nullptr),
	m_lpProgressBar(nullptr),
	m_initializing(true),
	m_loading(false)
{
	initUI();
	createActions();

	m_initializing	= false;
}

cMainWindow::~cMainWindow()
{
	delete ui;
}

void cMainWindow::closeEvent(QCloseEvent *event)
{
	QSettings	settings;
	settings.setValue("main/width", QVariant::fromValue(size().width()));
	settings.setValue("main/height", QVariant::fromValue(size().height()));
	settings.setValue("main/x", QVariant::fromValue(x()));
	settings.setValue("main/y", QVariant::fromValue(y()));
	if(this->isMaximized())
		settings.setValue("main/maximized", QVariant::fromValue(true));
	else
		settings.setValue("main/maximized", QVariant::fromValue(false));

	QList<qint32>	sizes	= ui->m_lpSplitter->sizes();

	for(int x = 0;x < sizes.count();x++)
		settings.setValue(QString("main/splitter%1").arg(x+1), QVariant::fromValue(sizes[x]));

	event->accept();
}

void cMainWindow::initUI()
{
	ui->setupUi(this);

	m_lpProgressBar			= new QProgressBar(this);
	m_lpProgressBar->setVisible(false);
	ui->m_lpStatusBar->addPermanentWidget(m_lpProgressBar);

	m_lpLeftListModel		= new QStandardItemModel(0, 1);
	m_lpLeftListProxyModel	= new cSortFilterProxyModel(this);
	ui->m_lpLeftList->setModel(m_lpLeftListProxyModel);
	m_lpLeftListProxyModel->setSourceModel(m_lpLeftListModel);

	m_lpRightListModel		= new QStandardItemModel(0, 1);
	m_lpRightListProxyModel	= new cSortFilterProxyModel(this);
	ui->m_lpRightList->setModel(m_lpRightListProxyModel);
	m_lpRightListProxyModel->setSourceModel(m_lpRightListModel);

	QSettings	settings;

	if(!settings.value("main/maximized").toBool())
	{
		qint32		iX		= settings.value("main/x", QVariant::fromValue(-1)).toInt();
		qint32		iY		= settings.value("main/y", QVariant::fromValue(-1)).toInt();
		qint32		iWidth	= settings.value("main/width", QVariant::fromValue(-1)).toInt();
		qint32		iHeight	= settings.value("main/height", QVariant::fromValue(-1)).toInt();

		if(iWidth != -1 && iHeight != -1)
			resize(iWidth, iHeight);
		if(iX != -1 && iY != -1)
			move(iX, iY);
	}

	qint32		iWidth1	= settings.value("main/splitter1", QVariant::fromValue(-1)).toInt();
	qint32		iWidth2	= settings.value("main/splitter2", QVariant::fromValue(-1)).toInt();

	ui->m_lpSplitter->setSizes(QList<int>() << iWidth1 << iWidth2);

	g_bMatchDir			= settings.value("filter/dir", QVariant::fromValue(true)).toBool();
	g_bMatchFile		= settings.value("filter/file", QVariant::fromValue(true)).toBool();
	g_bMatchDateTime	= settings.value("filter/datetime", QVariant::fromValue(true)).toBool();
	g_bMatchSize		= settings.value("filter/size", QVariant::fromValue(true)).toBool();

	ui->m_lpMatchDir->setChecked(g_bMatchDir);
	ui->m_lpMatchFile->setChecked(g_bMatchFile);
	ui->m_lpMatchDateTime->setChecked(g_bMatchDateTime);
	ui->m_lpMatchSize->setChecked(g_bMatchSize);
}

void cMainWindow::createActions()
{
	setToolButtonStyle(Qt::ToolButtonFollowStyle);

	createFileActions();
	createContextActions();

	connect(ui->m_lpLoadListFromFileLeft,			&QToolButton::clicked,					this,	&cMainWindow::onLoadListFromFileLeft);
	connect(ui->m_lpLoadListFromFileRight,			&QToolButton::clicked,					this,	&cMainWindow::onLoadListFromFileRight);

	connect(ui->m_lpLoadListFromDirLeft,			&QToolButton::clicked,					this,	&cMainWindow::onLoadListFromDirLeft);
	connect(ui->m_lpLoadListFromDirRight,			&QToolButton::clicked,					this,	&cMainWindow::onLoadListFromDirRight);

	connect(ui->m_lpLeftList->selectionModel(),		&QItemSelectionModel::selectionChanged,	this,	&cMainWindow::onLeftListSelected);
	connect(ui->m_lpRightList->selectionModel(),	&QItemSelectionModel::selectionChanged,	this,	&cMainWindow::onRightListSelected);

	connect(ui->m_lpMatchDir,						&QCheckBox::clicked,					this,	&cMainWindow::onMatchDirChecked);
	connect(ui->m_lpMatchFile,						&QCheckBox::clicked,					this,	&cMainWindow::onMatchFileChecked);
	connect(ui->m_lpMatchDateTime,					&QCheckBox::clicked,					this,	&cMainWindow::onMatchDateTimeChecked);
	connect(ui->m_lpMatchSize,						&QCheckBox::clicked,					this,	&cMainWindow::onMatchSizeChecked);

	connect(ui->m_lpMarkDifference,					&QToolButton::clicked,					this,	&cMainWindow::onMarkDifference);

	connect(ui->m_lpClearSelectionLeft,				&QToolButton::clicked,					this,	&cMainWindow::onClearSelectionLeft);
	connect(ui->m_lpClearSelectionRight,			&QToolButton::clicked,					this,	&cMainWindow::onClearSelectionRight);
}

void cMainWindow::createFileActions()
{
}

void cMainWindow::createContextActions()
{
}

void cMainWindow::onLoadListFromFileLeft()
{
	QSettings	settings;
	QString		szPath	= settings.value("main/leftFile", QDir::homePath()).toString();
	QString		szFile	= QFileDialog::getOpenFileName(this, tr("Open Text File"), szPath, tr("Text Files (*.txt)"));

	if(szFile.isEmpty())
		return;

	settings.setValue("main/leftFile", QVariant::fromValue(szFile.left(szFile.lastIndexOf("/"))));

	m_loading	= true;
	m_lpProgressBar->setVisible(true);
	ui->m_lpStatusBar->showMessage("loading list...");
	qApp->processEvents();
	m_listLeft.load(szFile, m_lpProgressBar);
	ui->m_lpStatusBar->showMessage("populating list...");
	qApp->processEvents();
	displayList(ui->m_lpLeftList, m_lpLeftListModel, m_listLeft);
	ui->m_lpStatusBar->showMessage("done.", 5000);
	m_lpProgressBar->setVisible(false);
	m_loading	= false;
}

void cMainWindow::onLoadListFromFileRight()
{
	QSettings	settings;
	QString		szPath	= settings.value("main/rightFile", QDir::homePath()).toString();
	QString		szFile	= QFileDialog::getOpenFileName(this, tr("Open Text File"), szPath, tr("Text Files (*.txt)"));

	if(szFile.isEmpty())
		return;

	settings.setValue("main/rightFile", QVariant::fromValue(szFile.left(szFile.lastIndexOf("/"))));

	m_loading	= true;
	m_lpProgressBar->setVisible(true);
	ui->m_lpStatusBar->showMessage("loading list...");
	qApp->processEvents();
	m_listRight.load(szFile, m_lpProgressBar);
	ui->m_lpStatusBar->showMessage("populating list...");
	qApp->processEvents();
	displayList(ui->m_lpRightList, m_lpRightListModel, m_listRight);
	ui->m_lpStatusBar->showMessage("done.", 5000);
	m_lpProgressBar->setVisible(false);
	m_loading	= false;
}

void cMainWindow::onLoadListFromDirLeft()
{
	QSettings	settings;
	QString		szPath	= settings.value("main/leftDir", QDir::homePath()).toString();
	QString		szDir	= QFileDialog::getExistingDirectory(this, tr("Open Directory"), szPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if(szDir.isEmpty())
		return;

	settings.setValue("main/leftDir", QVariant::fromValue(szDir));

	m_loading	= true;
	ui->m_lpStatusBar->showMessage("loading list...");
	qApp->processEvents();
	m_listLeft.load(szDir, ui->m_lpStatusBar);
	ui->m_lpStatusBar->showMessage("populating list...");
	qApp->processEvents();
	displayList(ui->m_lpLeftList, m_lpLeftListModel, m_listLeft);
	ui->m_lpStatusBar->showMessage("done.", 5000);
	m_loading	= false;
}

void cMainWindow::onLoadListFromDirRight()
{
	QSettings	settings;
	QString		szPath	= settings.value("main/rightDir", QDir::homePath()).toString();
	QString		szDir	= QFileDialog::getExistingDirectory(this, tr("Open Directory"), szPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if(szDir.isEmpty())
		return;

	settings.setValue("main/rightDir", QVariant::fromValue(szDir));

	m_loading	= true;
	ui->m_lpStatusBar->showMessage("loading list...");
	qApp->processEvents();
	m_listRight.load(szDir, ui->m_lpStatusBar);
	ui->m_lpStatusBar->showMessage("populating list...");
	qApp->processEvents();
	displayList(ui->m_lpRightList, m_lpRightListModel, m_listRight);
	ui->m_lpStatusBar->showMessage("done.", 5000);
	m_loading	= false;
}

void cMainWindow::onMarkDifference()
{
	qint64			div	= m_lpLeftListModel->rowCount() / 1000;

	ui->m_lpStatusBar->showMessage("processing left list...");

	m_lpProgressBar->setVisible(true);
	m_lpProgressBar->setRange(0, m_lpLeftListModel->rowCount());

	QVariant	white	= QVariant::fromValue(QBrush(QColor(Qt::white)));
	QVariant	green	= QVariant::fromValue(QBrush(QColor(Qt::green)));

	for(int i = 0;i < m_lpLeftListModel->rowCount();i++)
	{
		QStandardItem*	lpItem	= m_lpLeftListModel->item(i, 0);
		cFile*			lpFile	= lpItem->data(Qt::UserRole+1).value<cFile*>();
		QVariant		var;

		if(m_listRight.contains(*lpFile))
		{
			lpItem->setData(green, Qt::BackgroundRole);
			m_lpLeftListModel->setData(m_lpLeftListModel->index(i, 1), green, Qt::BackgroundRole);
			m_lpLeftListModel->setData(m_lpLeftListModel->index(i, 2), green, Qt::BackgroundRole);
#ifndef TREEVIEW
			m_lpLeftListModel->setData(m_lpLeftListModel->index(i, 3), green, Qt::BackgroundRole);
#endif
		}

		if(!(i % div))
		{
			m_lpProgressBar->setValue(i);
			qApp->processEvents();
		}
	}

	div	= m_lpRightListModel->rowCount() / 1000;

	ui->m_lpStatusBar->showMessage("processing right list...");

	m_lpProgressBar->setRange(0, m_lpRightListModel->rowCount());

	for(int i = 0;i < m_lpRightListModel->rowCount();i++)
	{
		QStandardItem*	lpItem	= m_lpRightListModel->item(i, 0);
		cFile*			lpFile	= lpItem->data(Qt::UserRole+1).value<cFile*>();
		QBrush			brush(Qt::white);

		if(m_listLeft.contains(*lpFile))
		{
			lpItem->setData(green, Qt::BackgroundRole);
			m_lpRightListModel->setData(m_lpRightListModel->index(i, 1), green, Qt::BackgroundRole);
			m_lpRightListModel->setData(m_lpRightListModel->index(i, 2), green, Qt::BackgroundRole);
#ifndef TREEVIEW
			m_lpRightListModel->setData(m_lpRightListModel->index(i, 3), green, Qt::BackgroundRole);
#endif
		}

		if(!(i % div))
		{
			m_lpProgressBar->setValue(i);
			qApp->processEvents();
		}
	}

	m_lpProgressBar->setVisible(false);

	ui->m_lpStatusBar->showMessage("done.", 5000);
}

void cMainWindow::onClearSelectionLeft()
{
	ui->m_lpLeftList->clearSelection();
	m_lpRightListProxyModel->setFilterFile(nullptr);
}

void cMainWindow::onClearSelectionRight()
{
	ui->m_lpRightList->clearSelection();
	m_lpLeftListProxyModel->setFilterFile(nullptr);
}

#ifdef TREEVIEW
QStandardItem* insertPath(QTreeView* lpView, QStandardItem* lpRootItem, QString szPath)
{
	if(!lpRootItem)
		return(nullptr);

	QStringList			szPathList	= szPath.split("/");
	QStandardItem*		lpCurRoot	= lpRootItem;
	int					path;
	bool				bFound;
	QString				szPath1		= "";

	for(path = 0;path < szPathList.count();path++)
	{
		bFound	= false;
		for(int x = 0;x < lpCurRoot->rowCount();x++)
		{
			QStandardItem*	lpCurItem	= lpCurRoot->child(x, 0);

			if(!lpCurItem->text().compare(szPathList[path], Qt::CaseInsensitive))
			{
				lpCurRoot	= lpCurItem;
				bFound		= true;
				break;
			}
		}

		if(!bFound)
			break;

		szPath1.append(szPathList[path]+"/");
	}

	for(;path < szPathList.count();path++)
	{
		szPath1.append(szPathList[path]);

		QStandardItem*	lpNewItem	= new QStandardItem(szPathList[path]);

		lpCurRoot->appendRow(lpNewItem);
		lpView->setFirstColumnSpanned(lpRootItem->rowCount()-1, lpRootItem->index(), true);

		lpNewItem->setData(QVariant::fromValue(szPath1), Qt::UserRole+1);
		szPath1.append("/");

		lpCurRoot	= lpNewItem;
	}

	return(lpCurRoot);
}
#endif

void cMainWindow::displayList(QTreeView* lpView, QStandardItemModel* lpModel, cFileList& fileList)
{
	lpView->setUpdatesEnabled(false);

	lpModel->clear();

	QStringList			header;
#ifdef TREEVIEW
	header << "file" << "date/time" << "size";
	QStandardItem*		lpDir	= nullptr;
#else
	header << "path" << "file" << "date/time" << "size";
#endif

	lpModel->setHorizontalHeaderLabels(header);
	QStandardItem*		lpRoot		= lpModel->invisibleRootItem();

	m_lpProgressBar->setRange(0, fileList.count());

	for(cFileList::iterator i = fileList.begin();i != fileList.end();i++)
	{
		QList<QStandardItem*>	items;
		cFile*					lpFile	= &(*i);

#ifdef TREEVIEW
		lpDir	= insertPath(lpView, lpRoot, lpFile->dir());
#else
		items.append(new QStandardItem(lpFile->dir()));
#endif

		items.append(new QStandardItem(lpFile->file()));
		items.append(new QStandardItem(lpFile->dateTime().toString()));
		items.append(new QStandardItem(QString::number(lpFile->size())));
		items[0]->setData(QVariant::fromValue(lpFile), Qt::UserRole+1);
		items[1]->setData(QVariant::fromValue(lpFile), Qt::UserRole+1);
		items[2]->setData(QVariant::fromValue(lpFile), Qt::UserRole+1);
#ifdef TREEVIEW
		items[2]->setTextAlignment(Qt::AlignRight);
#else
		items[3]->setData(QVariant::fromValue(lpFile), Qt::UserRole+1);
		items[3]->setTextAlignment(Qt::AlignRight);
#endif

#ifdef TREEVIEW
		lpDir->appendRow(items);
#else
		lpRoot->appendRow(items);
#endif
	}

#ifdef TREEVIEW
	lpView->expandAll();
#endif
	lpView->resizeColumnToContents(0);
	lpView->resizeColumnToContents(1);
	lpView->resizeColumnToContents(2);
#ifndef TREEVIEW
	lpView->resizeColumnToContents(3);
#endif
	lpView->setUpdatesEnabled(true);
	lpView->update();
}

void cMainWindow::onLeftListSelected(const QItemSelection& selection, const QItemSelection& /*previous*/)
{
	if(m_loading)
		return;

	if(!selection.indexes().count())
		return;

	QModelIndex		index	= selection.indexes()[0];
	if(!index.isValid())
		return;

	cFile*			lpFile	= index.data(Qt::UserRole+1).value<cFile*>();
	if(!lpFile)
		return;

	m_lpLeftListProxyModel->setFilterFile(nullptr);
	m_lpRightListProxyModel->setFilterFile(lpFile);
}

void cMainWindow::onRightListSelected(const QItemSelection& selection, const QItemSelection& /*previous*/)
{
	if(m_loading)
		return;

	if(!selection.indexes().count())
		return;

	QModelIndex		index	= selection.indexes()[0];
	if(!index.isValid())
		return;

	cFile*			lpFile	= index.data(Qt::UserRole+1).value<cFile*>();
	if(!lpFile)
		return;

	m_lpRightListProxyModel->setFilterFile(nullptr);
	m_lpLeftListProxyModel->setFilterFile(lpFile);
}

void cMainWindow::onMatchDirChecked(bool checked)
{
	if(m_initializing)
		return;

	g_bMatchDir	= checked;

	QSettings	settings;
	settings.setValue("filter/dir", QVariant::fromValue(checked));

	if(m_lpLeftListProxyModel->filterFile())
		m_lpLeftListProxyModel->invalidate();
	if(m_lpRightListProxyModel->filterFile())
		m_lpRightListProxyModel->invalidate();
}

void cMainWindow::onMatchFileChecked(bool checked)
{
	if(m_initializing)
		return;

	g_bMatchFile	= checked;

	QSettings	settings;
	settings.setValue("filter/file", QVariant::fromValue(checked));

	if(m_lpLeftListProxyModel->filterFile())
		m_lpLeftListProxyModel->invalidate();
	if(m_lpRightListProxyModel->filterFile())
		m_lpRightListProxyModel->invalidate();
}

void cMainWindow::onMatchDateTimeChecked(bool checked)
{
	if(m_initializing)
		return;

	g_bMatchDateTime	= checked;

	QSettings	settings;
	settings.setValue("filter/datetime", QVariant::fromValue(checked));

	if(m_lpLeftListProxyModel->filterFile())
		m_lpLeftListProxyModel->invalidate();
	if(m_lpRightListProxyModel->filterFile())
		m_lpRightListProxyModel->invalidate();
}

void cMainWindow::onMatchSizeChecked(bool checked)
{
	if(m_initializing)
		return;

	g_bMatchSize	= checked;

	QSettings	settings;
	settings.setValue("filter/size", QVariant::fromValue(checked));

	if(m_lpLeftListProxyModel->filterFile())
		m_lpLeftListProxyModel->invalidate();
	if(m_lpRightListProxyModel->filterFile())
		m_lpRightListProxyModel->invalidate();
}
