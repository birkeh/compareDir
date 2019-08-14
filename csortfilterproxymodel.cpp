#include "csortfilterproxymodel.h"


cSortFilterProxyModel::cSortFilterProxyModel(QObject* parent) :
	QSortFilterProxyModel(parent),
	m_lpFile(nullptr)
{
}

void cSortFilterProxyModel::setFilterFile(cFile* lpFile)
{
	m_lpFile	= lpFile;

	invalidateFilter();
}

cFile* cSortFilterProxyModel::filterFile()
{
	return(m_lpFile);
}

bool cSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	if(!m_lpFile)
		return(true);

	bool		bValid		= false;

	QModelIndex	index0		= sourceModel()->index(sourceRow, 0, sourceParent);
	cFile*		lpFile		= sourceModel()->data(index0, Qt::UserRole+1).value<cFile*>();

	if(!lpFile)
		return(true);

	return(*m_lpFile == *lpFile);
}
