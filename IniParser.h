/***********************************************************************
*                       ��ȡ INI �����ļ�                              *
* INI �ļ���ʽ��
* 	[Node]
*		Param1 = Value1
* 		Param2 = Value2
* 		Param3 = Value3
* 		.
* 		.
* 		.
* 		ParamN = ValueN
* �ο����ϣ�http://blog.csdn.net/chexlong/article/details/6818017
************************************************************************/

// FILE ����ͷ�ļ�
#include <stdio.h>

/// Read Ini Error
#define INI_ERROR "error"

/// ����ļ�·��MAX_PATH = 260
#define  MAX_PATH 260

/// INI���������MAX_LEN = 260
#define MAX_LEN 260

/************************************************************************
* @class IniParser INI �ļ���д��
************************************************************************/
class IniParser
{
public:
	IniParser();
	~IniParser();

	// �� INI �ļ�
	bool Open(const char *filename = 0);

	// �ر� INI �ļ�
	void Close();

	// ���´��ļ�
	bool ReOpen(const char *mode);

	// ��ȡ INI �ļ�
	_inline const char* GetFile();

	// ��INI�ļ���ȡ�ַ�����������
	void GetString(const char *title, const char *key, char *dst, int len);

	// ���ַ���д���ļ�
	void SetString(const char *title, const char *key, char *src);

	// ��INI�ļ���ȡ������������
	int GetInt(const char *title, const char *key);

	void SetInt(const char *title, const char *key, int src);

	// ��INI�ļ���ȡ������������
	double GetDouble(const char *title, const char *key);

	void SetDouble(const char *title, const char *key, double src);

private:
	/// INI �ļ�
	char m_FileName[MAX_PATH];

	/// �ļ�ID
	FILE *m_FileID;

	// ��INI�ļ���ȡ�ַ�����������
	const char* _GetString(const char *title, const char *key);
};

// ��ȡ��ǰ����Ŀ¼������Ŀ¼��βָ��
char* GetCurrentPath(char *buf);
