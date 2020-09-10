#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#define NAME    20
#define BUF_SIZE 4096

void* start_recv(void* arg)
{
  int sockfd = *(int*)arg;
  char buf[BUF_SIZE] = {};
  size_t buf_size = sizeof(buf);
  for(;;)
  {
    size_t rec_size = recv(sockfd,buf,buf_size,0);
    if(0==rec_size)
    {
      printf("服务器已断开!!\n");
      return NULL;
    }
    printf("%s\n",buf);
  }
}

int main()
{
  //创建套接字
  int sockfd = socket(AF_INET,SOCK_STREAM,0);
  if(0 > sockfd)
  {
    perror("socket");
    return EXIT_FAILURE;
  }
  //准备地址
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8890);
  addr.sin_addr.s_addr = inet_addr("47.102.221.173");
  socklen_t len = sizeof(addr);

  //连接
  if(connect(sockfd,(struct sockaddr*)&addr,len))
  {
    perror("connect");
    return EXIT_FAILURE;
  }

  puts("***欢迎来到聊天室***");
  printf("请输入你的聊天名：");
  char name[NAME] = {};
  gets(name);
  //发送姓名
  send(sockfd,name,strlen(name)+1,0);
  pthread_t pid;
  pthread_create(&pid,NULL,start_recv,&sockfd);
  
  char buf[BUF_SIZE] = {};
  char buf2[BUF_SIZE] = {};
  for(;;)
  {
    printf(">>>");
    gets(buf);
    sprintf(buf2,"%s：%s",name,buf);
    size_t rec_size = send(sockfd,buf2,strlen(buf2)+1,0);
    if(0==rec_size||0 == strcmp("quit",buf))
    {
      printf("***退出聊天室***\n");
      close(sockfd);
      return 0;
    }
  }
}
