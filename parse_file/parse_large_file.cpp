// parse_large_file.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
const int MAX_SIZE  = 256;
const int _MB = 1024 * 1024;
const int BLOCK_SIZE = _MB;
static inline __int64 get_file_size(FILE *fp)
{
	fpos_t len;
	fseek(fp, 0L, SEEK_END);
	fgetpos(fp, &len);
	fseek(fp, 0L, SEEK_SET);
	return len;
}

static inline FILE *open_file(const char *filename, const char *mode)
{
	FILE *fp = fopen(filename, mode);
	char errcode[MAX_SIZE] = { 0 };
	if (fp == NULL)
	{
		sprintf(errcode, "错误打开%s失败!", filename);
		printf("%s\n", errcode);
		abort();
		return NULL;
	}
	return fp;
}

static inline void *_malloc(size_t n)
{
	char errcode[MAX_SIZE] = { 0 };
	void *buf = malloc(sizeof(char*)*n);
	if(buf == NULL)
	{
		sprintf(errcode, "分配%字节空间失败!", n);
		printf("%s\n", errcode);
		abort();
	}
	memset(buf, 0, n);
	return buf;
}

static inline FILE *create_file(__int64 num, const char *mode)
{
	char new_file[MAX_SIZE] = { 0 };
	itoa(num, new_file, 10);
	strcat(new_file,   ".txt");
	FILE *pbytes = open_file(new_file, mode);
	return pbytes;
}

static inline __int64 countline(const char *filename)
{
	char buf[MAX_SIZE*16];
	FILE *fp = fopen(filename, "rb");
	size_t size_read = 0;
	 __int64 count = 0;
	while(size_read = fread(buf, sizeof(char),  MAX_SIZE*16 , fp))
	{
		for(size_t i = 0; i < size_read; i++)
		{
			if(buf[i] == '\n')
				count++;
		}
	}
	fclose(fp);
	return count;
}

static inline int parse_bytes(const char *buf, __int64 len, __int64 line)
{
	const char *pos = buf, *p = buf;
	__int64 bytes_size = len;
	static __int64 count = 0;
	size_t file_size = 0;
	static __int64 last = count;
	static __int64 file_count = 1;
	static char last_file[MAX_SIZE] = {0};
	FILE *pbytes = NULL;
	while (bytes_size--)
	{
		if ((*p == '\r' && *(p+1) =='\n')|| (*p =='\n'))
			count++;
		if (count >0 && count % line == 0 && last != count )
		{
			if(strlen(last_file)==0)
			{
				pbytes = create_file(file_count, "wb+");
				file_count++;
			}
			else
				pbytes = open_file(last_file, "ab+");
			memset(last_file, 0, MAX_SIZE);	
			file_size = p - pos + 1;
			fwrite(pos , sizeof(char), file_size, pbytes);
			pos = p + 1;
			fclose(pbytes);
			last = count;
		}
		p++;
	}
	if(pos <= buf + len)
	{
		FILE *pbytes = NULL;
		if(strlen(last_file)==0)
		{
			pbytes = create_file(file_count, "wb+");
			itoa(file_count, last_file, 10);
			strcat(last_file,   ".txt");
			file_count++;
		}
		else
			pbytes = open_file(last_file, "ab+");
		file_size = p - pos;
		fwrite(pos , sizeof(char), file_size, pbytes);
		fclose(pbytes);
	}
	return count;
}

int parse_file(const char *filename, __int64 line, int size_count)
{
	FILE *fp = open_file(filename, "rb+");
	static __int64 count = 0;
	__int64 buf_size = sizeof(char) * BLOCK_SIZE * size_count;
	char *buf = (char*)_malloc(buf_size);
	__int64 file_size = 0;
	__int64 pos = 0;
	memset(buf, 0, buf_size);
	while(file_size = fread(buf, sizeof(char), BLOCK_SIZE * size_count, fp))
		count = parse_bytes(buf, file_size, line);

	free(buf);
	fclose(fp);
	return 0;
}

int main(int argc, char **argv)
{
	float start = clock();
	__int64 line = 4;
	int size_count = 5;
	char *filename = "C:\\IE11_main.log";
	__int64 file_count = 3;
	if(argc>3)
	{
		filename = argv[1];
		line = atoi(argv[2]);
		size_count = atoi(argv[3]);
		size_count += 1;
		size_count /= 2;
	}

	if(argc>4)
	{
		filename = argv[1];
		size_count = atoi(argv[3]);
		size_count += 1;
		size_count /= 2;

		file_count = atoi(argv[4]);
		line = countline(filename) / file_count ;
	}
	__int64 all_lines = countline(filename);
	line =  all_lines % file_count == 0 ? all_lines/(file_count):all_lines / (file_count-1) ;
	printf("每个文件行数行数:%u 行\n",line);
	parse_file(filename, line, size_count);
	float end = clock();
	printf("共用时:%f ms\n", (end - start));

	getchar();
	return 0;
}