#include "IniParser.h"
#include <stdio.h>
#include <stdlib.h>
#include "time.h"

int main()
{
	char p[MAX_LEN];
	IniParser ini;
	if(ini.Open())
	{
		// Read
		ini.GetString("Node", "a", p, MAX_LEN);
		printf("a = %s\n", p);
		ini.GetString("Node", "b", p, MAX_LEN);
		printf("b = %s\n", p);
		ini.GetString("Node", "c", p, MAX_LEN);
		printf("c = %s\n", p);
		ini.GetString("Node", "d", p, MAX_LEN);
		printf("d = %s\n", p);
		ini.GetString("Node", "e", p, MAX_LEN);
		printf("e = %s\n", p);
		system("PAUSE");

		//////////////////////////////////////////////////////////////////////////
		// Write
		time_t t; time(&t);
		srand((unsigned int)t);

		int temp = rand();
		ini.SetInt("Node", "a", temp);
		printf("a = %d\n", temp);
		temp = rand();
		ini.SetInt("Node", "b", temp);
		printf("b = %d\n", temp);
		temp = rand();
		ini.SetInt("Node", "c", temp);
		printf("c = %d\n", temp);
		temp = rand();
		ini.SetInt("Node", "d", temp);
		printf("d = %d\n", temp);
		temp = rand();
		ini.SetInt("Node", "e", temp);
		printf("e = %d\n", temp);
		ini.Close();
		system("PAUSE");
	}
	return 0;
}