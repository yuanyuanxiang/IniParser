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
* @brief ���� INI �ļ�
* @param[in] *filename INI�ļ�
* @return �Ƿ����óɹ����ļ�������ʱ����ʧ�ܣ�
* @remark ������Ĭ�ϲ���ʱ�����ӳ�������Ŀ¼��ȡconfig.ini
* @warning ����r+��ʽ�򿪣������Ҫ��ĩβ׷�ӣ���ο�ReOpen
************************************************************************/
bool IniParser::Open(const char *filename)
{
	if (filename)
	{
		// �ж��ļ��Ƿ����
		if (-1 == _access(filename, 0))
			return false;
		int nLength = strlen(filename);
		memset(m_FileName, 0, MAX_PATH);
		memcpy(m_FileName, filename, nLength > MAX_PATH ? MAX_PATH : nLength);
		m_FileID = fopen(m_FileName, "r+");
		return (NULL != m_FileID);
	}
	// Ĭ�ϴӳ�������Ŀ¼��ȡ"config.ini"
	char buf[MAX_PATH] = { 0 };
	char *end = GetCurrentPath(buf);
	memcpy(end, "config.ini", strlen("config.ini"));
	bool temp = Open(buf);
	return temp;
}

// ���´��ļ�
bool IniParser::ReOpen(const char *mode)
{
	if (m_FileID)
	{
		fclose(m_FileID);
		m_FileID = fopen(m_FileName, mode);
	}
	return (NULL != m_FileID);
}

// �ر��ļ�
void IniParser::Close()
{
	if (m_FileID)
	{
		fclose(m_FileID);
		m_FileID = NULL;
	}
}

/// ��ȡ INI �ļ�
const char* IniParser::GetFile()
{
	return m_FileName;
}

/************************************************************************
* @brief ��INI�ļ���ȡ�ַ�����������
* @param[in] *title �ڵ����ƣ����������������ģ�
* @param[in] *key ��
* @param[in] *fp �ļ�ָ��
* @return ���ؼ�ֵ�������ǰ��Ŀո�
* @mark ��δ�ҵ�ʱ����һ��"error"
************************************************************************/
const char* IniParser::_GetString(const char *title, const char *key)
{
	// �ж�title���������޷�Χ
	assert (strlen(title) < MAX_LEN);
	
	if (NULL == m_FileID)
	{
		fprintf(stderr, "File has not been opened.\n");
		return INI_ERROR;
	}

	// �ƶ����ļ�ͷ
	fseek(m_FileID, 0, SEEK_SET);

	// INI�ļ����в��ܳ���MAX_LEN
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
		case '\n':// ���з���Ҫ��Ϊ'\0'
			new_line = true;
#ifndef _WIN32
			i--;
#endif 
			szLine[--i] = '\0';
			i = 0;
			break;
		case EOF: // �ļ���βû�л��з�
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
			// strchr�������ַ���s���״γ����ַ�c��λ��
			// ˵���������״γ���c��λ�õ�ָ�룬���صĵ�ַ
			// ���ַ������ڴ����������ĵ�ַ�ټ�����������
			// ���ַ����ַ���λ�ã����s�в�����c�򷵻�NULL��

			tmp = strchr(szLine, '=');

			if(( tmp != NULL ) && title_exist)
			{
				// strstr����һ���ַ�������һ���ַ����еĵ�һ�γ��֡�
				// �ú��������ַ��������ಿ�֣���ƥ��㣩��

				if(strstr(szLine, key)!= NULL)
				{
					// ע����
					if ('#' == szLine[0])
					{
					}
					else
					{
						// �ҵ�key��Ӧ����
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
				if( strncmp(tmpstr, szLine, strlen(tmpstr))== 0 )// �ҵ�title
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
* @brief ��INI�ļ���ȡ�ַ�����������
* @param[in] *title �ڵ����ƣ����������������ģ�
* @param[in] *key ��
* @param[in] *dst ��ֵ
* @param[in] *len dst���泤��
* @return ��
* @note �ú����ڲ�����private _GetString
************************************************************************/
void IniParser::GetString(const char *title, const char *key, char *dst, int len)
{
	memset(dst, 0, len);
	len = len > MAX_LEN ? MAX_LEN : len;
	const char *temp = _GetString(title, key);
	char *pHead = (char *)temp;
	// ȥ��ǰ��Ŀո�
	while (' ' == *pHead)
		pHead++;
	int n = strlen(pHead);
	memcpy(dst, pHead, n > len ? len : n);
}

/************************************************************************
* @brief ���ַ���д���ļ����޸�ָ���ļ�ֵ��
* @param[in] *title �ڵ����ƣ����������������ģ�
* @param[in] *key ��
* @param[in] *src �µļ�ֵ
* @return ��
************************************************************************/
void IniParser::SetString(const char *title, const char *key, char *src)
{
	const char *temp = _GetString(title, key);
	if(strcmp(temp, INI_ERROR) == 0)
		return;
	int len = strlen(temp);
	fseek(m_FileID, -1, SEEK_CUR);/* ��ǰ�ƶ�һ�ֽ� */
	long pos = ftell(m_FileID);	/* ����λ�� */
	char tmp[MAX_LEN] = { 0 };
	fread(tmp, 1, MAX_LEN, m_FileID);	/* �Ѻ�������ݻ��� */
	fseek(m_FileID, pos - len, 0);/* �ƶ�������λ�� */
	fprintf(m_FileID, src);		/* ���룬����д������ */
	fprintf(m_FileID, tmp);		/* ��ԭ����������д������ ��� */
}

/************************************************************************
* @brief ��INI�ļ���ȡ������������
* @param[in] *title �ڵ����ƣ����������������ģ�
* @param[in] *key ��
* @return ����
* @note �ú����ڲ�����_GetString
************************************************************************/
int IniParser::GetInt(const char *title, const char *key)
{
	return atoi(_GetString(title, key));
}

/************************************************************************
* @brief ��INI�ļ���ȡ������������
* @param[in] *title �ڵ����ƣ����������������ģ�
* @param[in] *key ��
* @param[in] *src �µļ�ֵ
************************************************************************/
void IniParser::SetInt(const char *title, const char *key, int src)
{
	char temp[MAX_LEN] = { 0 };
	sprintf(temp, "%d", src);
	SetString(title, key, temp);
}

/************************************************************************
* @brief ��INI�ļ���ȡ������������
* @param[in] *title �ڵ����ƣ����������������ģ�
* @param[in] *key ��
* @return ���ؼ�ֵ�������ǰ��Ŀո�
* @note �ú����ڲ�����_GetString
************************************************************************/
double IniParser::GetDouble(const char *title, const char *key)
{
	return atof(_GetString(title, key));
}

/************************************************************************
* @brief ��INI�ļ���ȡ������������
* @param[in] *title �ڵ����ƣ����������������ģ�
* @param[in] *key ��
* @param[in] *src �µļ�ֵ
************************************************************************/
void IniParser::SetDouble(const char *title, const char *key, double src)
{
	char temp[MAX_LEN] = { 0 };
	sprintf(temp, "%.6f", src);
	SetString(title, key,temp);
}

/************************************************************************
* @brief ��ȡ��ǰ����Ŀ¼������Ŀ¼��βָ�롣
* @example ����Ӧ�ó����Ŀ¼Ϊ"C:\test"����õ���bufΪ"C:\test"����pָ��
* �ַ�t�ĺ�һ���ַ���Ϊ���ܰ�ȫ�Ķ�pָ����������Ҫ��buf�Ĵ�С����̫С��
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
