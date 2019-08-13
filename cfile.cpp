#include "cfile.h"

#include <QFile>
#include <QTextStream>


cFile::cFile(const QString& dir, const QString& file, const QDateTime& dateTime, const qint64& size, QObject* parent) :
	QObject(parent),
	m_dir(dir),
	m_file(file),
	m_dateTime(dateTime),
	m_size(size)
{
}

cFile::~cFile()
{
}

bool cFile::compare(const QString& dir, const QString& file, const QDateTime& dateTime, const qint64& size)
{
	if(!dir.isNull() && dir != m_dir)
		return(false);
	if(!file.isNull() && file != m_file)
		return(false);
	if(!dateTime.isNull() && dateTime != m_dateTime)
		return(false);
	if(size != -1 && size != m_size)
		return(false);
	return(true);
}

QString cFile::dir()
{
	return(m_dir);
}

QString cFile::file()
{
	return(m_file);
}

QDateTime cFile::dateTime()
{
	return(m_dateTime);
}

qint64 cFile::size()
{
	return(m_size);
}

void cFileList::parseLine(const QString& curDir, const QString& line)
{
	QString	date;
	QString	time;
	QString	size;
	QString	file;

	date	= line.left(10);
	time	= line.mid(12, 5);
	size	= line.mid(17, 18);
	file	= line.mid(36);

	size	= size.replace("(", "").replace(")", "").replace(QChar(0xA0), "").replace(" ", "").replace(".", "");

	QDateTime	dateTime	= QDateTime(QDate::fromString(date, "dd.MM.yyyy"), QTime::fromString(time, "hh:mm"));

	add(curDir, file, dateTime, size.toLongLong(), false);
}

bool cFileList::load(const QString& file)
{
	clear();

	QFile	inFile(file);

	if(!inFile.open(QIODevice::ReadOnly))
		return(false);


	QTextStream in(&inFile);

	QString		curDir	= "";

	while(!in.atEnd())
	{
		QString		line = in.readLine();

		if(line.left(17) == " Volume in drive ")
			continue;
		else if(line.left(25) == " Volume Serial Number is ")
			continue;
		else if(line.isEmpty())
			continue;
		else if(line.left(4) == "    ")
			continue;
		else if(line.contains("<DIR>"))
			continue;
		else if(line.left(14) == " Directory of ")
			curDir	= line.mid(14);
		else
			parseLine(curDir, line);
	}

	inFile.close();

	return(true);
}

cFile* cFileList::add(const QString& dir, const QString& file, const QDateTime& dateTime, const qint64& size, bool search)
{
	cFile*	lpFile	= nullptr;

	if(search)
		lpFile	= find(dir, file, dateTime, size);

	if(!lpFile)
	{
		lpFile	= new cFile(dir, file, dateTime, size);
		append(lpFile);
	}
	return(lpFile);
}

cFile* cFileList::find(const QString& dir, const QString& file, const QDateTime& dateTime, const qint64& size)
{
	for(cFileList::iterator i = begin();i != end();i++)
	{
		cFile*	lpFile	= *i;
		if(lpFile->compare(dir, file, dateTime, size))
			return(lpFile);
	}
	return(nullptr);
}
