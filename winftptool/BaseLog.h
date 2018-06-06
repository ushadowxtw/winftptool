#pragma once

#include "lock.h"
#include "io.h"
#include "direct.h"
#include <SYS\STAT.H>
class BaseLog:public BaseLock
{
public:
	BaseLog(void);
	~BaseLog(void);

public: 
	void WriteLog(CString &str);


private:
	void  GetLogFilename(char* log_Path, int len);
	unsigned int getfilesize(char* file);

};

extern BaseLog g_Log;
void WriteLog( const char *fm, ...);