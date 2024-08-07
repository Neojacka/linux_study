#include"myFile.h"
int main()
{
    _File* fp=_fopen("test.txt","w");
    char* buff = "hello myFile\n";
    _fwrite(fp,buff,strlen(buff));
    char sbuff[1024];
    _fread(fp,sbuff,sizeof(buff)-1);
    printf("%s",buff);
    _fclose(fp);
    return 0;
}