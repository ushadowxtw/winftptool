
#include "stdafx.h"
#include "mywork.h"
//#include "PublicFun.h"
#include "BaseLog.h"
#include "myftp.h"
#include "PublicFun.h"

void my_work()
{

		ftpInfo oneftpInfo;
		oneftpInfo.ip="192.168.1.1";
		oneftpInfo.remotepath="/serverdir/";
		oneftpInfo.upmode=0;//0-FTP 1-SFTP
		oneftpInfo.type=0;//0-主动模式
		oneftpInfo.port=21;
		oneftpInfo.localpath="D:\\data\\";

		myftp upload;
		upload.iniftp(1,oneftpInfo.user, oneftpInfo.pass,oneftpInfo.ip,oneftpInfo.port,oneftpInfo.upmode);
		upload.init_curl();
		int uploadinfo=upload.ftp_upmove_dir( oneftpInfo.remotepath,oneftpInfo.localpath);
		upload.ftp_disconnect_server();




	return;
}