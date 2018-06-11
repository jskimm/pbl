#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define  NAMESIZE 1024
#define  BUFSIZE 1024

void* send_data(int sock);
void* recv_data(int sock);
void error_handling(char * message);

char name[NAMESIZE] = "[Default]";
char buf[BUFSIZE];

int main(int argc, char **argv)
{
   int sock;
   struct sockaddr_in serv_addr;
   pthread_t snd_thread, rcv_thread;
   void* thread_result;

   if(argc != 3){
      printf("사용법 : %s <아이피> <포트>\n", argv[0]);
      exit(1);
   }

   sock = socket(PF_INET, SOCK_STREAM, 0);
   if(sock == -1)
      error_handling("socket() 에러");

   memset(&serv_addr, 0, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
   serv_addr.sin_port=htons(atoi(argv[2]));

   if(connect(sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
      error_handling("connect() 에러");

   pthread_create(&snd_thread, NULL, send_data, sock);
   pthread_create(&rcv_thread, NULL, recv_data, sock);

   pthread_join(snd_thread, &thread_result);
   pthread_join(rcv_thread, &thread_result);
   
   close(sock);
   return 0;
}

void* send_data(int sock)
{
   char data[BUFSIZE];
   write(sock, "\n", 1);
   while(1){
     fgets(buf, BUFSIZE, stdin);
    //  if(!strcmp(buf, "0\n")) { // 0 입력 시 클라이언트 종료
    //     close(sock);
    //   exit(0);
    //  }
     sprintf(data, "%s", buf);
     write(sock, data, strlen(data));
   }
}

void* recv_data(int sock)
{
   char data[BUFSIZE];
   int str_len;
   while(1)
   {
      str_len = read(sock, data, BUFSIZE-1);
      if(str_len == -1) return (void*)1;
      data[str_len]= '\0'; 
      fputs(data, stdout);
   }
}



void error_handling(char *data)
{
   fputs(data ,stderr);
   fputc('\n', stderr);
}