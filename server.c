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

// DB env
#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASS "root"
#define DB_NAME "pbl"

void* clnt_connection(void * arg);
void send_message(char* message, int len);
void send_text(char* message);
void error_handling(char * message);
void slice_str(const char * str, char * buffer, size_t start, size_t end);
int searchAll(MYSQL *);
int login(MYSQL *, char *id, char *pw);
char *info(MYSQL *conn, char *username);

int clnt_number=0;
int clnt_socks[10];
MYSQL *connection;

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

    // DB init
    // mysql default setting is using thread. no need touch
    connection = NULL;
    mysql_init(&conn);
    connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char *)NULL, 0);
    if(connection == NULL){
            printf("connect error!\n");
            exit(1);
    }
    // ~DB init

    // test sector
    searchAll(connection);
    printf("%s", info(connection, "wwwlk"));
    // ~test

    if(argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    if(pthread_mutex_init(&mutx,NULL))
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

    //   FD_ZERO(&reads);
    //   FD_SET(serv_sock, &reads);
    //   fd_max = serv_sock;

    while(1)
    {
        clnt_addr_size = sizeof(clnt_addr);   
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

        pthread_mutex_lock(&mutx);

        clnt_socks[clnt_number++]=clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&thread, NULL, clnt_connection, (void*) clnt_sock);
        printf(" IP : %s \n", inet_ntoa(clnt_addr.sin_addr));
    }
    return 0;

}

void *clnt_connection(void *arg)
{
    /*
    *   @ bug: at status LOGIN_STATE, Segmentation fault
    */
    
    // TODO: `int clnt_sock = (int) arg;` <= this define so suck. please change
    int clnt_sock = (int) arg;
    int str_len=0;
    char message[BUFSIZE];
    char *userId_, *command;
    char curUser[0x20];
    char username[0x20];
    char textbuf[0x40];
    unsigned int status = GUEST_STATE;
    int i;
    char *logo[] = {
        "[0] Exit\n",
        "[1] Info\n",
        "[?] bla bla\n"
    };

    while((str_len=read(clnt_sock, message, sizeof(message))) != 0 ) {
        message[strlen(message) - 1] = 0;

        // message.split()
        userId_ = strtok(message, " ");
        command = strtok(NULL, " ");

        // userId_[1:-1]
        slice_str(userId_, username, 1, sizeof(userId_) - 3);

        switch(status) {
            // TODO: before login
            case GUEST_STATE:
                if (login(connection, username, command) == 1) {
                    // print menu
                    send_text("---------------------------------------\n");
                    send_text("login\n");
                    send_text("You are so suck\n");
                    sprintf(textbuf, "Hello mr.%s welcome.\n", username);
                    send_text(textbuf);
                    for (i = 0; i < sizeof(logo) / sizeof(logo[0]); i++)
                        send_text(logo[i]);
                    status = LOGIN_STATE;
                    strcpy(curUser, username);
                }
                else
                    send_text("input your passwd\n");
                break;

            case LOGIN_STATE:
                // if this code write at client, very very annoying
                // TODO: this code occur Segmentation fault
                if (command == NULL) break; 
                if (command[0] == '0') {
                    send_text("bye bye\n");
                    status = GUEST_STATE;
                    break;
                }
                else if (command[0] == '1') {
                    sprintf(textbuf, "%12s%12s%12s\n", "name", "score", "passwd");
                    send_text(textbuf);
                    send_text(info(connection, curUser));
                    break;
                }
                // print menu
                send_text("---------------------------------------\n");
                send_text("login\n");
                send_text("You are so suck\n");
                sprintf(textbuf, "Hello mr.%s welcome.\n", username);
                send_text(textbuf);
                for (i = 0; i < sizeof(logo) / sizeof(logo[0]); i++)
                   send_text(logo[i]);
                break;
            default:
                send_text("cant land heeeeere\n");
                break;
        }
    }

    pthread_mutex_lock(&mutx);
    for(i=0;i<clnt_number;i++){ 
        if(clnt_sock == clnt_socks[i])
        {
            for(;i<clnt_number-1;i++)
            clnt_socks[i] = clnt_socks[i+1];
            break;
        }
    }
    clnt_number--;

    pthread_mutex_unlock(&mutx);

    close(clnt_sock);
    return 0;
}

void send_message(char * message, int len)
{
    int i;
    pthread_mutex_lock(&mutx);

    for(i=0;i<clnt_number;i++)
        write(clnt_socks[i], message, len);
    pthread_mutex_unlock(&mutx);
}

void send_text(char * message)
{
    int len = strlen(message);
    int i;

    pthread_mutex_lock(&mutx);

    for(i=0;i<clnt_number;i++)
        write(clnt_socks[i], message, len);
    pthread_mutex_unlock(&mutx);
}

void error_handling(char * message)
{
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}

// test: mysql usage
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
// ~test

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

void slice_str(const char * str, char * buffer, size_t start, size_t end)
{
    size_t j = 0;
    for ( size_t i = start; i <= end; ++i ) {
        buffer[j++] = str[i];
    }
    buffer[j] = 0;
}
