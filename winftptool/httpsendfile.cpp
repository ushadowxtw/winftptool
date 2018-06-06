#include "stdafx.h"
#include "httpsendfile.h"

#include "HttpClient.h"
#include "BaseLog.h"
int http_upmove_dir(HttpClient &oneHttpClient,const char* ftpuser,const char *remotepath, const char *localpath);
bool sendfile(ftpInfo oneftpInfo)
{

	int port=atoi(oneftpInfo.port);
	string ip=oneftpInfo.ip;
	HttpClient oneHttpClient;
	if(oneHttpClient. HTTPOpen((char*)ip.c_str(),port))
	{
				
		if( http_upmove_dir(oneHttpClient,oneftpInfo.user,oneftpInfo.remotepath, oneftpInfo.localpath)!=0)
		{
		  WriteLog("HTTP 传输失败，IP=%s,PORT=%d",ip.c_str(),port);
			return false;
		}

	}
	else
	{
		
			WriteLog("HTTP 传输连接失败，IP=%s,PORT=%d",ip.c_str(),port);
			return false;		
	}
	return true;

}
int http_upmove_dir(HttpClient &oneHttpClient,const char* ftpuser,const char *remotepath, const char *localpath)
{

	CFileFind filefind;                                         //声明CFileFind类型变量
    CString strWildpath ;     //所有文件都列出。
	strWildpath.Format("%s\\*.*",localpath);
	if(filefind.FindFile(strWildpath, 0))	                   //开始检索文件
    {
        BOOL bRet = TRUE;
        while(bRet)
        {
            bRet = filefind.FindNextFile();                 //枚举一个文件
            if(filefind.IsDots())                                 //如果是. 或 .. 做下一个
                continue;
			CString fileName= filefind.GetFileName();

            if(filefind.IsDirectory())//目录上报
            {
				char remotepathtmp[512];
				int len=sprintf(remotepathtmp,"%s/%s/",remotepath,fileName);
				remotepathtmp[len]=0;
				char localpathtmp[512];
				len=sprintf(localpathtmp,"%s\\%s",localpath,fileName);
				localpathtmp[len]=0;
				if(http_upmove_dir(oneHttpClient,ftpuser,remotepathtmp, localpathtmp)!=0)
				{
					filefind.Close();
					return -1;
				}
			}
			else//文件上报
			{
				
				int pos=fileName.ReverseFind('.');
				CString strLastName=fileName.Right(fileName.GetLength()-pos-1);
				if(strLastName.Compare("tmp")==0||strLastName.Compare("temp")==0)
				{
					continue;
				}
				char myfilename[512];
				int len=sprintf(myfilename,"%s",fileName);
				myfilename[len]=0;


				char localfilename[512];
				len=sprintf(localfilename,"%s\\%s",localpath,fileName);
				localfilename[len]=0;
				char remotefilename[512];
				len=sprintf(remotefilename,"%s/%s",remotepath,fileName);
				remotefilename[len]=0;
				//if(ftp_upload_rmfile(remotefilename,localfilename)!=0)

				char*poutbuf;
				unsigned int datalen;
				oneHttpClient.HTTPSendFile2Server(localfilename,(char*)ftpuser,myfilename,(char*)remotepath,&poutbuf,datalen);
				if(datalen==2&&strncmp(poutbuf,"ok",2)==0)
				{
						WriteLog("success: upload file [%s] [%s]is success and delete!",localfilename,remotepath);
						remove(localfilename);
				}
				else
				{
					WriteLog("false: upload file [%s] [%s]is failed ",localfilename,remotepath);
					return -1;
				}
				//printf("out==%s,len==%d\n",poutbuf,datalen);
			}
		}
		filefind.Close();
	}
	return 0;


}

/*
 * @param : lpDest           目的IP
 * @param : sDestPort        目的PORT
 * @param : srcdir           本地文件源目录
 * @param : dstdir           目的绝对路径（一般不填）
 * @param : filterfiletype   文件类型
 * @param : ftpuser          ftp用户名
 * @param : remotedir        远程拼接路径
 * @param : maxfilenum       最大传文件数量远程拼接路径
 * @param : loglevel         日志等级
 * @param : sendokdelete     上传成功后是否删除本地文件
 */
int CommSendDir_Custom(char* lpDest, unsigned short sDestPort,char* srcdir,char* dstdir,char* filterfiletype,
char* ftpuser,char* remotedir,int maxfilenum,int loglevel,bool sendokdelete)
{
	if(strlen(dstdir)>0)
	{
		ftpuser="";
	}


	int retvalue=-1;
	vector<string> array;
	vector<CString> filearr;
	sdzwsearch_dir(srcdir,filterfiletype,filearr,maxfilenum);
//	sdzwsearch_dir((char*)srcdir,array,(char*)filterfiletype,0,maxfilenum);
	if(array.size()==0)
	{
		if(loglevel>=1)
			 WriteLog("senddir=%s,filenum=0",srcdir);
		return 0;
	}


	HttpClient hc;
	bool flag=false;
	flag=hc.HTTPOpen(lpDest,sDestPort);
	if(!flag)
	{
		WriteLog("senddir=%s HTTPOpen error,filenum=%d",srcdir,array.size());
		return -1;
	}

	retvalue=0;
	for(int i=0;i<array.size();i++)
	{
		string  srcfilename=array[i];

		char dstfilename[512]={0};
		sprintf(dstfilename,"%s%s",dstdir,srcfilename.c_str()+strlen(srcdir));
	
	//	RemoveTimeDir( dstfilename );

// 		g_statistics.AddCount((char*)srcfilename.c_str());

		//if(loglevel>2)
		//WriteSendLog("sendfile","CommSendDir sendfile=%s ",srcfilename.c_str());

		char* pbuf=NULL;
		unsigned int buflen=0;
		if(hc.HTTPSendFile2Server((char*)srcfilename.c_str(),ftpuser,(char*)dstfilename,remotedir,&pbuf,buflen))
		{
			if(stricmp(pbuf,"ok")==0)
			{
			//	if(loglevel>1)
				//	 WriteSendLog("sendfile","CommSendDir sendfile=%s ok!!!",srcfilename.c_str());
				retvalue++;
				if(sendokdelete)
					remove(srcfilename.c_str());
			}
			else
			{
				// WriteSendLog("sendfile","CommSendDir1 sendfile=%s error!!!",srcfilename.c_str());
				 break;
			}
		}
		else
		{
			 WriteLog("sendfile","CommSendDir sendfile=%s error!!!",srcfilename.c_str());
			 break;
		}

		Sleep(1);
	}
	if(loglevel>=1)
		WriteLog("CommSendDir=%s,searchnum=%d,sendfileoknum=%d",srcdir,array.size(),retvalue);

	hc.HTTPClose();



	return retvalue;
}