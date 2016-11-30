#include "IniParser.h"
#include <string.h>
#include <assert.h>

#ifdef _WIN32
#include <Windows.h>
#include <io.h>
#else

#define _access access
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>

#endif

IniParser::IniParser()
{
	m_FileID = NULL;
}

IniParser::~IniParser()
{
	if (m_FileID)
	{
		fprintf(stderr, "Close function is not called. Destruction will do it.\n");
		fclose(m_FileID);
		m_FileID = 0;
	}
}

/************************************************************************
* @brief 设置 INI 文件
* @param[in] *filename INI文件
* @return 是否设置成功（文件不存在时返回失败）
* @remark 当采用默认参数时，将从程序所在目录读取config.ini
* @warning 采用r+方式打开，如果需要在末尾追加，请参看ReOpen
************************************************************************/
bool IniParser::Open(const char *filename)
{
	if (filename)
	{
		// 判断文件是否存在
		if (-1 == _access(filename, 0))
			return false;
		int nLength = strlen(filename);
		memset(m_FileName, 0, MAX_PATH);
		memcpy(m_FileName, filename, nLength > MAX_PATH ? MAX_PATH : nLength);
		m_FileID = fopen(m_FileName, "r+");
		return (NULL != m_FileID);
	}
	// 默认从程序所在目录读取"config.ini"
	char buf[MAX_PATH] = { 0 };
	char *end = GetCurrentPath(buf);
	memcpy(end, "config.ini", strlen("config.ini"));
	bool temp = Open(buf);
	return temp;
}

// 重新打开文件
bool IniParser::ReOpen(const char *mode)
{
	if (m_FileID)
	{
		fclose(m_FileID);
		m_FileID = fopen(m_FileName, mode);
	}
	return (NULL != m_FileID);
}

// 关闭文件
void IniParser::Close()
{
	if (m_FileID)
	{
		fclose(m_FileID);
		m_FileID = NULL;
	}
}

/// 获取 INI 文件
const char* IniParser::GetFile()
{
	return m_FileName;
}

/************************************************************************
* @brief 从INI文件读取字符串类型数据
* @param[in] *title 节点名称（以中括号括起来的）
* @param[in] *key 键
* @param[in] *fp 文件指针
* @return 返回键值（会忽略前面的空格）
* @mark 当未找到时返回一个"error"
************************************************************************/
const char* IniParser::_GetString(const char *title, const char *key)
{
	// 判断title长度在有限范围
	assert (strlen(title) < MAX_LEN);
	
	if (NULL == m_FileID)
	{
		fprintf(stderr, "File has not been opened.\n");
		return INI_ERROR;
	}

	// 移动到文件头
	fseek(m_FileID, 0, SEEK_SET);

	// INI文件单行不能超过MAX_LEN
	char szLine[MAX_LEN] = { 0 };
	static char tmpstr[MAX_LEN] = { 0 };
	int retval = 0;
	int i = 0;
	bool title_exist = false;
	bool new_line = false;
	char *tmp = 0;

	while(retval != EOF)
	{
		retval = fgetc(m_FileID);

		szLine[i++] = retval;

		switch (retval)
		{
		case '\n':// 换行符需要变为'\0'
			new_line = true;
#ifndef _WIN32
			i--;
#endif 
			szLine[--i] = '\0';
			i = 0;
			break;
		case EOF: // 文件结尾没有换行符
			new_line = true;
#ifndef _WIN32
			i--;
#endif 
			szLine[i] = '\0';
			i = 0;
			break;
		default:
			continue;
		}

		if(new_line)
		{
			new_line = false;
			// strchr：查找字符串s中首次出现字符c的位置
			// 说明：返回首次出现c的位置的指针，返回的地址
			// 是字符串在内存中随机分配的地址再加上你所搜索
			// 的字符在字符串位置，如果s中不存在c则返回NULL。

			tmp = strchr(szLine, '=');

			if(( tmp != NULL ) && title_exist)
			{
				// strstr搜索一个字符串在另一个字符串中的第一次出现。
				// 该函数返回字符串的其余部分（从匹配点）。

				if(strstr(szLine, key)!= NULL)
				{
					// 注释行
					if ('#' == szLine[0])
					{
					}
					else
					{
						// 找到key对应变量
						strcpy(tmpstr, tmp + 1);
						return tmpstr;
					}
				}
			}
			else
			{
				strcpy(tmpstr, "[");
				strcat(tmpstr, title);
				strcat(tmpstr, "]");
				if( strncmp(tmpstr, szLine, strlen(tmpstr))== 0 )// 找到title
					title_exist = true;
			}
		}
	}

	fprintf(stderr, "EOF but nothing found. ");

	if (!title_exist)
	{
		fprintf(stderr, "Title is not exist.\n");
	}
	else
	{
		fprintf(stderr, "Key is not exist.\n");
	}

	return INI_ERROR;
}

/************************************************************************
* @brief 从INI文件读取字符串类型数据
* @param[in] *title 节点名称（以中括号括起来的）
* @param[in] *key 键
* @param[in] *dst 键值
* @param[in] *len dst缓存长度
* @return 无
* @note 该函数内部调用private _GetString
************************************************************************/
void IniParser::GetString(const char *title, const char *key, char *dst, int len)
{
	memset(dst, 0, len);
	len = len > MAX_LEN ? MAX_LEN : len;
	const char *temp = _GetString(title, key);
	char *pHead = (char *)temp;
	// 去除前面的空格
	while (' ' == *pHead)
		pHead++;
	int n = strlen(pHead);
	memcpy(dst, pHead, n > len ? len : n);
}

/************************************************************************
* @brief 将字符串写入文件（修改指定的键值）
* @param[in] *title 节点名称（以中括号括起来的）
* @param[in] *key 键
* @param[in] *src 新的键值
* @return 无
************************************************************************/
void IniParser::SetString(const char *title, const char *key, char *src)
{
	const char *temp = _GetString(title, key);
	if(strcmp(temp, INI_ERROR) == 0)
		return;
	int len = strlen(temp);
	fseek(m_FileID, -1, SEEK_CUR);/* 向前移动一字节 */
	long pos = ftell(m_FileID);	/* 插入位置 */
	char tmp[MAX_LEN] = { 0 };
	fread(tmp, 1, MAX_LEN, m_FileID);	/* 把后面的数据缓存 */
	fseek(m_FileID, pos - len, 0);/* 移动到插入位置 */
	fprintf(m_FileID, src);		/* 插入，就是写入数据 */
	fprintf(m_FileID, tmp);		/* 把原来的数据再写回来， 完成 */
}

/************************************************************************
* @brief 从INI文件读取整数类型数据
* @param[in] *title 节点名称（以中括号括起来的）
* @param[in] *key 键
* @return 整数
* @note 该函数内部调用_GetString
************************************************************************/
int IniParser::GetInt(const char *title, const char *key)
{
	return atoi(_GetString(title, key));
}

/************************************************************************
* @brief 从INI文件读取整数类型数据
* @param[in] *title 节点名称（以中括号括起来的）
* @param[in] *key 键
* @param[in] *src 新的键值
************************************************************************/
void IniParser::SetInt(const char *title, const char *key, int src)
{
	char temp[MAX_LEN] = { 0 };
	sprintf(temp, "%d", src);
	SetString(title, key, temp);
}

/************************************************************************
* @brief 从INI文件读取浮点类型数据
* @param[in] *title 节点名称（以中括号括起来的）
* @param[in] *key 键
* @return 返回键值（会忽略前面的空格）
* @note 该函数内部调用_GetString
************************************************************************/
double IniParser::GetDouble(const char *title, const char *key)
{
	return atof(_GetString(title, key));
}

/************************************************************************
* @brief 从INI文件读取整数类型数据
* @param[in] *title 节点名称（以中括号括起来的）
* @param[in] *key 键
* @param[in] *src 新的键值
************************************************************************/
void IniParser::SetDouble(const char *title, const char *key, double src)
{
	char temp[MAX_LEN] = { 0 };
	sprintf(temp, "%.6f", src);
	SetString(title, key,temp);
}

/************************************************************************
* @brief 获取当前程序目录，返回目录的尾指针。
* @example 例如应用程序的目录为"C:\test"，则得到的buf为"C:\test"，且p指向
* 字符t的后一个字符。为了能安全的对p指针操作，这就要求buf的大小不能太小。
************************************************************************/
char* GetCurrentPath(char *buf)
{
	char *p = 0;
#ifdef _WIN32
	GetModuleFileName(NULL, buf, MAX_PATH);
#else
	char pidfile[64] = { 0 };
	sprintf(pidfile, "/proc/%d/cmdline", getpid());

	int fd = open(pidfile, O_RDONLY, 0);
	if (-1 == fd)
		return NULL;

	int bytes = read(fd, buf, MAX_PATH);
	close(fd);

	if (bytes <= 0)
		return NULL;

	buf[bytes - 1] = '\0';
#endif

	p = &buf[strlen(buf)];
	do
	{
		*p = '\0';
		p--;
#ifdef _WIN32
	}while( '\\' != *p );
#else
	}while( '/' != *p );
#endif

	p++;

	return p;
}
