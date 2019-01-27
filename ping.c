#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

//计算两个时差，在 man 2 gettimeofday 可以查到
float diftime(struct timeval* end, struct timeval* sta)
{
  return (float)((end->tv_sec - sta->tv_sec)*1000 + (end->tv_usec - sta->tv_usec)/1000);
}

//校验和算法
unsigned short chksum(unsigned short* addr,int len)
{
  unsigned int ret =0;
  while(len > 1)
  {
    ret += *addr++;
    len -= 2;
  }
  if(len == 1) //奇数位
  {
    ret += (unsigned char*)addr;
  }
  ret = (ret>>16) + (ret&0xffff);
  ret += (ret>>16);
  return (unsigned short)(~ret);
}
int main()
{
  //通过域名获得地址（即DNS解析）
  struct hostent* p = gethostbyname("www.baidu.com");
  printf("%s\n",inet_ntoa(*(struct in_addr*)p->h_addr));
  return 0;
}
