#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H


#include "cfile.h"

#include <QMainWindow>
#include <QTreeView>
#include <QStandardItemModel>


namespace Ui {
class cMainWindow;
}

class cMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit cMainWindow(QWidget *parent = nullptr);
	~cMainWindow();

private slots:
	void				on_m_lpLoadListFromFileLeft_clicked();
	void				on_m_lpLoadListFromFileRight_clicked();

private:
	Ui::cMainWindow*	ui;
	QStandardItemModel*	m_lpLeftListModel;
	QStandardItemModel*	m_lpRightListModel;
	cFileList			m_listLeft;
	cFileList			m_listRight;

	void				displayList(QStandardItemModel* lpModel, cFileList& fileList);
};

#endif // CMAINWINDOW_H
