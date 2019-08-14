#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H


#include "cfile.h"
#include "csortfilterproxymodel.h"

#include <QMainWindow>
#include <QTreeView>
#include <QStandardItemModel>
#include <QProgressBar>


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
	void							onLoadListFromFileLeft();
	void							onLoadListFromFileRight();

	void							onLeftListSelected(const QItemSelection& selection, const QItemSelection& previous);
	void							onRightListSelected(const QItemSelection& selection, const QItemSelection& previous);

	void							onMatchDirChecked(bool checked);
	void							onMatchFileChecked(bool checked);
	void							onMatchDateTimeChecked(bool checked);
	void							onMatchSizeChecked(bool checked);

private:
	Ui::cMainWindow*				ui;
	QStandardItemModel*				m_lpLeftListModel;
	QStandardItemModel*				m_lpRightListModel;
	cSortFilterProxyModel*			m_lpLeftListProxyModel;
	cSortFilterProxyModel*			m_lpRightListProxyModel;
	cFileList						m_listLeft;
	cFileList						m_listRight;
	QProgressBar*					m_lpProgressBar;

	bool							m_initializing;
	bool							m_loading;

	void							initUI();
	void							displayList(QTreeView* lpView, QStandardItemModel* lpModel, cFileList& fileList);
	void							createActions();
	void							createFileActions();
	void							createContextActions();

protected:
	void							closeEvent(QCloseEvent* event);
};

#endif // CMAINWINDOW_H
