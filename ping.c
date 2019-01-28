#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

#define DATA_LEN 56

char sendbuf[1024];
char recvbuf[1024];
int sendnum = 0; 
int recvnum = 0;

struct sockaddr_in from;

void handler(int s)
{
    printf("-----------------------status-----------------------\n");
    printf("%d packets transmitted, %d received, %.3f%% packet lost.\n",sendnum, recvnum, ((sendnum-recvnum) / sendnum) * 100);
    exit(0);
}
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
    ret += *(char*)addr;
  }
  ret = (ret>>16) + (ret&0xffff);
  ret = (ret>>16) + ret;
  return (unsigned short)(~ret);
}
int pack(int num,pid_t pid)
{
  struct icmp *p = (struct icmp*)sendbuf;
  p->icmp_type   = ICMP_ECHO;
  p->icmp_code   = 0;
  p->icmp_cksum  = 0;
  p->icmp_id     = pid;
  p->icmp_seq    = num;

  gettimeofday((struct timeval*)p->icmp_data,NULL);

  p->icmp_cksum = chksum((unsigned short*)sendbuf,DATA_LEN+8);
  
  return DATA_LEN+8;
}
void send_packet(int sfd,pid_t pid,struct sockaddr_in addr)
{
  //封装包
  sendnum++;  //告知第几个包
  int r = pack(sendnum,pid);
  sendto(sfd,sendbuf,r,0,(struct sockaddr*)&addr,sizeof(addr));
}

void unpack(int num,pid_t pid)
{
  struct timeval end;
  gettimeofday(&end,NULL);
  struct ip *pip = (struct ip*)recvbuf;
  struct icmp *picmp = (struct icmp*)(recvbuf+(pip->ip_hl<<2));
  printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms\n",DATA_LEN,inet_ntoa(from.sin_addr),picmp->icmp_seq,pip->ip_ttl,diftime(&end,(struct timeval*)picmp->icmp_data));
}
void recv_packet(int sfd,pid_t pid)
{
  recvnum++;
  socklen_t len = sizeof(from);

  recvfrom(sfd,recvbuf,1024,0,(struct sockaddr*)&from,&len);
  recvnum++;
  //解包
  unpack(recvnum,pid);

}
//a.out ip/域名
int main(int argc,char* argv[])
{
  if(argc !=2 )
  {
    fprintf(stderr,"Usage:a.out ip/域名\n");
    exit(1);
  }
  signal(SIGINT, handler);
  in_addr_t ad;
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  int sock = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
  if((addr.sin_addr.s_addr = inet_addr(argv[1])) == INADDR_NONE)
  {
    //通过域名获得地址（即DNS解析）
    struct hostent* p = gethostbyname(argv[1]);
    if(p == NULL)
    {
       perror("gethostofname");
      exit(2);
    }
    memcpy((void*)&addr.sin_addr,(void*)p->h_addr,p->h_length);
  }
  addr.sin_addr.s_addr = ad;
  //接受和发送数据包
  pid_t pid = getpid();
  printf("PING %s (%s) %d bytes of data.\n",argv[1],inet_ntoa(addr.sin_addr),DATA_LEN);
  while(1)
  {
    send_packet(sock,pid,addr); //套接字 pid 原始地址
    recv_packet(sock,pid);
    sleep(1);
  }
  return 0;
}
