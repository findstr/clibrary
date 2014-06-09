// sz2int.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "assist.h"
#include "zstring.h"



int main(int argc, char * argv[])
{
        int a;
        int i;

        CACL_TAKES_TIME_BEGIN(aaa);
        for (i = 0; i < 1000000; i++)
                a = zstring_sztoul("123456", 6, 16);
                //a = strtoul("0x123456", NULL, 10);

        printf("value: %p ,takes: %d ms", a, CACL_TAKES_TIME_END(aaa));
	return 0;
}

