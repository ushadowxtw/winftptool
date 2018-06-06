#include "stdafx.h"
#include "myftp.h"
#include "PublicFun.h"
#include <sys/stat.h> 
#include <io.h>
#include "BaseLog.h"
myftp::myftp(void)
{
	dealnum=0;
	curl=NULL;
	m_dealfilenum=0;
	memset(username,0,64);
	memset(password,0,64);
	memset(ip,0,24);
	memset(server_url,0,256);
	memset(login_info,0,256);
	m_is_del_path=false;
}


myftp::~myftp(void)
{
}
bool myftp::init_curl()
{
// 	if(curl==NULL)
// 	{
		curl = curl_easy_init();	
// 	}
	return true;

}
void myftp::ftp_disconnect_server()
{
	if( curl == NULL )
		return;
	curl_easy_cleanup(curl);
// 	curl =NULL;
}
struct FtpFile
{
    const char *filename;
    FILE *stream;
};

static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
    struct FtpFile *out=(struct FtpFile *)stream;
    if(out && !out->stream)
    {
        /* open file for writing */
        out->stream=fopen(out->filename, "wb");
        if(!out->stream)
        return -1; /* failure, can't open file to write */
    }

    return fwrite(buffer, size, nmemb, out->stream);
}

int myftp::sftp_Get_dir(const char* localfile)
{
	FILE *listfile = NULL;
    listfile=fopen(localfile ,"wb");
    if(listfile==NULL)
    {
        return -1;
    }

	CURLcode res;
    struct curl_slist *headerlist = NULL;

    char szCmd_1[1024] = {0}; /* SFTP的重命名命令 */
    strcat(szCmd_1, "cd ~ \"\0");
    headerlist = curl_slist_append(headerlist, szCmd_1);

    curl_easy_reset(curl);
	curl_easy_setopt(curl, CURLOPT_VERBOSE,1);
    curl_easy_setopt(curl, CURLOPT_USERPWD, login_info);
    curl_easy_setopt(curl, CURLOPT_URL, server_url);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 0);  /// 严重注意：这句一定不能少！！！
   // curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,"pwd");
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, listfile);

    res = curl_easy_perform(curl);
    curl_slist_free_all (headerlist);
	 fclose(listfile);
    if( res == CURLE_OK )
    {
        return 0;
    }
    else
    {
        return -1;
    }

}

bool myftp::iniftp(int ftptype,const char* user,const char* pass,const char* ftpip,int ftpport,int mode)
{
	m_mode=mode;
	if(ftptype==0)
	{
		WriteLog( "libcurl set PORT mode *** ( 主动模式 )" );
		m_use_port_mode=true;
	}
	
	else
	{
		 WriteLog( "libcurl set PASV mode *** ( 被动模式 )" );
		 m_use_port_mode=false;
	}


	 m_is_del_path=false;


	dealtype=ftptype;
	port=ftpport;
	strcpy(username,user);
	strcpy(password,pass);
	strcpy(ip,ftpip);
	sprintf(server_url, "ftp://%s:%d", ip, port);

	if(mode==0)
    {
        sprintf(server_url, "ftp://%s:%d", ip, port);
    }
    else if(mode==1)
    {
        sprintf(server_url, "sftp://%s:%d", ip, port);
    }
	else
	{
		 sprintf(server_url, "ftp://%s:%d", ip, port);
	}
	sprintf(login_info, "%s:%s", username, password);
	printf("login_info:%s\r\n",login_info);
	return true;
}
bool myftp::ftp_connect_server()
{
	CURLcode res;
	/* 获得curl句柄 */
	curl = curl_easy_init();
	if(curl==NULL||login_info==NULL||server_url ==NULL)
	{
		return false;
	}


	curl_easy_reset(curl);
	/* 显示详细请求信息 */
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        /* 设置用户名和密码 */
	curl_easy_setopt(curl, CURLOPT_USERPWD, login_info);
	/* 设置服务器地址 */
	curl_easy_setopt(curl, CURLOPT_URL, server_url);
	res = curl_easy_perform(curl);

	if( res==0 )
	{
		WriteLog("－－－－－（上传)连接ftp服务器成功！－－－－－");
		return true;
	}
	else
	{
		WriteLog("－－－－－（上传)连接ftp服务器失败！－－－－－");
		return false;
	}
}
int myftp::ftp_upload_rmfile(const char *remotepath, const char *localpath)
{
	WriteLog("ftp_upload_rmfile[%s->%s]", localpath,remotepath);
	CURLcode res;
	
	if(curl==NULL||server_url ==NULL||remotepath==NULL||localpath==NULL)
	{
		return -1;
	}

	int fd;
	FILE * local_file ;
// 	double total_time, size_upload;
	char remote_path[1024];
	memset(remote_path,0,1024);
	struct stat file_info;
	/* 拼接完整的文件名 */
	if(m_mode!=1)
	{
		sprintf(remote_path, "%s/%s.tmp", server_url, remotepath);
	}
	else
	{
		sprintf(remote_path, "%s/~/%s.tmp", server_url, remotepath);
	}
	char remote_path2tmp[1024];
	memset(remote_path2tmp,0,1024);
	sprintf(remote_path2tmp, "/%s.tmp",  remotepath);

	string curltmpfile="";
	CString allfilename=CString(remote_path2tmp);
	CString curlfilename;
	if( GetUnxiFileName( allfilename,curlfilename))
	{
		CString curlfilepapth;
		if( GetFilePath(allfilename,curlfilepapth))
		{
			char curltmpfile1[1024];
			memset(curltmpfile1,0,1024);
			sprintf(curltmpfile1,"%s.in.%s.",curlfilepapth,curlfilename);
			curltmpfile=curltmpfile1;

		}

	}

	
		/* 获得上传文件的大小 */
	fd = open(localpath, 0);
	if( fd == -1 )
	{
 		WriteLog("Open %s failed!\n", localpath);
		return -2;
	}
	fstat(fd, &file_info);
	close(fd);
	if(file_info.st_size>1024*1024*500)
	{
		WriteLog("upload a big file [%d]  , this file is too big !",file_info.st_size);
		remove(localpath);
		return 0;
	}

	/* 打开要上传的文件 */
	local_file = fopen(localpath, "rb");
	if( local_file == NULL )
	{
 		WriteLog("Open %s failed!\n", localpath);
		return -2;
	}
	/* 设置上传标识CURLOPT_UPLOAD(0:下载 1：上传) */
	curl_easy_reset(curl);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1) ;

	curl_easy_setopt(curl, CURLOPT_VERBOSE,1);
	curl_easy_setopt(curl, CURLOPT_USERPWD, login_info);
//	printf("login_info:%s\r\n",login_info);
	/* 设置目标文件路径 */
	curl_easy_setopt(curl, CURLOPT_URL, remote_path);
	
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10*60);
	/* 设置源文件路径 */
	curl_easy_setopt(curl, CURLOPT_INFILE, local_file);
	
	/* 设置上传大小 */
	curl_easy_setopt(curl, CURLOPT_INFILESIZE, file_info.st_size);
	
//	curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, file_info.st_size);//断点续传
	//	curl_easy_setopt(curl, CURLOPT_RESUME_FROM, file_info.st_size);//断点续传
	if(!m_use_port_mode)
	curl_easy_setopt(curl, CURLOPT_FTP_USE_EPSV, 0);//直接被动
	else
	{
	curl_easy_setopt(curl, CURLOPT_FTPPORT, "_");//主动模式
		curl_easy_setopt(curl, CURLOPT_FTP_USE_EPRT, 0);//主动模式
	}


	/* 自动创建服务器上不存在的中途目录 */
	curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1);
	curl_easy_setopt(curl,CURLOPT_NOSIGNAL,1L); //alarm
	/* 开始上传 */
	res = curl_easy_perform(curl);
	
	/* 获取上传速度及时间 */
// 	curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
// 	curl_easy_getinfo(curl, CURLINFO_SIZE_UPLOAD, &size_upload);
	
	fclose(local_file);
	
	if( res == CURLE_OK )
	{
// 		dealfilenum++;
			 WriteLog("upload a file [%s] is success",localpath);
        int retflg=0;
        if(m_mode==1)
        {
            retflg=sftp_rename_item(remote_path2tmp, remotepath);
        }
        else
        {
            retflg=ftp_rename_item(remote_path2tmp, remotepath);
        }
        if(retflg==0)
        {
          //  g_StatDataNum.Insert();
            WriteLog("rename a file [%s] is success",remotepath);
            unlink(localpath);
            return 0;
        }
        else
        {
            WriteLog("false:rename is failed [%s] !",remotepath);
            return -1;
        }
	}
	else
	{
		
		if(ftp_delete_file(remote_path2tmp)==0)
		{
			WriteLog("false: upload file [%s] is failed and delete!",localpath);
		}
		else
		{
			WriteLog("false: upload file [%s] is failed and delete error!",localpath);
	
			if(ftp_delete_file(curltmpfile.c_str())==0)
			{

			}
			else
			{
				WriteLog("false: file [%s]  delete error!",curltmpfile.c_str());
			}
			

		}
		return -1;
	}	
}
/**
 *@brief    sftp_rename_item - 重命名SFTP服务器上的文件或空目录
 * @par     curlhandle(i): CURL句柄
 * @par     oldname(i)   : 旧名称
 * @par     newname(i)   : 新名称
 * @return(o)    : 更名成功返回0；失败返回-1
 */
 
 int myftp::sftp_rename_item(const char *oldname, const char *newname)
{
	string oldnamestr="./";
	oldnamestr+=oldname;

	string newnamestr="./";
	newnamestr+=newname;

    CURLcode res;
    struct curl_slist *headerlist = NULL;

    
    char remote_path[1024];
	memset(remote_path,0,1024);

	/* 拼接完整的文件名 */
	sprintf(remote_path, "%s/%s", server_url, "~");

    
    if( curl == NULL || oldname == NULL || newname == NULL )
    {
        return -1;
    }
    char szCmd_1[1024] = {0}; /* SFTP的重命名命令 */
    strcat(szCmd_1, "rename \"\0");
 
    strcat(szCmd_1,oldnamestr.c_str());
    strcat(szCmd_1,"\" \"\0");
    strcat(szCmd_1,newnamestr.c_str());
    strcat(szCmd_1,"\"\"\0");
   // WriteLog("sftp_rename_item[%s->%s]", oldnamestr.c_str(),newnamestr.c_str());
    headerlist = curl_slist_append(headerlist, szCmd_1);

    curl_easy_reset(curl);

    {
        curl_easy_setopt(curl, CURLOPT_VERBOSE,1);
    }
	//WriteLog("server_url：[%s]",remote_path);
    curl_easy_setopt(curl, CURLOPT_USERPWD, login_info);
    curl_easy_setopt(curl, CURLOPT_URL, remote_path);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 0);  /// 严重注意：这句一定不能少！！！
    curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

    if ( m_use_port_mode )
    {
        // printf( "设置为PORT MODE****************\n" );
        curl_easy_setopt(curl, CURLOPT_FTPPORT, "-");
        curl_easy_setopt(curl, CURLOPT_FTP_USE_EPRT, 0);//主动模式
    }
    else
    {
        curl_easy_setopt(curl, CURLOPT_FTP_USE_EPSV, 0);//被动模式
    }
	
    res = curl_easy_perform(curl);
    curl_slist_free_all (headerlist);

    if( res == CURLE_OK )
    {
        return 0;
    }
    else
    {
        return -1;
    }
}
//获取SFTP目录
string myftp::sftp_pwd_item()
{
	 
	
}





/**
 *@brief    ftp_rename_item - 重命名FTP服务器上的文件或空目录
 * @par     curlhandle(i): CURL句柄
 * @par     oldname(i)   : 旧名称
 * @par     newname(i)   : 新名称
 * @return(o)    : 更名成功返回0；失败返回-1 
*/
int myftp::ftp_rename_item(const char *oldname, const char *newname)
{
	CURLcode res;
	struct curl_slist *headerlist = NULL;
	
	char szCmd_1[256] = "RNFR "; /* FTP的重命名命令 */
	char szCmd_2[256] = "RNTO ";
	
	if( curl == NULL || oldname == NULL || newname == NULL )
		return -1;
	
	strcat(szCmd_1, oldname);
	strcat(szCmd_2, newname);
	
	headerlist = curl_slist_append(headerlist, szCmd_1);
	headerlist = curl_slist_append(headerlist, szCmd_2);
	curl_easy_reset(curl);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
	curl_easy_setopt(curl, CURLOPT_USERPWD, login_info);
	curl_easy_setopt(curl, CURLOPT_URL, server_url);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 0);	/// 严重注意：这句一定不能少！！！
	curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

	
    if ( m_use_port_mode )
    {
        // printf( "设置为PORT MODE****************\n" );
        curl_easy_setopt(curl, CURLOPT_FTPPORT, "-");
        curl_easy_setopt(curl, CURLOPT_FTP_USE_EPRT, 0);//主动模式
    }
    else
    {
        curl_easy_setopt(curl, CURLOPT_FTP_USE_EPSV, 0);//被动模式
    }
	

	res = curl_easy_perform(curl);
	curl_slist_free_all (headerlist);
	
	if( res == CURLE_OK )
		return 0;
	else
		return -1;
}

/**
 *@brief ftp_delete_file - 删除FTP服务器上的某个文件
 *@par curhandle(i): CURL句柄
 *@par filename(i) : 要删除的完整文件名
 *@return(i)   : 删除成功返回0；失败返回-1 
*/
int myftp::ftp_delete_file(const char *filename)
{



	CURLcode res;
	char szCmd[256] = "DELE ";  /* FTP的删除文件命令 */

	struct curl_slist *headerlist = NULL;
	
	if( curl == NULL || filename == NULL )
		return -1;
	
	strcat(szCmd, filename);
	curl_easy_reset(curl);
	curl_easy_setopt(curl, CURLOPT_VERBOSE,1);
	curl_easy_setopt(curl, CURLOPT_URL,server_url);

	headerlist = curl_slist_append(headerlist, szCmd);
	curl_easy_setopt(curl, CURLOPT_USERPWD, login_info);
	curl_easy_setopt(curl, CURLOPT_QUOTE, headerlist);
	if ( m_use_port_mode )
    {
        // printf( "设置为PORT MODE****************\n" );
        curl_easy_setopt(curl, CURLOPT_FTPPORT, "-");
        curl_easy_setopt(curl, CURLOPT_FTP_USE_EPRT, 0);//主动模式
    }
    else
    {
        curl_easy_setopt(curl, CURLOPT_FTP_USE_EPSV, 0);//被动模式
    }

	res = curl_easy_perform(curl);
	curl_slist_free_all (headerlist);
	if( res == CURLE_OK )
	{	
		return 0;
	}
	else
	{
		return -1;
	}
}
/**
 * @brief ftp_upmove_dir - 上传本地某个目录至FTP服务器并删除源目录中的文件（保留源目录结构）
 * @par  curlhandle(i): CRUL句柄
 * @par  remotepath(i): 远程目录路径
 * @par  localpath(i) : 本地目录路径
 * @return(o)    : 成功返回0；失败返回-1
 */
int myftp::ftp_upmove_dir(const char *remotepath, const char *localpath)
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
				int len=0;
				if(!m_is_del_path)
				{
					len=sprintf(remotepathtmp,"%s/%s",remotepath,fileName);
				}
				else
				{
					len=sprintf(remotepathtmp,"%s",remotepath);
				}
				remotepathtmp[len]=0;


				char localpathtmp[512];
				len=sprintf(localpathtmp,"%s\\%s",localpath,fileName);
				localpathtmp[len]=0;


				if(ftp_upmove_dir(remotepathtmp, localpathtmp)!=0)
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

				char localfilename[512];
				int len=sprintf(localfilename,"%s\\%s",localpath,fileName);
				localfilename[len]=0;
				char remotefilename[512];
				len=sprintf(remotefilename,"%s/%s",remotepath,fileName);
				remotefilename[len]=0;
				if(ftp_upload_rmfile(remotefilename,localfilename)!=0)
				{
					filefind.Close();
					return -1;
				}
			}
		}
		filefind.Close();
	}
	else
	{
			WriteLog("false: ftp_upmove_dir open die error [%s]",localpath);
	}
	return 0;
}