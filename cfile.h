#ifndef CFILE_H
#define CFILE_H


#include <QMetaType>
#include <QList>
#include <QString>
#include <QDateTime>
#include <QObject>


class cFile : public QObject
{
	Q_OBJECT
public:
	explicit cFile(const QString& dir, const QString& file, const QDateTime& dateTime, const qint64& size, QObject *parent = nullptr);
	virtual ~cFile();

	bool		compare(const QString& dir, const QString& file, const QDateTime& dateTime, const qint64& size);

	QString		dir();
	QString		file();
	QDateTime	dateTime();
	qint64		size();
private:
	QString		m_dir;
	QString		m_file;
	QDateTime	m_dateTime;
	qint64		m_size;
};

Q_DECLARE_METATYPE(cFile*)

class cFileList : public QList<cFile*>
{
public:
	bool		load(const QString& file);
	cFile*		add(const QString& dir, const QString& file, const QDateTime& dateTime, const qint64& size, bool search = true);
	cFile*		find(const QString& dir, const QString& file, const QDateTime& dateTime, const qint64& size);

private:
	void		parseLine(const QString& curDir, const QString& line);
};

#endif // CFILE_H
