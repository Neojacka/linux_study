#include"pocess.h"
void pocess()
{
    int cnt = 0;
    char bar[101];
    char lbar[4] = {'|','\\','-','/'};
    memset(bar, '\0', sizeof(bar));
    while (cnt<=100)
    {  
	    printf("[%-100s][%d%%][%c]\r",bar, cnt,lbar[cnt%4]);
	    fflush(stdout);
	    bar[cnt++] = '#';
	    usleep(50000);
    }
    printf("\n");
}
