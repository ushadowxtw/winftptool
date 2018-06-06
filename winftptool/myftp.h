#ifndef MYFTP_H_
#define MYFTP_H_

#include "curl.h"
#include <string>
using namespace std;
typedef	struct onefilemassage
{
        //目前只处理文件类型  和文件名  后期在跟该
	char  F_type;		///文件类型  (l为连接文件/－为普通文件/d为目录/b代表设备)
        char F_tmpname[512];    //.tmp文件明
	char F_name[512];;	///真正的文件名

	onefilemassage()
	{
            memset(F_name,0,512);
            memset(F_tmpname,0,512);
	}

}FileMessage,*lpFileMessage;
class myftp
{
public:
	myftp(void);
	~myftp(void);
public:

	bool iniftp(int ftptype,const char* user,const char* pass,const char* ftpip,int ftpport,int mode=0);
	bool ftp_connect_server();
	int ftp_upload_file(const char *remotepath, const char *localpath);
	int ftp_upload_rmfile(const char *remotepath, const char *localpath);
	int ftp_upmove_dir(const char *remotepath, const char *localpath);
	int start_ftp_upmove_dir(const char *remotepath, const char *localpath,bool isnew=false);
	int ftp_upmove_dir_auth(const char *remotepath, const char *localpath,int relatepathsize,int filenum);
	int ftp_rename_item(const char *oldname, const char *newname);
	int sftp_rename_item(const char *oldname, const char *newname);
   int sftp_Get_dir(const char* localfile);
	string sftp_pwd_item();
	int ftp_delete_file(const char *filename);
	int ftp_onefile_download(const char* remotefile,const char* localfile);
	int ftp_LIST_dir(const char* remotefile,const char* localfile);
//	int ftp_get_filename(const char*  localpath,const char* RemotePath,vector<FtpFileInfor>& SV);
	bool init_curl();
	
	void ftp_disconnect_server();
	int m_dealfilenum;
private:
	long int dealnum;
	int dealtype;//1 下载 2 上传 3 上传与下载  4既不上传也不下载
	char username[64];
	char password[64];
	char ip[24];
	int port;
	char login_info[256];
	char server_url[256];
	CURL *curl;
	int m_mode;//0 ftp 1 sftp
	bool m_use_port_mode;//是否使用主动模式

	bool m_is_del_path;
	
};

#endif

