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
int login(MYSQL *, char *id, char *pw, char *num, char *name);

/* STUDENT */
void printAttendance(int sock, MYSQL *conn, char *num);
void printScore(int sock, MYSQL *conn, char *num);
/* PROFESSOR */
int insertAttendance(int sock, MYSQL *conn, char *num);
int insertScore(int sock, MYSQL *conn, char *num);
/* MESSAGE */
int sendMessage(int sock, MYSQL *conn, char *from, int state);
int messageBox(int sock, MYSQL *conn, char *num, int state);

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
    pthread_attr_t attr;
    MYSQL conn;
    MYSQL_RES *sql_result;
    MYSQL_ROW row;
    int field;
    int query_stat;

    signal(SIGPIPE, SIG_IGN);

    if(argc != 2) {
        printf("사용법 : %s <포트>\n", argv[0]);
        exit(1);
    }

    // 데이터베이스 초기화 및 연결
    dbconn = NULL;
    mysql_init(&conn);
    mysql_options(&conn, MYSQL_SET_CHARSET_NAME, "utf8");       // set encoding
    mysql_options(&conn, MYSQL_INIT_COMMAND, "SET NAMES utf8"); // set encoding
    dbconn = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char *)NULL, 0);
    if(dbconn == NULL) {
        printf("DB 연결 에러\n");
        exit(1);
    }
    // DB TEST
    searchAll(dbconn);

    if(pthread_mutex_init(&mutx, NULL))
        error_handling("MUTEX 초기화 에러");

    // if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)!=0)
    //     error_handling("setdetachstate 에러");

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
        pthread_detach(thread);
    }
    return 0;

}

// 이 부분이 주 루틴
void *clnt_connection(int sock)
{
    int result;
    char id[0x20];
    char pw[0x20];
    char num[0x10];
    char name[0x10];
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
        "[3] 메시지 전송\n",
        "[4] 메시지함\n",
    };

    char *prof_menu[] = {
        "[0] 클라이언트 종료\n",
        "[1] 출결 관리\n",
        "[2] 성적 입력\n",
        "[3] 메시지 전송\n",
        "[4] 메시지함\n",
    };

    recv_data(sock, id); // SOCK_STREAM 버퍼 비움

    /* 로그인 기능 */
    while(1){
        result = 0;
        send_data(sock, "[ ID 입력 ]\n");
        recv_data(sock, id);
        send_data(sock, "[ PW 입력 ]\n");
        recv_data(sock, pw);
        
        result = login(dbconn, id, pw, num, name);
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
        sprintf(textbuf, "%s(%s)님 반갑습니다.\n", name, num);
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
                sendMessage(sock, dbconn, num, state);
            } else if ( !strcmp(command,"4") ){
                messageBox(sock, dbconn, num, state);
            } else if ( !strcmp(command,"0") ){
                state = 9;
            }
            else continue;
            break;

            case PROFESSOR: // 교수 화면
            for (i = 0; i < sizeof(prof_menu) / sizeof(prof_menu[0]); i++)
                send_data(sock, prof_menu[i]); 
            recv_data(sock, command);
            if ( !strcmp(command,"1") ) { // 1번 선택 시
                insertAttendance(sock, dbconn, num);
            } else if ( !strcmp(command,"2") ){
                insertScore(sock, dbconn, num);
            } else if ( !strcmp(command,"3") ){
                sendMessage(sock, dbconn, num, state);
            } else if ( !strcmp(command,"4") ){
                messageBox(sock, dbconn, num, state);
            } else if ( !strcmp(command,"0") ){
                state = 9;
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
    while(1) {
        str_len = read(sock, buf, BUFSIZE-1);
        // if(str_len == -1) return (void*)1;
        if (str_len > 0 && buf[str_len - 1] == '\n') 
            buf[str_len - 1] = '\0';

        if(str_len == 0) return (void*)-1;
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

    mysql_query(conn, "select * from account");     // send query
    sql_result=mysql_store_result(conn);            // result saved
    field=mysql_num_fields(sql_result);             // result parsing
    while((row=mysql_fetch_row(sql_result))){       // result print
        for(int i = 0; i < field; i++)
            printf("%12s", row[i]);
        printf("\n");
    }
}

int login(MYSQL *conn, char *id, char *pw, char *num, char *name)
{
    int field;
    char buf[0x200];
    MYSQL_RES *sql_result;
    MYSQL_ROW row;

    sprintf(buf,"select isprof, num, name from account where id = \"%s\" and pw = \"%s\";", id, pw);
    mysql_query(conn, buf);
    sql_result = mysql_store_result(conn);
    row = mysql_fetch_row(sql_result);
    
    if ( mysql_num_rows(sql_result) ){ // 쿼리 결과가 있을 경우
        
        sprintf(buf, "%s", row[0]);
        sprintf(num, "%s", row[1]);           // number 변수에 학번/교번 복사
        sprintf(name, "%s", row[2]);          // number 변수에 이름 복사
        if (!strncmp(buf, "0", 1))
            return 0;     // 학생은 0 반환
        else if(!strncmp(buf, "1", 1))
            return 1;     // 교수는 1 반환
    }
    return 9;

}

void printAttendance(int sock, MYSQL *conn, char *num)
{
    char buf[0x200];
    MYSQL_RES *sql_result;
    MYSQL_ROW row;

    sprintf(buf,"select (select name from subject where subject_code=attendance.subject_code), type, date, reason from attendance where student_num=\"%s\";", num);
    mysql_query(conn, buf);
    sql_result = mysql_store_result(conn);

    sprintf(buf, "| %-25s| %-25s| %-25s| %-25s\n", "name", "type", "date", "reason");
    send_data(sock, buf);
    // result print
    while((row=mysql_fetch_row(sql_result))){
        for(int i = 0; i < mysql_num_fields(sql_result); i++){
            sprintf(buf, "| %-25s", row[i]);
            send_data(sock, buf);
        }
        send_data(sock, "\n");
    }
}

void printScore(int sock, MYSQL *conn, char *num){
    char buf[0x200];
    MYSQL_RES *sql_result;
    MYSQL_ROW row;

    sprintf(buf,"select (select name from subject where subject_code=score.subject_code), midterm, final, homework, attendance, grade from score where student_num=\"%s\";", num);
    mysql_query(conn, buf);
    sql_result = mysql_store_result(conn);

    sprintf(buf, "| %-15s| %-15s| %-15s| %-15s| %-15s| %-15s\n", "subject", "midterm", "final", "homework", "attendance", "grade");
    send_data(sock, buf);
    // result print
    while((row=mysql_fetch_row(sql_result))){
        for(int i = 0; i < mysql_num_fields(sql_result); i++){
            sprintf(buf, "| %-15s", row[i]);
            send_data(sock, buf);
        }
        send_data(sock, "\n");
    }
}

int insertAttendance(int sock, MYSQL *conn, char *num){
    char buf[0x200];
    char subject_code[0x10];
    char student_num[0x10];
    char type[0x10];
    char reason[0x100]="";
    MYSQL_RES *sql_result;
    MYSQL_ROW row;

    // 과목 선택
    // TODO : 교수가 강의하지 않는 과목도 질의 가능
    send_data(sock, "[*] 현재 강의 중인 과목\n");
    sprintf(buf,"select subject_code, name, credit from subject where prof_num=\"%s\";", num);
    mysql_query(conn, buf);
    sql_result = mysql_store_result(conn);
    if (!mysql_num_rows(sql_result) ){ // 쿼리 결과가 있을 경우
        send_data(sock, "[-] 현재 강의중인 과목이 없습니다.\n");
        return 0;    
    }
    sprintf(buf, "| %-15s| %-15s| %-15s\n", "subject code", "subject", "credit");
    send_data(sock, buf);
    while((row=mysql_fetch_row(sql_result))){
        for(int i = 0; i < mysql_num_fields(sql_result); i++){
            sprintf(buf, "| %-15s", row[i]);
            send_data(sock, buf);
        }
        send_data(sock, "\n");
    }
    send_data(sock, "\n[*] 수강생을 조회할 과목 코드를 입력하세요.\n");
    recv_data(sock, subject_code); // 과목코드 입력

    // 학생 모두 출력
    sprintf(buf,"select student_num, (select name from student where student_num=score.student_num) from score where subject_code=\"%s\";", subject_code);
    mysql_query(conn, buf);
    sql_result = mysql_store_result(conn);
    if (!mysql_num_rows(sql_result)){
        send_data(sock, "[-] 해당 과목이 존재하지 않습니다.\n");
        return 0; // 쿼리 결과가 없으면 0 반환
    }
    send_data(sock, "[*] 수강생 목록\n");

    sprintf(buf, "| %-15s| %-15s\n", "number", "name");
    send_data(sock, buf);
    while((row=mysql_fetch_row(sql_result))){
        for(int i = 0; i < mysql_num_fields(sql_result); i++){
            sprintf(buf, "| %-15s", row[i]);
            send_data(sock, buf);
        }
        send_data(sock, "\n");
    }
    // 학번 입력
    send_data(sock, "[*] 학번을 입력하세요.\n");
    recv_data(sock, student_num);
    
    // 입력한 학번 존재? --> 학생의 날짜 출결 사유 순서대로 입력
    sprintf(buf,"select distinct student.student_num, name, tel from score join student where subject_code=\"%s\" and student.student_num=\"%s\" and score.student_num=\"%s\";", subject_code, student_num, student_num);
    mysql_query(conn, buf);
    sql_result = mysql_store_result(conn);
    if ( mysql_num_rows(sql_result) ){ // 쿼리 결과가 있을 경우
        // 종류 입력 (absent, late, early)
        send_data(sock, "[*] 출결 정보를 입력하세요. (absent, late, early)\n");
        recv_data(sock, type);
        // 사유 입력
        send_data(sock, "[*] 사유를 입력하세요. (미 입력 시 공란)\n");
        recv_data(sock, reason);
        // db insert
        sprintf(buf, "insert into attendance values (\"%s\", \"%s\", \"%s\", now(), \"%s\");", 
                        subject_code, student_num, type, reason);
        mysql_query(conn, buf);
        return 1;
    }
    else send_data(sock, "[-] 해당 학번이 존재하지 않습니다.\n");
    return 0;
}

int insertScore(int sock, MYSQL *conn, char *num) {
    char midterm[20], final[20], homework[20], attendance[20];
    char student_num[20];
    char subject_code[20];
    char buf[200];
    MYSQL_RES *sql_result;
    MYSQL_ROW row;

    send_data(sock, "[*] 현재 강의 중인 과목\n");
    sprintf(buf,"select subject_code, name, credit from subject where prof_num=\"%s\";", num);
    mysql_query(conn, buf);
    sql_result = mysql_store_result(conn);
    if (!mysql_num_rows(sql_result) ){ // 쿼리 결과가 있을 경우
        send_data(sock, "[-] 현재 강의중인 과목이 없습니다.\n");
        return 0;    
    }
    sprintf(buf, "| %-15s| %-15s| %-15s\n", "subject code", "subject", "credit");
    send_data(sock, buf);
    while((row=mysql_fetch_row(sql_result))){
        for(int i = 0; i < mysql_num_fields(sql_result); i++){
            sprintf(buf, "| %-15s", row[i]);
            send_data(sock, buf);
        }
        send_data(sock, "\n");
    }
    send_data(sock, "\n[*] 수강생을 조회할 과목 코드를 입력하세요.\n");
    recv_data(sock, subject_code); // 과목코드 입력

    // 학생 모두 출력
    sprintf(buf,"select student_num, (select name from student where student_num=score.student_num) from score where subject_code=\"%s\";", subject_code);
    mysql_query(conn, buf);
    sql_result = mysql_store_result(conn);
    if (!mysql_num_rows(sql_result)){
        send_data(sock, "[-] 해당 과목이 존재하지 않습니다.\n");
        return 0; // 쿼리 결과가 없으면 0 반환
    }
    send_data(sock, "[*] 수강생 목록\n");

    sprintf(buf, "| %-15s| %-15s|\n", "number", "name");
    send_data(sock, buf);
    while((row=mysql_fetch_row(sql_result))){
        for(int i = 0; i < mysql_num_fields(sql_result); i++){
            sprintf(buf, "| %-15s", row[i]);
            send_data(sock, buf);
        }
        send_data(sock, "\n");
    }
    //학생 성적 입력 부분
    send_data(sock, "[*] 성적을 입력할 학생의 학번을 입력하세요.\n");
    recv_data(sock, student_num);

    //입력한 학번이 데이터 베이스 안에 존재하지 않을때 오류메시지 출력
    sprintf(buf,"select * from score where student_num=\"%s\" and subject_code=\"%s\";", student_num, subject_code);
    mysql_query(conn, buf);
    sql_result = mysql_store_result(conn);
    if ( mysql_num_rows(sql_result) ){ // 쿼리 결과가 있을 경우
        send_data(sock, "[*] 중간 : \n");
        recv_data(sock, midterm);
        send_data(sock, "[*] 기말 : \n");
        recv_data(sock, final);
        send_data(sock, "[*] 과제 : \n");
        recv_data(sock, homework);
        send_data(sock, "[*] 출석 : \n");
        recv_data(sock, attendance);
        send_data(sock, "\n");
        sprintf(buf, "update score set midterm=%s, final=%s, homework=%s, attendance=%s where student_num=\"%s\" and subject_code=\"%s\";", 
                    midterm, final, homework, attendance, student_num, subject_code);
        mysql_query(conn, buf);
        send_data(sock, "[*] 입력 완료\n");
        return 1;
    } else send_data(sock, "\n[-] 해당 학번이 존재하지 않습니다.\n");
    
   return 0;
}



int sendMessage(int sock, MYSQL *conn, char *from, int state){
    char buf[250];
    char msg[200];
    char to[20];
    MYSQL_RES *sql_result;
    MYSQL_ROW row;

    send_data(sock, "받는이의 학번 또는 교번을 입력하세요.\n");
    recv_data(sock, to);

    sprintf(buf,"select * from account where num=\"%s\";", to);
    mysql_query(conn, buf);
    sql_result = mysql_store_result(conn);
    if (!mysql_num_rows(sql_result) ){ // 쿼리 결과가 있을 경우
        send_data(sock, "[-] 없는 사용자 입니다.\n");
        return 0;    
    }
    send_data(sock, "[*] 보낼 내용을 입력하세요. (최대 200byte)\n");
    recv_data(sock, msg);
    
    sprintf(buf, "insert into message values (\"%s\", \"%s\", \"%s\", now());", from, to, msg);
    mysql_query(conn, buf);
    send_data(sock, "[*] 전송 완료\n");
    return 1;
}

int messageBox(int sock, MYSQL *conn, char *num, int state){
    char buf[250];
    MYSQL_RES *sql_result;
    MYSQL_ROW row;

    send_data(sock, "[*] 메시지함\n");
    sprintf(buf,"select (select name from account where num=message.from), sendtime, content from message where message.to=\"%s\";", num);
    mysql_query(conn, buf);
    sql_result = mysql_store_result(conn);
    if (!mysql_num_rows(sql_result) ){ // 쿼리 결과가 있을 경우
        send_data(sock, "[-] 메시지가 없습니다.\n");
        return 0;    
    } 
    
    while((row=mysql_fetch_row(sql_result))){
        sprintf(buf, "보낸이 : %s | ", row[0]);
        send_data(sock, buf);
        sprintf(buf, "보낸 날짜 : %s\n", row[1]);
        send_data(sock, buf);
        sprintf(buf, "%s\n", row[2]);
        send_data(sock, buf);
        send_data(sock, "\n");
    }
    return 1;
}

void debug(char *buf){
    char testbuf[0x200];
    sprintf(testbuf, "echo '%s' > debug", buf); 
    system(testbuf);
}

