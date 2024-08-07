#include<stdio.h>
#include"my_add.h"
#include"my_sub.h"
int main()
{
    int a=10,b=5;
    int c = add(a,b);
    int d = sub(a,b);
    printf("c=%d,d=%d\n",c,d);
    return 0;
}
