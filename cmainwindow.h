#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>

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
	void on_m_lpLoadListFromFileLeft_triggered(QAction *arg1);

	void on_m_lpLoadListFromFileRight_triggered(QAction *arg1);

private:
	Ui::cMainWindow *ui;
};

#endif // CMAINWINDOW_H
