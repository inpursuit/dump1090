#ifndef SQL_H
#define SQL_H

#ifndef _WIN32
  #include <stdio.h>
  #include <stdlib.h>
  #include <signal.h>
  #include "dump1090.h"
  #include <sqlite3.h>
#endif

#define TIMEOUT_FREQ 2
#define TIMEOUT_AGE 30

struct aircraft;
sqlite3 *database;

void modesDestroySQL (void);
void modesInitSQL    (void);
void modesSQLAircraftRemoved(struct aircraft *a);
void modesSQLOutput (struct aircraft *a);

#endif
