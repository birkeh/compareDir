#ifndef CSORTFILTERPROXYMODEL_H
#define CSORTFILTERPROXYMODEL_H


#include "cfile.h"

#include <QSortFilterProxyModel>


class cSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	cSortFilterProxyModel(QObject* parent = nullptr);

	cFile*					filterFile();
	void					setFilterFile(cFile* lpFile);

protected:
	bool					filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
	cFile*					m_lpFile;
};

#endif // CSORTFILTERPROXYMODEL_H
