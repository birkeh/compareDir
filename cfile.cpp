#include "common.h"

#include "cfile.h"

#include <QApplication>

#include <QFile>
#include <QTextStream>


cFile::cFile() :
	m_dir(""),
	m_file(""),
	m_dateTime(QDateTime()),
	m_size(0)
{
}

cFile::cFile(const QString& dir, const QString& file, const QDateTime& dateTime, const qint64& size) :
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

bool cFile::operator==(const cFile& other) const
{
	if(g_bMatchDir)
		if(m_dir != other.m_dir)
			return(false);
	if(g_bMatchFile)
		if(m_file != other.m_file)
			return(false);
	if(g_bMatchDateTime)
		if(m_dateTime != other.m_dateTime)
			return(false);
	if(g_bMatchSize)
		if(m_size != other.m_size)
			return(false);

	return(true);
}

void cFileList::parseLine(const QString& curDir, const QString& line)
{
	QString	date;
	QString	time;
	QString	size;
	QString	file;
	QString	dir;

	date	= line.left(10);
	time	= line.mid(12, 5);
	size	= line.mid(17, 18);
	file	= line.mid(36);
	dir		= curDir;

	size	= size.replace("(", "").replace(")", "").replace(QChar(0xA0), "").replace(" ", "").replace(".", "");

	QDateTime	dateTime	= QDateTime(QDate::fromString(date, "dd.MM.yyyy"), QTime::fromString(time, "hh:mm"));

	add(dir.replace("\\", "/"), file, dateTime, size.toLongLong(), false);
}

void cFileList::parseDir(const QString& dir, QStatusBar* lpStatusBar)
{
	QDir			_dir(dir);

	if(lpStatusBar)
	{
		lpStatusBar->showMessage(_dir.path());
		qApp->processEvents();
	}

	QFileInfoList	fileList	= _dir.entryInfoList(QDir::Files, QDir::Name);

	for(QFileInfoList::iterator i = fileList.begin();i != fileList.end();i++)
		add(_dir.path(), i->fileName(), i->birthTime(), i->size(), false);


	QStringList	dirList	= _dir.entryList(QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDir::Name);

	for(QStringList::iterator i = dirList.begin();i != dirList.end();i++)
	{
		parseDir(dir + QDir::separator() + *i, lpStatusBar);
	}
}

bool cFileList::load(const QString& file, QProgressBar* lpProgressBar)
{
	clear();

	QFile	inFile(file);

	if(!inFile.open(QIODevice::ReadOnly))
		return(false);

	qint64	cur	= 0;
	qint64	max	= inFile.size();

	lpProgressBar->setRange(0, 100);
	QTextStream in(&inFile);

	QString		curDir	= "";

	while(!in.atEnd())
	{
		QString		line = in.readLine();
		cur	+= line.length()+2;

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

		lpProgressBar->setValue(static_cast<int>(cur*100/max));
		qApp->processEvents();
	}

	inFile.close();

	return(true);
}

bool cFileList::load(const QString& dir, QStatusBar* lpStatusBar)
{
	clear();

	parseDir(dir, lpStatusBar);

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
		append(*lpFile);
	}
	return(lpFile);
}

cFile* cFileList::find(const QString& dir, const QString& file, const QDateTime& dateTime, const qint64& size)
{
	for(cFileList::iterator i = begin();i != end();i++)
	{
		if(i->compare(dir, file, dateTime, size))
			return(&(*i));
	}
	return(nullptr);
}

bool cFileList::contains(cFile& file, bool bDir, bool bFile, bool bDate, bool bSize)
{
	g_bMatchDir			= bDir;
	g_bMatchFile		= bFile;
	g_bMatchDateTime	= bDate;
	g_bMatchSize		= bSize;

	return(QList::contains(file));
}

bool cFileList::contains(cFile& file)
{
	return(QList::contains(file));
}
