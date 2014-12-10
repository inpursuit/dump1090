#include "sql_io.h"

void createTablesIfNotExist() {
  char *sql;
  char *errMsg = 0;
  int rc;

  sql = "CREATE TABLE ADSB_AIRCRAFT(" \
        "ICAO INTEGER PRIMARY KEY NOT NULL," \
        "flight TEXT," \
        "latitude NUMERIC," \
        "longitude NUMERIC," \
        "altitude INTEGER," \
        "speed INTEGER," \
        "course INTEGER," \
        "vert_rate INTEGER," \
        "messages INTEGER," \
        "modea INTEGER," \
        "modec INTEGER," \
        "modea_count INTEGER," \
        "modec_count INTEGER," \
        "modeac_flags INTEGER );";
  rc = sqlite3_exec(database, sql, 0, 0, &errMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Could not create table or table already exists: %s\n", errMsg);
    sqlite3_free(errMsg);
  }
}

void removeAllData() {
  char *sql;
  char *errMsg = 0;
  int rc;

  sql = "DELETE FROM ADSB_AIRCRAFT;";
  rc = sqlite3_exec(database, sql, 0, 0, &errMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Could not remove all data: %s\n", errMsg);
    sqlite3_free(errMsg);
  }
}

void modesDestroySQL() {
  if (database) {
    removeAllData();
    sqlite3_close(database);
  }
}

void modesInitSQL() {
  int rc = sqlite3_open(Modes.sql_file, &database);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Could not open database: %s\n", sqlite3_errmsg(database));
  } else {
    createTablesIfNotExist();
    removeAllData();
  }
}

void modesSQLAircraftRemoved(struct aircraft *a) {
  if (!database) {
    return;
  }

  char *sql;
  sqlite3_stmt *stmt;
  int rc;
  sql = "DELETE FROM ADSB_AIRCRAFT WHERE " \
        "ICAO = ?";

  rc = sqlite3_prepare(database, sql, -1, &stmt, 0);
  if (rc == SQLITE_OK) {
    sqlite3_bind_int(stmt, 1, a->addr);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
      fprintf(stderr, "Could not execute DELETE statement\n");
    }
  } else {
    fprintf(stderr, "Could not prepare statement\n");
  }
}

void modesSQLOutput(struct aircraft *a) {
  if (!database) {
    return;
  }

  char *sql;
  sqlite3_stmt *stmt;
  int rc;
  sql = "INSERT OR REPLACE INTO ADSB_AIRCRAFT" \
        "(ICAO,flight,latitude,longitude,altitude,speed," \
        "course,vert_rate,messages,modea,modec," \
        "modea_count,modec_count,modeac_flags) " \
        "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

  rc = sqlite3_prepare(database, sql, -1, &stmt, 0);
  if (rc == SQLITE_OK) {
    sqlite3_bind_int(stmt, 1, a->addr);
    sqlite3_bind_text(stmt, 2, &(a->flight[0]), 16, NULL);
    sqlite3_bind_double(stmt, 3, a->lat);
    sqlite3_bind_double(stmt, 4, a->lon);
    sqlite3_bind_int(stmt, 5, a->altitude);
    sqlite3_bind_int(stmt, 6, a->speed);
    sqlite3_bind_int(stmt, 7, a->track);
    sqlite3_bind_int(stmt, 8, a->vert_rate);
    sqlite3_bind_int(stmt, 9, (int)(a->messages));
    sqlite3_bind_int(stmt, 10, a->modeA);
    sqlite3_bind_int(stmt, 11, a->modeC);
    sqlite3_bind_int(stmt, 12, (int)(a->modeAcount));
    sqlite3_bind_int(stmt, 13, (int)(a->modeCcount));
    sqlite3_bind_int(stmt, 14, a->modeACflags);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
      fprintf(stderr, "Could not execute UPDATE statement\n");
    }
  } else {
    fprintf(stderr, "Could not prepare statement\n");
  } 
}
