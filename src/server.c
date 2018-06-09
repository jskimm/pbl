#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <mysql.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#define BUFSIZE 1024

#define STUDENT 0
#define PROFESSOR 1

// DB 환경설정
#define DB_HOST "127.0.0.1"     // DB 주소
#define DB_USER "root"          // DB 계정명
#define DB_PASS "root"          // DB 패스워드 
#define DB_NAME "pbl"           // 사용하는 DB


//debug
void debug(char *buf);

void* clnt_connection(int sock);
void send_data(int sock, char* message);
int recv_data(int sock, char *buf);
void error_handling(char * message);
int searchAll(MYSQL *);
int login(MYSQL *, char *id, char *pw, char *num);
void printAttendance(int sock, MYSQL *conn, char *num);
void printScore(int sock, MYSQL *conn, char *num);
char *info(MYSQL *conn, char *username);

int clnt_number=0;
int clnt_socks[10]; // Client socket을 담는 배열
char data[BUFSIZE];
MYSQL *dbconn;

pthread_mutex_t mutx;

int main(int argc, char **argv)
{
    int serv_sock;
    int clnt_sock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    int clnt_addr_size;
    pthread_t thread;
    MYSQL conn;
    MYSQL_RES *sql_result;
    MYSQL_ROW row;
    int field;
    int query_stat;

    signal(SIGPIPE, SIG_IGN);

    // 데이터베이스 연결
    dbconn = NULL;
    mysql_init(&conn);
    dbconn = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char *)NULL, 0);
    if(dbconn == NULL){
            printf("DB 연결 에러\n");
            exit(1);
    }

    // DB TEST
    searchAll(dbconn);

    if(argc != 2) {
        printf("사용법 : %s <포트>\n", argv[0]);
        exit(1);
    }

    if(pthread_mutex_init(&mutx, NULL))
        error_handling("MUTEX 초기화 에러");

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
        error_handling("socket() 에러");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() 에러");

    if(listen(serv_sock, 5) == -1)
        error_handling("listen() 에러");

    while(1)
    {
        clnt_addr_size = sizeof(clnt_addr);   
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

        pthread_mutex_lock(&mutx);

        clnt_socks[clnt_number++]=clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&thread, NULL, (void *)clnt_connection, clnt_sock);
        printf(" 연결된 IP : %s \n", inet_ntoa(clnt_addr.sin_addr));
    }
    return 0;

}

// 이 부분이 주 루틴
void *clnt_connection(int sock)
{
    int result;
    int str_len=0;
    char id[0x20];
    char pw[0x20];
    char num[0x10]="";
    char command[0x20];
    char textbuf[0x40];
    /* 
        state 1 : STUDENT
        state 2 : PROFESSOR
        state 9 : GUEST
    */
    unsigned int state = 0;
    int i;
    char *student_menu[] = { // 메뉴
        "[0] 클라이언트 종료\n",
        "[1] 출결 정보 확인\n",
        "[2] 성적 조회\n",
        "[3] 메시지\n"
    };

    char *prof_menu[] = {
        "[0] 클라이언트 종료\n",
        "[1] 출결 관리\n",
        "[?] 성적 입력\n",
        "[3] 메시지\n"
    };

    recv_data(sock, id); // SOCK_STREAM 버퍼 비움

    /* 로그인 기능 */
    while(1){
        result = 0;
        send_data(sock, "[ ID 입력 ]\n");
        recv_data(sock, id);
        send_data(sock, "[ PW 입력 ]\n");
        recv_data(sock, pw);
        
        result = login(dbconn, id, pw, num);
        if( result == 0 ){ // 학생 로그인
            state = STUDENT;
            break;
        } 
        else if( result == 1 ){ // 교수 로그인
            state = PROFESSOR;
            break;
        }
        else send_data(sock, "ID 또는 PW가 잘못되었습니다.\n"); 
    }

    /* 메뉴 기능 */
    while(state != 9){
        send_data(sock, "===================================\n");
        sprintf(textbuf, "%s %s님 반갑습니다.\n", num, id);
        send_data(sock, textbuf);

        switch(state){
            case STUDENT: // 학생 화면
            for (i = 0; i < sizeof(student_menu) / sizeof(student_menu[0]); i++)
                send_data(sock, student_menu[i]);  // 학생 메뉴 출력
            recv_data(sock, command); 
            if ( !strcmp(command,"1") ) { // 1번 선택 시
                printAttendance(sock, dbconn, num);
            } else if ( !strcmp(command,"2") ){
                printScore(sock, dbconn, num);
            } else if ( !strcmp(command,"3") ){
                send_data(sock, "학생 3번 메뉴 선택\n");
            }
            else continue;
            break;

            case PROFESSOR: // 교수 화면
            for (i = 0; i < sizeof(prof_menu) / sizeof(prof_menu[0]); i++)
                send_data(sock, prof_menu[i]); 
            recv_data(sock, command);
            if ( !strcmp(command,"1") ) { // 1번 선택 시
                send_data(sock, "교수 1번 메뉴 선택\n");
                // send_data(sock, info(dbconn, curUser));
            } else if ( !strcmp(command,"2") ){
                send_data(sock, "교수 2번 메뉴 선택\n");
            } else if ( !strcmp(command,"3") ){
                send_data(sock, "교수 3번 메뉴 선택\n");
            }
            else continue; 
            break;
            
            default: 
            send_data(sock, "비정상적인 행위 발견. 프로그램 종료\n");
            state = 9;
        }
        
    }

    /* 종료 루틴 */
    pthread_mutex_lock(&mutx);
    for(i=0;i<clnt_number;i++){ 
        if(sock == clnt_socks[i])
        {
            for(;i<clnt_number-1;i++)
            clnt_socks[i] = clnt_socks[i+1];
            break;
        }
    }
    clnt_number--;

    pthread_mutex_unlock(&mutx);
    /* 소켓 닫음 */
    close(sock);
    return 0;
}

// 데이터 보내기
void send_data(int sock, char *data)
{
    int len = strlen(data);

    pthread_mutex_lock(&mutx);
    write(sock, data, len);
    pthread_mutex_unlock(&mutx);
}


// 데이터 받기
int recv_data(int sock, char *buf) 
{
   int str_len;
   while(1)
   {
      str_len = read(sock, buf, BUFSIZE-1);
      if(str_len == -1) return (void*)1;
      buf[str_len-1]= '\0';
      return str_len;
   }
}


void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}


int searchAll(MYSQL *conn)
{
    int field;
    MYSQL_RES *sql_result;
    MYSQL_ROW row;

    mysql_query(conn, "select * from account");    // send query
    sql_result=mysql_store_result(conn);     // result saved
    field=mysql_num_fields(sql_result);    // result parsing
    printf("%12s%12s%12s\n", "id", "pw", "isprof");
    while((row=mysql_fetch_row(sql_result))){    // result print
        for(int i = 0; i < field; i++)
            printf("%12s", row[i]);
        printf("\n");
    }
}


// char *info(MYSQL *conn, char *username)
// {
//     int field;
//     // return buffer
//     static char buf[0x200];
//     MYSQL_RES *sql_result;
//     MYSQL_ROW row;

//     sprintf(buf,"select * from account where id = \"%s\";", username);
    
//     mysql_query(conn, "select * from user");
//     sql_result=mysql_store_result(conn);
//     field=mysql_num_fields(sql_result);
//     row=mysql_fetch_row(sql_result);
//     sprintf(buf,"%12s%12s%12s\n", row[0], row[1], row[2]);
//     return buf;
// }

int login(MYSQL *conn, char *id, char *pw, char *num)
{
    int field;
    char buf[0x200];
    MYSQL_RES *sql_result;
    MYSQL_ROW row;

    sprintf(buf,"select isprof, num from account where id = \"%s\" and pw = \"%s\";", id, pw);
    mysql_query(conn, buf);
    sql_result = mysql_store_result(conn);
    row = mysql_fetch_row(sql_result);
    
    if ( mysql_num_rows(sql_result) ){ // 쿼리 결과가 있을 경우
        
        sprintf(buf, "%s", row[0]);
        sprintf(num, "%s", row[1]);           // number 변수에 학번/교번 복사
        if (!strncmp(buf, "0", 1))
            return 0;     // 학생은 0 반환
        else if(!strncmp(buf, "1", 1))
            return 1; // 교수는 1 반환
    }
    return 9;

}

void printAttendance(int sock, MYSQL *conn, char *num)
{
    char buf[0x200];
    MYSQL_RES *sql_result;
    MYSQL_ROW row;

    sprintf(buf,"select name, type, date, reason from attendance join subject where student_num=\"%s\";", num);
    mysql_query(conn, buf);
    sql_result = mysql_store_result(conn);

    sprintf(buf, "| %-19s| %-19s| %-19s| %-19s\n", "name", "type", "date", "reason");
    send_data(sock, buf);
    // result print
    while((row=mysql_fetch_row(sql_result))){
        for(int i = 0; i < mysql_num_fields(sql_result); i++){
            sprintf(buf, "| %-19s", row[i]);
            send_data(sock, buf);
        }
        send_data(sock, "\n");
    }
}

void printScore(int sock, MYSQL *conn, char *num){
    char buf[0x200];
    MYSQL_RES *sql_result;
    MYSQL_ROW row;

    sprintf(buf,"select name, midterm, final, homework, attendance, grade from score join subject where student_num=\"%s\";", num);
    mysql_query(conn, buf);
    sql_result = mysql_store_result(conn);

    sprintf(buf, "| %-12s| %-12s| %-12s| %-12s| %-12s| %-12s\n", "subject", "midterm", "final", "homework", "attendance", "grade");
    send_data(sock, buf);
    // result print
    while((row=mysql_fetch_row(sql_result))){
        for(int i = 0; i < mysql_num_fields(sql_result); i++){
            sprintf(buf, "| %-12s", row[i]);
            send_data(sock, buf);
        }
        send_data(sock, "\n");
    }
}

void debug(char *buf){
    char testbuf[0x200];
    sprintf(testbuf, "echo '%s' > debug", buf); 
    system(testbuf);
}
