#include <stdio.h>
#include <sqlite3.h>

// http://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm

static void html_content() {
  printf("Content-type: text/html\r\n\r\n");
}

static void html_start() {
  printf("<!DOCTYPE html>");
  printf("<html>\n");
}

static void html_end() {
  printf("</body>\r\n");
  printf("</html>\r\n");
}

static const char kDbName[] = "users.db";

int main(int argc, char* argv[]) {
  html_content();
  html_start();
  printf("<head>\n");
  printf("</head>\n");
  printf("<body>\n");

  sqlite3* db;
  int rc = sqlite3_open(kDbName, &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return rc;
  } else {
    fprintf(stdout, "Opened database successfully\n");
  }
  sqlite3_close(db);

  html_end();
  return 0;
}
