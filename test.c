#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main()
{
  int fd = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);//ping就可以产生ssh协议
  if(fd < 0)
  {
    perror("socket");
    exit(1);
  }
  char buf[2048];
  int op = 1;
  setsockopt(fd,IPPROTO_IP,IP_HDRINCL,&op,sizeof(op));
  while(1)
  {
    bzero(&buf,sizeof(buf));
    read(fd,buf,sizeof(buf));
  }
  return 0;
}
