#include "TcpServer.h"
#include "EchoServer.h"
#include <mysql/mysql.h>

int main(int argc , char * argv[])
{
    if(argc != 3)
    {
        printf("usgae :./tecepoll ip port\n");
        return -1;
    }
    MYSQL *mysql = mysql_init(NULL);
    if(mysql_real_connect(mysql,"0.0.0.0","root","234432rT&","test",3306,NULL,0)!=NULL)
    {
        printf("connect ok\n");
    }
    EchoServer echoServer(argv[1],atoi(argv[2]),3,3);
    echoServer.Start();
    return 0;
}
