#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h> //用于变长数组
#include <arpa/inet.h> //用于inet_pton(),htons()

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/errno.h> //用于strerror()函数

#define SERVER_PORT 18000
#define MAX_LINE 100
void error_handler(const char *fmt,...);
int main(int argc,char **argv){
    
    int  sockfd; //fd = file descriptor, socket本质上也是文件，listening socket descriptor info
    struct sockaddr_in serv_addr;
    char sendline[MAX_LINE];
    char recvline[MAX_LINE];
    int  sendbytes;
    int  n;
    
    //检查参数 编译后使用: ./client 8.8.8.8
    if(argc != 2){
        error_handler("usage: %s <server address>",argv[0]);
    }


    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        error_handler("socket creation error");
    }
    //socket 属于system call

    memset(&serv_addr, 0, sizeof(serv_addr)); //每个字节都用0填充,初始化
    serv_addr.sin_len = sizeof(serv_addr); //将长度保存在结构体内，这样之后就不用再调用sizeof()进行计算了
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    if (inet_pton(AF_INET,argv[1],&serv_addr.sin_addr) <= 0){
        error_handler("inet_pton error for %s", argv[1]);
    } 
    // serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    //具体的IP地址，同时进行错误处理，例如输入了256.0.0.0这种错误地址(0)，或发生了系统错误(-1)将会退出
    
    serv_addr.sin_port = htons(SERVER_PORT);  //设置端口
    

    //进行连接
    if(connect(sockfd, (struct sockaddr*)&serv_addr, serv_addr.sin_len) == -1){
        error_handler("connection failed");
    };
    
    //准备要传输给服务端的数据
    memset(sendline,0,MAX_LINE);
    // sprintf(sendline,"GET / HTTP/1.1\r\nHost: www.httpbin.org\r\n\r\nyou cant see this shit\r\nyesyou\r\n");
    sprintf(sendline,"");
    sendbytes = strlen(sendline);

    //使用write syscall将sendline写进sockfd中，注意写有可能失败，要做处理
    if (write(sockfd, sendline, sendbytes) != sendbytes){
        error_handler("write_error");
    }
    sleep(2);
    
    sprintf(sendline,"this is seconds line");
    sendbytes = strlen(sendline);
    close(sockfd);
    exit(0);
    if (write(sockfd, sendline, sendbytes) != sendbytes){
        error_handler("write_error");
    }

    //在sockfd写入数据时数据被传到服务器，服务器随即应答，我们使用read syscall读取数据
    memset(recvline,0,sizeof(recvline));
    while( (n = read(sockfd, recvline, MAX_LINE - 1 )) > 0){
        
        printf("%s",recvline);
        memset(recvline,0,sizeof(recvline));
    }
    if(n < 0){
        error_handler("read error");
    }
    close(sockfd);
    return 0;
}


void error_handler(const char *fmt, ...){
    int errno_save;
    va_list     ap;

    errno_save = errno; //errno为可能发生syscall错误记录，后续操作可能会改变，需要现在保存
    //开始处理变长数组,打印信息
    va_start(ap, fmt);
    vfprintf(stdout,fmt,ap);
    fprintf(stdout,"\n");
    fflush(stdout);

    if (errno_save != 0){
        fprintf(stdout,"(errno = %d) : %s\n",errno_save,strerror(errno_save));
        fprintf(stdout,"\n");
        fflush(stdout);
    }

    va_end(ap); 
    exit(1);
}