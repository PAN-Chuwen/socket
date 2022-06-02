#include "common.h"
#include <string.h>

  
int main(){
    int                     listenfd,connfd; // file descriptors
    int                     n;
    struct sockaddr_in      serv_addr;
    char                    sendline[MAX_LINE];
    char                    recvline[MAX_LINE];
    
    
    //创建套接字
    if( (listenfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
        error_handler("socket creation error");
    }
    //将套接字和IP、端口绑定
    
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //具体的IP地址
    serv_addr.sin_port = htons(SERVER_PORT);  //端口

    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
        error_handler("bind error!");
    }

    //进入监听状态，等待用户发起请求
    if (listen(listenfd, 10) == -1){
        error_handler("listen error!");
    }

    //接收客户端请求, 这里的while循环是为了反复接收客户端请求(但不能同时接收，和多线程有区别)
    while(1){
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        fprintf(stdout,"waiting for a connection on port %d.\n",SERVER_PORT);
        fflush(stdout);
        //connfd = accept(listenfd, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
        getpeername(connfd,(struct sockaddr*)&client_addr,&client_addr_size);
        fprintf(stdout,"new connection request accepted,IPv4 address of client: %s\n",inet_ntoa(client_addr.sin_addr));
        //清空用于接收数据的buffer
        
        //现在是循环，后续改成每次连接创建thread
        handle_connection(connfd);

        fprintf(stdout,"end of data from client\n");
        
            

        
        //准备服务器要发送的数据
        snprintf((char*)sendline,sizeof(sendline),"HTTP/1.0 200 OK\r\n\r\nHello");
        
        //发送数据
        write(connfd, (char*)sendline, strlen((char*)sendline));
        //关闭socket
        close(connfd);
   
    //关闭套接字
    }
   
    close(listenfd);

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

void handle_connection(int client_socket){
    /* 步骤
    读 HTTP header，根据header选择功能 1. GET 2. POST
    */
   char buffer[BUF_SIZE];
   
   u_int32_t msg_size = 0;
   u_int64_t bytes_read;

//read request+data
   while((bytes_read = read(client_socket,buffer+msg_size,BUF_SIZE-msg_size)) > 0){
       msg_size += bytes_read;
       if(msg_size >= BUF_SIZE){
           error_handler("file too large");
       }
   }
    if(bytes_read == -1) error_handler("read error");
    fprintf(stdout,"Request: %s", buffer);
    fflush(stdout);

}