#ifndef _test_module_H_
#define _test_module_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    // fork()
#include <sys/types.h> // pid_t
#include <semaphore.h> // semafor
#include <sys/shm.h>   // share memory
#include <time.h>
#include <unistd.h>

#define MAX_SKIERS 20000
#define MAX_STOPS 10
#define ARGC 6

typedef struct
{
  int skiers;
  int stops;
  int skibusCapacity;
  int maxComeTime;
  int maxRideTime;
} config_t;

typedef enum
{
  BREAKFAST,
  WAITING,
  ONRIDE,
  IN_FINISH,
} skier_state_t;

typedef struct
{
  int id;
  int stop_id;
  skier_state_t state;
} skier_t;

typedef enum
{
  RIDE,
  ON_STOP,
} skibus_state_t;

typedef struct
{
  int skibus_stop_id;
  int skibus_filled;
  int counter;
} storage_t;

void run_skier(int skier_id, skier_t *skiers, storage_t *storage, config_t config);
void run_skibus(skier_t *skiers, storage_t *storage, config_t config);

#endif
