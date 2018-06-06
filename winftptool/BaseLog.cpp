#include "StdAfx.h"
#include "BaseLog.h"
#include "PublicFun.h"
BaseLog g_Log;


BaseLog::BaseLog(void)
{
}


BaseLog::~BaseLog(void)
{
}



void BaseLog::GetLogFilename(char* log_Path, int len)
{
	ASSERT(len >= MAX_PATH);
	strcpy(log_Path, GetCurWorkDir());

	if (_access(log_Path, 0) !=0)
	//	_mkdir(log_Path);
		 CreatePath(log_Path);
	strcat(log_Path, "\\log\\ftpmanager\\");
	if(_access(log_Path,0) != 0)
		//_mkdir(log_Path);
	 CreatePath(log_Path);


	char todaytime[20];
	memset(todaytime,0,20);
	time_t ttNow = time(0);
	struct tm *tmNow = localtime(&ttNow);
	strftime(todaytime, sizeof(todaytime), "%Y%m%d", tmNow);

	char filename[100];
	memset(filename,0,100);


	sprintf(filename,"ftpmanager_%s.log",todaytime);



	strcat(log_Path, filename);

	return;
}

unsigned int BaseLog::getfilesize(char* file)
{
	unsigned int iresult;
	struct _stat buf;
	iresult = _stat(file,&buf);
	if(iresult == 0)
	{
		return buf.st_size;
	}
	return 0;
}
void BaseLog::WriteLog( CString &str)
{
	lock();
	
	#ifdef _DEBUG
	OutputDebugString(str);
	#endif

	// 写日志到文件
	str.Format("%s\t%s\r\n", CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S"),(CString)str);
	char fname[MAX_PATH];
	GetLogFilename(fname, MAX_PATH);
	int filesize=getfilesize(fname);
	if (filesize<20*1024*1024)	//日志过长
	{
		CFile logfile;
		DWORD mode = CFile::modeWrite;
		if (_access(fname, 0) != 0)
		{
			mode |= CFile::modeCreate;
		}


			logfile.Open(fname, mode);
			if (logfile.m_hFile != CFile::hFileNull)
			{

				// 判断文件大小
				if (logfile.GetLength() < 20*1024*1024)		//20M
				{

					logfile.SeekToEnd();
					logfile.Write((LPCTSTR)str, str.GetLength());
				}

				logfile.Close();	
			}
	}

	unlock();
}

void WriteLog( const char *fm, ...)
{
	CString str;
	va_list args;
	va_start( args, fm );
	str.FormatV(fm, args);
	va_end( args );

	g_Log.WriteLog(str);
	printf("%s\r\n",str);

}