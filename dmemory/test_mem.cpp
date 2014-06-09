// test_mem.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "dmemory.h"
#include <string.h>
#include <stdlib.h>

int _tmain(int argc, _TCHAR* argv[])
{
	dmemory_init();
	unsigned char *p1 = (unsigned char *)dmalloc(100);
	unsigned char *p = (unsigned char *)dmalloc(100);
	memset(p, 0, 101);
	memset(p1, 0, 100);
	dfree(p);
	//dfree(p1);
	printf("is leak:%d\n", dcheck_leak());
	return 0;
}

