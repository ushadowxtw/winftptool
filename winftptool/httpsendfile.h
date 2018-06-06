#ifndef HTTPSENDFILE_H_
#define HTTPSENDFILE_H_
#include "PublicFun.h"
bool sendfile(ftpInfo oneftpInfo);
//loglevel   1.  打印计数   2. 打印文件路径    return=filenum  -1 error   dstdir如果为空使用ftpuser相对路径。 不为空为绝对路径
int CommSendDir_Custom(char* lpDest, unsigned short sDestPort,char* srcdir,char* dstdir,char* filterfiletype,
char* ftpuser,char* remotedir,int maxfilenum=2000,int loglevel=1,bool sendokdelete=true);
#endif
