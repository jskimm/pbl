#include <stdio.h>
#include <mysql.h>


//gcc -o EXEName $(mysql_config --cflags) PRGName.c $(mysql_config --libs)
int main(){
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW *row;

		
	char *server = "localhost";
	char *user = "root";
	char *password = "root"; 
	char *database = "practice";
	
	conn = mysql_init(NULL);
	
	/* Connect to database */
	if (!mysql_real_connect(conn, server, user, password, 
                                      database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
	
	/* send SQL query */
	if (mysql_query(conn, "select * from account")) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
   
	res = mysql_use_result(conn);
   
	while ((row = mysql_fetch_row(res)) != NULL)
		printf("%s %s %s\n", row[0], row[1], row[2]);
   
	/* close connection */
	mysql_free_result(res);
	mysql_close(conn);
	return 0;
}