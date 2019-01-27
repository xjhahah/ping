#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//打印报头所需的信息，存在 /user/include/netinet/ip.h 当中
#if 0
struct iphdr
{
  u_int8_t tos;                                                                                 
  u_int16_t tot_len;                                                                      
  u_int16_t id;                                                                                  
  u_int16_t frag_off;                                                                            
  u_int8_t ttl;                                                                              
  u_int8_t protocol;   //查看                                                                          
  u_int16_t check;     //查看报头信息                                                             
  u_int32_t saddr;     //源地址                                                                 
  u_int32_t daddr;     //目标地址
};
#endif

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
  struct iphdr *pip = NULL;

  while(1)
  {
    bzero(&buf,sizeof(buf));
    read(fd,buf,sizeof(buf));
    pip = (struct iphdr*)buf;
    struct in_addr ad;
    ad.s_addr = pip->saddr,
    printf("ttl=%hhu,protocol=%hhu %s <----->",pip->ttl,pip->protocol,inet_ntoa(ad));
    ad.s_addr = pip -> daddr,
    printf("%s\n",inet_ntoa(ad));
  }
  return 0;
}
