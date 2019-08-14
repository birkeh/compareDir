#ifndef CFILE_H
#define CFILE_H


#include <QMetaType>
#include <QList>
#include <QString>
#include <QDateTime>
#include <QObject>
#include <QProgressBar>
#include <QStatusBar>
#include <QDir>


class cFile
{
public:
	cFile();
	explicit cFile(const QString& dir, const QString& file, const QDateTime& dateTime, const qint64& size);
	virtual ~cFile();

	bool		compare(const QString& dir, const QString& file, const QDateTime& dateTime, const qint64& size);

	QString		dir();
	QString		file();
	QDateTime	dateTime();
	qint64		size();

	void		setMatch(bool dir, bool file, bool dateTime, bool size);

	bool		operator==(const cFile& other) const;
private:
	QString		m_dir;
	QString		m_file;
	QDateTime	m_dateTime;
	qint64		m_size;
};

Q_DECLARE_METATYPE(cFile)
Q_DECLARE_METATYPE(cFile*)

class cFileList : public QList<cFile>
{
public:
	bool		load(const QString& file, QProgressBar* lpProgressBar = nullptr);
	bool		load(const QString& dir, QStatusBar* lpStatusBar = nullptr);
	cFile*		add(const QString& dir, const QString& file, const QDateTime& dateTime, const qint64& size, bool search = true);
	cFile*		find(const QString& dir, const QString& file, const QDateTime& dateTime, const qint64& size);

	bool		contains(cFile& file);
	bool		contains(cFile& file, bool bDir, bool bFile, bool bDate, bool bSize);
private:
	void		parseLine(const QString& curDir, const QString& line);
	void		parseDir(const QString& dir, QStatusBar* lpStatusBar);
};

#endif // CFILE_H
