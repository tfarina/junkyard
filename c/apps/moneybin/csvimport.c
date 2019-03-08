/*https://www.quora.com/How-do-I-read-a-csv-file-and-then-use-those-fields-to-create-an-array-structure-in-C*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

#include "third_party/libcsv/csv.h"
#include "csv_helper.h"
#include "third_party/iniparser/iniparser.h"
#include "db_mysql.h"
#include "file.h"
#include "ffileutils.h"
#include "futils.h"
#include "stock.h"
#include "strutils.h"

#define USERCONFFILE ".traderc"

typedef struct {
  char const *host;
  char const *user;
  char const *password;
  char const *dbname;
  int unsigned port;
} db_config_t;

static int db_config_init(db_config_t *config)
{
  char const *homedir;
  char *userconffile;
  dictionary *ini;

  homedir = f_get_home_dir();
  userconffile = f_build_filename(homedir, USERCONFFILE);

  ini = iniparser_load(userconffile);
  if (ini == NULL) {
    fprintf(stderr, "Cannot read configuration file: %s\n", userconffile);
    return -1;
  }

  config->host = f_strdup(iniparser_getstring(ini, "mysql:host", NULL));
  config->user = f_strdup(iniparser_getstring(ini, "mysql:user", NULL));
  config->password = f_strdup(iniparser_getstring(ini, "mysql:password", NULL));
  config->dbname = f_strdup(iniparser_getstring(ini, "mysql:dbname", NULL));

  iniparser_freedict(ini);

  return 0;
}

int main(int argc, char **argv) {
  MYSQL *conn = NULL;
  size_t len;
  char *csvdata;
  struct csv_parser parser;
  int rc;
  stock_info_t stock;
  size_t bytes_processed;
  db_config_t config;
  size_t i;

  if (argc != 3) {
    fputs("usage: csvimport filename.csv symbol\n", stderr);
    return 1;
  }
 
  csvdata = read_file(argv[1], &len);
  if (csvdata == NULL)
    return 1;
 
  if (csv_init(&parser, CSV_APPEND_NULL) != 0) {
    free(csvdata);
    fprintf(stderr, "failed to initialize CSV parser\n");
    return 1;
  }
 
  memset((void *)&stock, 0, sizeof(stock_info_t));
  stock.symbol = f_strdup(argv[2]);
  stock.ticks_capacity = 2;
  stock.ticks = malloc(stock.ticks_capacity * sizeof(stock_tick_t));
  if (stock.ticks == NULL) {
    fprintf(stderr, "failed to allocate %zu bytes for stock data\n",
	    stock.ticks_capacity * sizeof(stock_tick_t));
    free(csvdata);
    return 1;
  }
 
  bytes_processed = csv_parse(&parser, (void*)csvdata, len,
                              csv_column_cb, csv_row_cb, &stock);
  rc = csv_fini(&parser, csv_column_cb, csv_row_cb, &stock);
  free(csvdata);
 
  if (stock.error || rc != 0 || bytes_processed < len) {
    fprintf(stderr,
            "read %zu bytes out of %zu: %s\n",
	    bytes_processed, len, csv_strerror(csv_error(&parser)));
    return 1;
  }

  db_config_init(&config);
 
  if ((conn = db_mysql_connect(config.host, config.user, config.password, config.dbname)) == NULL) {
    return -1;
  }

  /* NOTE: This will overwrite existing data. */
  printf("Importing records...\n");

  for (i = 0; i < stock.ticks_length; i++) {
    stock_tick_t *tick = stock.ticks + i;

    if (stock_add_tick(conn, &stock, tick) != -1) {
    }

    printf("symbol=\"%s\"; date=\"%s\"; open=%.4lf; high=%.4lf; low=%.4lf; close=%.4lf; adj_close=%.4lf; volume=%d\n",
           stock.symbol, tick->date, tick->open, tick->high, tick->low, tick->close, tick->adj_close, tick->volume);

    free(tick->date);
  }

  printf("%d rows imported\n", stock.ticks_length);

  free(stock.ticks);
 
  return 0;
}