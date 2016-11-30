/***********************************************************************
*                       读取 INI 配置文件                              *
* INI 文件格式：
* 	[Node]
*		Param1 = Value1
* 		Param2 = Value2
* 		Param3 = Value3
* 		.
* 		.
* 		.
* 		ParamN = ValueN
* 参考资料：http://blog.csdn.net/chexlong/article/details/6818017
************************************************************************/

// FILE 所在头文件
#include <stdio.h>

/// Read Ini Error
#define INI_ERROR "error"

/// 最大文件路径MAX_PATH = 260
#define  MAX_PATH 260

/// INI单行最长长度MAX_LEN = 260
#define MAX_LEN 260

/************************************************************************
* @class IniParser INI 文件读写器
************************************************************************/
class IniParser
{
public:
	IniParser();
	~IniParser();

	// 打开 INI 文件
	bool Open(const char *filename = 0);

	// 关闭 INI 文件
	void Close();

	// 重新打开文件
	bool ReOpen(const char *mode);

	// 获取 INI 文件
	_inline const char* GetFile();

	// 从INI文件读取字符串类型数据
	void GetString(const char *title, const char *key, char *dst, int len);

	// 将字符串写入文件
	void SetString(const char *title, const char *key, char *src);

	// 从INI文件读取整数类型数据
	int GetInt(const char *title, const char *key);

	void SetInt(const char *title, const char *key, int src);

	// 从INI文件读取浮点类型数据
	double GetDouble(const char *title, const char *key);

	void SetDouble(const char *title, const char *key, double src);

private:
	/// INI 文件
	char m_FileName[MAX_PATH];

	/// 文件ID
	FILE *m_FileID;

	// 从INI文件读取字符串类型数据
	const char* _GetString(const char *title, const char *key);
};

// 获取当前程序目录，返回目录的尾指针
char* GetCurrentPath(char *buf);
