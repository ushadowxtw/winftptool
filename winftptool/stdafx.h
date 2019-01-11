// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include "afx.h"
#include <iostream>

#ifdef _DEBUG
#pragma comment(lib,"libcurl_imp.lib")

#pragma comment(lib,"zlib.lib")
#else
//#pragma comment(lib,"libcurl.lib")
#pragma comment(lib,"libcurl.lib")
#pragma comment(lib,"libeay32.lib")
#pragma comment(lib,"ssleay32.lib")
#pragma comment(lib,"libssh2.lib")
#pragma comment(lib,"zlib.lib")
#pragma comment(lib,"Crypt32.lib")
//#pragma comment(linker,"/SUBSYSTEM:Windows /ENTRY:mainCRTStartup")
#endif

// TODO: 在此处引用程序需要的其他头文件
