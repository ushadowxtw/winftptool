#include "StdAfx.h"
#include "PublicFun.h"
#include <sys/stat.h> 
#include <io.h>

string g_rootpth="";
int CreatePath(const char* folder,bool all)
{
	 int errcount = 0;
	 
	 if(_access(folder, 0) == 0)
		 ::SetFileAttributes(folder, 0);
	 
	 if(all)
	 {
		 TCHAR path[MAX_PATH];
		 memset(path,0,MAX_PATH*sizeof(TCHAR));
		 
		 for(int i= 0 ; folder[i] ; ++i)
		 {
			 if(folder[i] == '\\') 
				 errcount += CreatePath(path,FALSE);
			 path[i] = folder[i];
		 }
		 errcount += CreatePath(path,FALSE);
	 }
	 else if(_access(folder,0) != 0 )
	 {
		 if(!::CreateDirectory(folder, NULL))
			 errcount ++; 
	 }
	 
	 return errcount;
}
void SplitString(const string &src_str, const string &separator,
				 vector<string> &part_strings)
{
	if (src_str.empty() || separator.empty())
		return;

	string tmp_str;
	const char *head_pos = src_str.c_str();
	const char *tail_pos = NULL;
	unsigned int separator_len = separator.size();

	while (head_pos != NULL)
	{
		tail_pos = strstr(head_pos, separator.c_str());

		if (tail_pos != NULL)
		{
			tmp_str = string(head_pos, tail_pos - head_pos);
			part_strings.push_back(tmp_str);

			head_pos = tail_pos + separator_len;
			tail_pos = NULL;
		}
		else
		{
			if (strlen(head_pos) > 0)
			{
				part_strings.push_back(head_pos);
			}

			break;
		}
	}
}

CString GetApiStr(const char* node , const char* key, const char* defaultvalue,const char* inifilepath)
{

	CString ReturnStr;
	CString DefaultStr;
	DefaultStr.Format("%s","null");
	CString session;
	session.Format("%s",node);
	CString keystr;
	keystr.Format("%s",key);
	::GetPrivateProfileString(session,keystr,DefaultStr,ReturnStr.GetBuffer(100),100,inifilepath);

	ReturnStr.ReleaseBuffer();
	if(ReturnStr.Compare("null")==0||(ReturnStr.IsEmpty()))
	{
		
		return "";	
	}
	ReturnStr.TrimLeft();
	ReturnStr.TrimRight();
	return ReturnStr;
}

CString GetFileDir1(CString filepath)
{
	CString res;
	int pos1;
	pos1=filepath.ReverseFind('\\');
	res=filepath.Left(pos1);
	return res;
}
 
CString GetCurWorkDir()
{
	CString retstr;

	char	ReSetup[MAX_PATH];
	memset(ReSetup, 0, MAX_PATH);
	GetModuleFileNameA(NULL, ReSetup, MAX_PATH);
	CString tmpstr=GetFileDir1(ReSetup);
	


	return tmpstr;
}

/**

 * check a file is a directory or  file

 @return 0 is a directory, -1 not exit ,1 file

 */

 bool IsDirectory(const char* filename)
{

  DWORD dwAttr = ::GetFileAttributes(filename);  //得到文件属性
  if (dwAttr == 0xFFFFFFFF)    // 文件或目录不存在
    return false;
    else if (dwAttr&FILE_ATTRIBUTE_DIRECTORY)  // 如果是目录

    return true; 

  else
    return false;

}
 //判断文件是否存在
 bool FileIsExist(const char* filename)
 {
	 if(_access(filename,0)!=-1)
	 {
		return true;
	 }
	return false;
 }
 //获取文件大小
unsigned int GFileSize(string filename)
{
    struct stat file;

    int result=stat(filename.c_str(),&file);

    if(result==0)
    {
        return file.st_size;
    }
    else
    {
        return -1;
    }

    return -1;
}
//读文件
bool ReadFromFile(const char * filename,char* filebuf, int len)
{

    FILE *pfile=fopen(filename,"rb");
    if(pfile)
    {
        fread(filebuf,1,len,pfile);
        fclose(pfile); 
    }
    return true;
}
/**
 * 拆分字符串
 * @param buf 
 * @param len 
 * @param e 
 * @param sVector 
 * @param isdelempty 
 */
void SplitStoV(char* buf,int len,char* e, std::vector<string>& sVector, bool isdelempty)
{
    sVector.clear();
    int count=0;
    char * tp=buf;
    int splitword=strlen(e);
    char * p=(char*)strstr(buf,e);
    while(p!=NULL)
    {
        *p = 0;
        int off=p-tp;
        if(! ( isdelempty && off==0 ) )
        {
            sVector.push_back(tp);
            count++;
        }
        tp=p+splitword;
        len-=(off+splitword);
        p=(char*)strstr(tp,e);
    }
    if(len>0)
    {
        sVector.push_back(tp);
        count++;
        return;
    }
    if( !isdelempty && len == 0 )
    {
        sVector.push_back(tp);
        count++;
    }
    return;
}
CString GetRunPath()
{
	CString sPath;
	::GetModuleFileName (NULL,sPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	sPath.ReleaseBuffer();
	int nPos=sPath.ReverseFind('\\');
	sPath=sPath.Left(nPos);
	return sPath;
}
/**
 * 分割符前的字符串
 *@param str 原串 
 *@param sp  分割串 
*/
string GetForntStr(string str,string sp)
{
	int p=str.find(sp);
	if(p!=string::npos)
	{
		return str.substr(0,p);
	}
	return "";
}
/**
 *便利文件
 *@P strDir 要便利的起始目录
 *@P filearr 便利后的输出
 *@P fixstr filetype
*/

void TravelFolder(CString strDir,CString fixstr,vector<CString> &filearr)
{
    CFileFind filefind;                                         //声明CFileFind类型变量
    CString strWildpath = strDir + _T("\\*.*");     //所有文件都列出。
    if(filefind.FindFile(strWildpath, 0))	                   //开始检索文件
    {
        BOOL bRet = TRUE;
        while(bRet)
        {
            bRet = filefind.FindNextFile();                 //枚举一个文件
            if(filefind.IsDots())                                 //如果是. 或 .. 做下一个
                continue;
            if(!filefind.IsDirectory())                          //不是子目录，把文件名打印出来
            {
				CString fileName= filefind.GetFileName();
				int pos=fileName.ReverseFind('.');
				CString strLastName=fileName.Right(fileName.GetLength()-pos-1);
				if(strLastName.Compare("tmp")==0||strLastName.Compare("temp")==0)
				{
					continue;
				}
				if(strLastName.Compare(fixstr)==0||fixstr.CompareNoCase("all")==0)
				{
					 CString strTextOut;
					 if(strDir.ReverseFind('\\')==(strDir.GetLength()-1))
					 {
						strTextOut = strDir  +fileName;
					 }
					 else
					 {
						strTextOut = strDir + CString(_T("\\")) +fileName;
					 }
					 filearr.push_back(strTextOut);
				}
            }
            else                                                    //如果是子目录，递归调用该函数
            {
                CString strTextOut = strDir + CString(_T("\\")) + filefind.GetFileName();

                TravelFolder(strTextOut,fixstr,filearr);//递归调用该函数打印子目录里的文件
            }
        }
        filefind.Close();
    }
}
void TraveloneFolder(CString strDir,CString fixstr,vector<CString> &filearr)
{
    CFileFind filefind;                                         //声明CFileFind类型变量
    CString strWildpath = strDir + _T("\\*.*");     //所有文件都列出。
    if(filefind.FindFile(strWildpath, 0))	                   //开始检索文件
    {
        BOOL bRet = TRUE;
        while(bRet)
        {
            bRet = filefind.FindNextFile();                 //枚举一个文件
            if(filefind.IsDots())                                 //如果是. 或 .. 做下一个
                continue;
            if(!filefind.IsDirectory())                          //不是子目录，把文件名打印出来
            {
				CString fileName= filefind.GetFileName();
				int pos=fileName.ReverseFind('.');
				CString strLastName=fileName.Right(fileName.GetLength()-pos-1);
				if(strLastName.Compare("tmp")==0||strLastName.Compare("temp")==0)
				{
					continue;
				}
				if(strLastName.Compare(fixstr)==0||fixstr.CompareNoCase("all")==0)
				{
					 CString strTextOut;
					 if(strDir.ReverseFind('\\')==(strDir.GetLength()-1))
					 {
						strTextOut = strDir  +fileName;
					 }
					 else
					 {
						strTextOut = strDir + CString(_T("\\")) +fileName;
					 }
					 filearr.push_back(strTextOut);
				}
            }
            else                                                    //如果是子目录，递归调用该函数
            {
                CString strTextOut = strDir + CString(_T("\\")) + filefind.GetFileName();

                TravelFolder(strTextOut,fixstr,filearr);//递归调用该函数打印子目录里的文件
            }
        }
        filefind.Close();
    }
}
int search_direct(const char *dir,std::vector<string>& files)
{
	int retnum=0;
	CFileFind filefind;                                         //声明CFileFind类型变量
    CString strWildpath;
	strWildpath.Format("%s\\*.*", dir);     //所有文件都列出。
    if(filefind.FindFile(strWildpath, 0))	                   //开始检索文件
    {
        BOOL bRet = TRUE;
        while(bRet)
        {
            bRet = filefind.FindNextFile();                 //枚举一个文件
            if(filefind.IsDots())                                 //如果是. 或 .. 做下一个
                continue;
            if(filefind.IsDirectory())                          //不是子目录，把文件名打印出来
            {
				CString fileName= filefind.GetFileName();
				string filenamestr=fileName;
				files.push_back(filenamestr);
				retnum++;
            }
            else                                                    //如果是子目录，递归调用该函数
            {
               continue;
            }
        }
        filefind.Close();
    }
	return retnum;
}
//删除目录
 void DeleteDirectories(CString csPath)  
{     
	  CFileFind finder;  
	 CString tempPath;  
	 tempPath.Format("%s%s", csPath, "//*.*");  
	 BOOL bWork = finder.FindFile(tempPath);  
	 while(bWork)  
	 {         
	      bWork = finder.FindNextFile();  
	      if(!finder.IsDots())  
		  {  
		        if(finder.IsDirectory())  
		        {  
		            DeleteDirectories(finder.GetFilePath());  
		        }  
		        else  
		        {  
		            DeleteFile(finder.GetFilePath());  
		        }  
		   }  
	 }  
	 finder.Close();  
	 RemoveDirectory(csPath);  
 } 
void copypath(CString srcDir,CString dstDir)
{
	CString fix;
	fix.Format("all");
	vector<CString> filearr;
	TravelFolder(srcDir,fix,filearr);
	int arrsize=filearr.size();
	for(int n=0;n<arrsize;n++)
	{
		CString file;
		file.Format("%s",filearr.at(n));
		file.Replace(srcDir,dstDir);
		CString filepath;
		bool flg=GetFilePathWin(file,filepath);
		if(flg)
		{
			CreatePath(filepath);
		}
		rename(filearr.at(n),file);
	}
	return;

}
//写文件
bool WriteToFile(const char * filename,const char * buf,unsigned int len)
{
    FILE *pfile=fopen(filename,"wb");
    if(pfile)
    {
        fwrite(buf,1,len,pfile);
        fclose(pfile);
    }
    else
    {
        return false;
    }
    return true;
}
//判断是否全是字母
bool AllIsAlpha(const char* p)
{
	int len=strlen(p);
	int i=0;
	for(;i<len;i++)
	{
	//	if(!isalpha(p[i]))
		//if(p[i]<'A'||(p[i]>'Z'&&p[i]<'a')||p[i]>'z')
		//	break;
		if(p[i]<'0'||(p[i]>'9'&&p[i]<'A')||(p[i]>'Z'&&p[i]<'a')||p[i]>'z')
			break;
	}
	if(i<len)
	{
		return false;
	}
	return true;

}
//获取文件名
bool GetSonFileName(const CString allfilename,CString &filename)
{

		int n=allfilename.ReverseFind('\\');
		if(n==-1)//未找到
		{
			filename=allfilename;
		}
		else
		{
			filename=allfilename.Mid(n+1);
		}
	return true;

}
bool GetUnxiFileName(const CString allfilename,CString &filename)
{

		int n=allfilename.ReverseFind('/');
		if(n==-1)//未找到
		{
			filename=allfilename;
		}
		else
		{
			filename=allfilename.Mid(n+1);
		}
	return true;

}
//"/"分隔
bool GetFilePath(const CString allfilename,CString &filepapth)
{

		int n=allfilename.ReverseFind('/');
		if(n==-1)//未找到
		{
			return false;
		}
		else
		{
			filepapth=allfilename.Left(n);
		}
	return true;

}
//"/"分隔
bool GetFilePathWin(const CString allfilename,CString &filepapth)
{

		int n=allfilename.ReverseFind('\\');
		if(n==-1)//未找到
		{
			return false;
		}
		else
		{
			filepapth=allfilename.Left(n);
		}
	return true;

}
//utf-8转unicode
wchar_t *Utf_8ToUnicode(char* szU8) 
 {
   //UTF8 to Unicode
    //由于中文直接复制过来会成乱码，编译器有时会报错，故采用16进制形式
     
    //预转换，得到所需空间的大小
    int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);
    //分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间
    wchar_t* wszString = new wchar_t[wcsLen + 2];
    //转换
    ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);
    //最后加上'\0'
    wszString[wcsLen] = '\0';
    return wszString;
 }
//unicode转utf-8
char* UnicodeToUTF_8First(CString str)
{   
	int u8Len =WideCharToMultiByte(CP_UTF8, NULL,CStringW(str),str.GetLength(), NULL, 0, NULL, NULL);
    char* szU8 = new  char[u8Len + 1];
    WideCharToMultiByte(CP_UTF8, NULL, CStringW(str), str.GetLength(), szU8, u8Len, NULL, NULL);
    szU8[u8Len] = '\0';
    return szU8;  
}
//UTF-8到GB2312的转换
char* U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len+1];
	memset(str, 0, len+1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if(wstr) delete[] wstr;
	return str;
}

//GB2312到UTF-8的转换
char* G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len+1];
	memset(str, 0, len+1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if(wstr) delete[] wstr;
	return str;
}

/*
*	判断字符串内指定字符数是否为utf8
*	输入
*		const char *str: 要检查的字符串
*		long length: 要检查的字符串的长度
*		int nWords: 要检查的最大字符数
*/
bool IsWordsUTF8(const char *str, long length, int nWords)
{
	int i;
	int nBytes=0;//UFT8可用1-6个字节编码,ASCII用一个字节
	unsigned char chr;
	BOOL bAllAscii=TRUE; //如果全部都是ASCII, 说明不是UTF-8
	
	if ( -1 == nWords)
	{
		nWords = (int)length;
	}

	for(i=0;i<length;i++)
	{
		if (0 >= nWords)
		{
			return !bAllAscii;
		}		

		chr= *(str+i);
		if( (chr&0x80) == 0 ) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx;中文ASCII编码可能最高位为1
			nWords--;
		else
			bAllAscii= FALSE;

		if(nBytes==0) //如果不是ASCII码,应该是多字节符,计算字节数
		{
			if(chr>=0x80)
			{
				nWords--;
				
				if(chr>=0xFC&&chr<=0xFD)//1111 1100		1111 1101
					nBytes=6;
				else if(chr>=0xF8)//1111 1000
					nBytes=5;
				else if(chr>=0xF0)//1111 0000
					nBytes=4;
				else if(chr>=0xE0)//1110 0000
					nBytes=3;
				else if(chr>=0xC0)//1100 0000
					nBytes=2;
				else
				{
					return FALSE;
				}

				nBytes--;
			}
		}
		else //多字节符的非首字节,应为 10xxxxxx
		{
			if( (chr&0xC0) != 0x80 )
			{
				return FALSE;
			}

			nBytes--;
		}
	}
	if( nBytes > 0 ) //违返规则
	{
		return FALSE;
	}
	if( bAllAscii ) //如果全部都是ASCII, 说明不是UTF-8
	{
		return FALSE;
	}
	return TRUE;
}