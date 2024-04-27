#ifndef _test_module_H_
#define _test_module_H_

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    // fork()
#include <sys/types.h> // pid_t
#include <semaphore.h> // semafor
#include <sys/shm.h>   // share memory
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>

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

typedef enum
{
  M_SKIER_START,
  M_SKIER_ARRIVED,
  M_SKIER_BOARDING,
  M_SKIER_SKIING,
  M_SKIBUS_START,
  M_SKIBUS_ARRIVED,
  M_SKIBUS_LEAVING,
  M_SKIBUS_ARRIVED_TO_FINAL,
  M_SKIBUS_LEAVING_FINAL,
  M_SKIBUS_FINISH
} message_type_t;

void run_skier(int skier_id, skier_t *skiers, storage_t *storage, config_t config);
void run_skibus(skier_t *skiers, storage_t *storage, config_t config);
void print_out(message_type_t message_type, storage_t *storage, skier_t *skiers, int i, int shm_check);
config_t config_setup(int argc, char **argv);
int random_number(int min, int max);
void semaphore_destroy();
void shm_skiers_destroy(int skiers_m, skier_t *skiers);
void shm_storage_destroy(int storage_m, storage_t *storage);

#endif
