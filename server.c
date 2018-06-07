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

#define GUEST_STATE 0x0
#define LOGIN_STATE 0x1

// DB ENV
#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASS "root"
#define DB_NAME "pbl"

void* clnt_connection(int sock);
void send_data(int sock, char* message);
int recv_data(int sock, char *buf);
void error_handling(char * message);
// void slice_str(const char * str, char * buffer, size_t start, size_t end);
int searchAll(MYSQL *);
int login(MYSQL *, char *id, char *pw);
char *info(MYSQL *conn, char *username);

int clnt_number=0;
int clnt_socks[10];
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

    // so angry!
    signal(SIGPIPE, SIG_IGN);

    // database init
    dbconn = NULL;
    mysql_init(&conn);
    dbconn = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char *)NULL, 0);
    if(dbconn == NULL){
            printf("connect error!\n");
            exit(1);
    }
    // ~DB init

    // DB CONNTEST
    searchAll(dbconn);
    // printf("%s", info(dbconn, "wwwlk"));

    if(argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    if(pthread_mutex_init(&mutx, NULL))
        error_handling("mutex init error");

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    while(1)
    {
        clnt_addr_size = sizeof(clnt_addr);   
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

        pthread_mutex_lock(&mutx);

        clnt_socks[clnt_number++]=clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&thread, NULL, (void *)clnt_connection, clnt_sock);
        printf(" IP : %s \n", inet_ntoa(clnt_addr.sin_addr));
    }
    return 0;

}

void *clnt_connection(int sock)
{
    int str_len=0;
    char message[BUFSIZE];
    char curUser[0x20];
    char username[0x20];
    char password[0x20];
    char command[0x20];
    char textbuf[0x40];
    unsigned int status = GUEST_STATE;
    int i;
    char *logo[] = {
        "[0] Exit\n",
        "[1] Info\n",
        "[?] bla bla\n"
    };

    //  debugging code
    // recv_data(sock, data);
    // sprintf(textbuf, "echo '%s' > input", data);
    // system(textbuf);
    send_data(sock, "id : ");
    recv_data(sock, username);
    send_data(sock, "pw : ");
    recv_data(sock, password);
    
    while((str_len=recv_data(sock, command)) != 0 ) {

        switch(status) {
            // TODO: before login
            case GUEST_STATE:
                if (login(dbconn, username, password) == 1) {
                    // print menu
                    send_data(sock, "---------------------------------------\n");
                    send_data(sock, "login\n");
                    sprintf(textbuf, "Hello mr.%s welcome.\n", username);
                    send_data(sock, textbuf);
                    for (i = 0; i < sizeof(logo) / sizeof(logo[0]); i++)
                        send_data(sock, logo[i]);
                    status = LOGIN_STATE;
                    strcpy(curUser, username);
                }
                else
                    send_data(sock, "input your passwd\n");
                break;

            case LOGIN_STATE:
                // if (command == NULL) break; 
                if ( !strcmp(command, "0") ) {
                    send_data(sock, "bye bye\n");
                    status = GUEST_STATE;
                    break;
                }
                else if ( !strcmp(command,"1") ) {
                    sprintf(textbuf, "%12s%12s%12s\n", "name", "score", "passwd");
                    send_data(sock, textbuf);
                    send_data(sock, info(dbconn, curUser));
                    break;
                }
                // print menu
                send_data(sock, "---------------------------------------\n");
                send_data(sock, "login\n");
                send_data(sock, "You are so suck\n");
                sprintf(textbuf, "Hello mr.%s welcome.\n", username);
                send_data(sock, textbuf);
                for (i = 0; i < sizeof(logo) / sizeof(logo[0]); i++)
                   send_data(sock, logo[i]);
                break;
            default:
                send_data(sock, "cant land heeeeere\n");
                break;
        }
    }

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

    close(sock);
    return 0;
}

void send_data(int sock, char *data)
{
    int len = strlen(data);

    pthread_mutex_lock(&mutx);
    write(sock, data, len);
    pthread_mutex_unlock(&mutx);
}

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

    // send query
    mysql_query(conn, "select * from user");
    // result saved
    sql_result=mysql_store_result(conn);
    // result parsing
    field=mysql_num_fields(sql_result);
    printf("%12s%12s%12s\n", "name", "score", "passwd");
    // result print
    while((row=mysql_fetch_row(sql_result))){
        for(int i = 0; i < field; i++)
            printf("%12s", row[i]);
        printf("\n");
    }
}


char *info(MYSQL *conn, char *username)
{
    int field;
    // return buffer
    static char buf[0x200];
    MYSQL_RES *sql_result;
    MYSQL_ROW row;

    sprintf(buf,"select * from user where name = \"%s\";", username);
    
    mysql_query(conn, "select * from user");
    sql_result=mysql_store_result(conn);
    field=mysql_num_fields(sql_result);
    row=mysql_fetch_row(sql_result);
    sprintf(buf,"%12s%12s%12s\n", row[0], row[1], row[2]);
    return buf;
}

int login(MYSQL *conn, char *id, char *pw)
{
    int field;
    char buf[0x200];
    MYSQL_RES *sql_result;
    MYSQL_ROW row;

    // changed preparesatement if you want
    sprintf(buf,"select count(*) from user where name = \"%s\" and passwd = \"%s\";", id, pw);
    mysql_query(conn, buf);
    sql_result = mysql_store_result(conn);
    field = mysql_num_fields(sql_result);
    row = mysql_fetch_row(sql_result);
    sprintf(buf, "%s", row[0]);
    if (!strncmp(buf, "0", 1))
        return 0;
    return 1;
}

// void slice_str(const char * str, char * buffer, size_t start, size_t end)
// {
//     size_t j = 0;
//     for ( size_t i = start; i <= end; ++i ) {
//         buffer[j++] = str[i];
//     }
//     buffer[j] = 0;
// }