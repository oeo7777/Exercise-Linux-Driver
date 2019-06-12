#include <stdio.h>

int main()
{
	char buf[512];
	FILE *fp = fopen("/dev/dummy0", "w+");
	if(fp == NULL) {
		printf("\n can't open device!\n");
		return 0;
	}

	fread(buf, sizeof(buf), 1, fp);
	fwrite(buf, sizeof(buf), 1, fp);
	fclose(fp);
	return 0;
}

