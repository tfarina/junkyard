#include <stdio.h>
#include <stdlib.h>

#include "db.h"
#include "user.h"

// http://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm

/* The name of the user database file.  */
static const char user_db_fname[] = "users.db";

// static int callback(void* data, int argc, char** argv, char** column_name) {
//   int i;
//   //fprintf(stdout, "%s: \n", (const char*)data);
//   for (i = 0; i < argc; ++i) {
//     printf("%-12s %s\n", column_name[i], argv[i] ? argv[i] : "NULL");
//   }
//   printf("\n");
//   return 0;
// }

static int db_user_update_email(sqlite3* db,
                                const char *username,
                                const char *email) {
  sqlite3_stmt *stmt;
  int rv;

  const char *sql = "UPDATE user SET email=?1 WHERE login=?2;";

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "error preparing update statement: %s\n",
            sqlite3_errmsg(db));
    return -1;
  }

  rv = sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);

  if (rv == SQLITE_OK)
    rv = sqlite3_bind_text(stmt, 2, username, -1, SQLITE_STATIC);

  if (rv != SQLITE_OK) {
    fprintf(stderr, "error binding a value for the user table: %s\n",
            sqlite3_errmsg(db));
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "error updating user table: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  sql_stmt_free(stmt);

  return 0;
}

int main(int argc, char **argv) {
  sqlite3* db;

  if (argc != 3) {
    fprintf(stderr, "usage: %s USERNAME E-MAIL\n", argv[0]);
    return -1;
  }

  db = db_open(user_db_fname);
  if (!db) {
    return -1;
  }

  if (db_user_create_table(db)) {
    sqlite3_close(db);
    return -1;
  }

  if (!db_user_exists(db, argv[1])) {
    fprintf(stderr, "%s: user (%s) does not exist in our database.\n", argv[0],
            argv[1]);
    sqlite3_close(db);
    return -1;
  }

  if (db_user_update_email(db, argv[1], argv[2])) {
    sqlite3_close(db);
    return -1;
  }

  sqlite3_close(db);

  return 0;
}