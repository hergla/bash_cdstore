#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#define DB "/.chdir.db"

bool error = false;
char *dbfile;
sqlite3 *db;
char *err_msg = 0;
bool verbose = false;

char **dir_array = NULL;
int dir_array_count = 0;


/* Diese callback ist nur fuer folgendes SQL nutzbar.
 * select_stmt("select dir from chdir order by date_time desc limit 20;");
 */
static int callback(void *nrecs_backPtr, int argc, char **argv, char **azColName) {
	int i;
   	for(i = 0; i<argc; i++) {
   		printf("%d - %s\n", (*(int *)nrecs_backPtr), argv[i] );
		dir_array = (char **)realloc(dir_array, (dir_array_count +1) * sizeof(char *));
		dir_array[dir_array_count] = malloc(strlen(argv[i])+1);
		strcpy( dir_array[dir_array_count], argv[i]);
		dir_array_count++;
   	}
	(*(int *)nrecs_backPtr)++;
   	return 0;
}

bool create_db() {
	char *sql = "CREATE TABLE IF NOT EXISTS chdir(" \
		   "DIR TEXT            NOT NULL," \
		   "DATE_TIME TEXT      NOT NULL);";     

	int rc = sqlite3_open(dbfile, &db);
	if (rc != SQLITE_OK) { 
		fprintf(stderr, "Error: %s - %s\n", dbfile, sqlite3_errmsg(db));
		return false;
	}
	rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
	if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", err_msg);
      sqlite3_free(err_msg);
	  return false;
	}
   	return true;	
}

bool insert_row(char *dir, char *datetime) {
	sqlite3_stmt *res;
	char *sql = "insert into chdir (dir, date_time) values (?,?)";
	int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    
    if (rc == SQLITE_OK) {
        if (sqlite3_bind_text(res, 1, dir, -1, SQLITE_STATIC) != SQLITE_OK) {
			printf("Could not bind double.\n");
		}
        sqlite3_bind_text(res, 2, datetime, -1, SQLITE_STATIC);
    } else {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }
	if (sqlite3_step(res) != SQLITE_DONE) {
    	printf("\nCould not step (execute) stmt.\n");
    	return false;
	}
	sqlite3_finalize(res);
}

bool update_dir(char *dir, char *datetime) {
	sqlite3_stmt *res;
	char *sql = "update chdir set date_time = ? where dir = ?";

	int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc == SQLITE_OK) {
        if (sqlite3_bind_text(res, 1, datetime, -1, SQLITE_STATIC) != SQLITE_OK) {
			printf("Could not bind double.\n");
		}
        sqlite3_bind_text(res, 2, dir, -1, SQLITE_STATIC);
    } else {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }
	if (sqlite3_step(res) != SQLITE_DONE) {
    	printf("\nCould not step (execute) stmt.\n");
    	return false;
	}
	sqlite3_finalize(res);
}

int select_stmt(const char *statement) {
	char *errmsg;
	int rc; 
	int nrecs=0;

	rc = sqlite3_exec(db, statement, callback, &nrecs, &errmsg);
	if (rc != SQLITE_OK) { 
		printf("Error: %s \n", errmsg);
		return 0;	
	}
	//printf("\n%d records found\n", nrecs);
	return nrecs;
}

bool dir_exists(char *dir) {
	sqlite3_stmt *res;
	int count;
	char *sql = "select count(*) from chdir where dir = ?";
	
	sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_bind_text(res, 1, dir, -1, SQLITE_STATIC);
	sqlite3_step(res);
	count = sqlite3_column_int(res,0);
	sqlite3_finalize(res);
	if (count == 0 ) 
		return false;
	return true;
}


char *get_datetime() { 
	static char buffer[80];
	time_t rawtime;
	struct tm *info;

	time(&rawtime);
	info = localtime(&rawtime);
	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", info);
	return buffer;
}

int get_input() {
	char buf[32];
	printf("Enter row number: ");
	fflush(stdout);
	fgets(buf, sizeof(buf), stdin);
	return atoi(buf);
}

int main(int argc, char* argv[]) {
	struct passwd *passwd;
	uid_t uid;
	size_t db_path_len = 0;
	char *cur_dir;

	//printf("%s\n", sqlite3_libversion());
	uid = geteuid();
	passwd = getpwuid(uid);
	//printf("UID: %d  -  HOME: %s\n", uid, passwd->pw_dir);
	//printf("strlen home: %d\n", strlen(passwd->pw_dir));
	//printf("strlen DB: %d\n", strlen(DB));
	db_path_len = strlen(passwd->pw_dir) + strlen(DB) + 1;
	dbfile = malloc(strlen(passwd->pw_dir) + strlen(DB) + 1);
	//printf("db_path_len: %d\n", db_path_len);
	memset(dbfile, 0x0, db_path_len);
	sprintf(dbfile, "%s%s", passwd->pw_dir, DB);
	// printf("db_path: %s\n", dbfile);
	
	create_db();

	cur_dir = getcwd(NULL, 0);
	char *now = get_datetime();
	if ( argc >1 ) {   //  with arg; add or update row
		if (dir_exists(cur_dir)) {
			update_dir(cur_dir, now);
		} else {
			insert_row(cur_dir , now);
		}
	} else {
		select_stmt("select dir from chdir order by date_time desc limit 20;");
		int x = get_input();
		if ( x < dir_array_count ) {
			fprintf(stderr, "%s", dir_array[x]);
		} else {
			fprintf(stderr, ".");
		}
	}
	sqlite3_close(db);
}
