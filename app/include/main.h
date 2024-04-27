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

/**
 * @brief struct for configuration filled by arguments
 *
 * @param skiers int - number of skiers
 * @param stops int - number of stops
 * @param skibusCapacity int - skibus capacity
 * @param maxComeTime int - max time that skier will come to stop
 * @param maxRideTime int - max time of ride between two stops
 */
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

/**
 * @brief struct for one skier
 *
 * @param id int - id of skier
 * @param stop_id int - id of stop that skier will be waiting
 * @param state skier_state_t - state of skier, he is in
 */
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

/**
 * @brief struct of shared information between processes
 *
 * @param skibus_stop_id int - id of stop that is skibus on or is going to be
 * @param skibus_filled int - how many skiers are in the skibus
 * @param counter int - counter of steps that we made
 */
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

/**
 * @brief function for skier process
 *
 * @param skier_id
 * @param skiers
 * @param storage
 * @param config
 */
void run_skier(int skier_id, skier_t *skiers, storage_t *storage, config_t config);

/**
 * @brief function for skibus process
 *
 * @param skiers
 * @param storage
 * @param config
 */
void run_skibus(skier_t *skiers, storage_t *storage, config_t config);

/**
 * @brief funtion for printing output messages in right format
 *
 * @param message_type
 * @param storage
 * @param skiers
 * @param i - index of skier or stop (depends on message_type)
 * @param shm_check - if 1 - runned with semaphore waiting / if 0 - no semaphore waiting (you must do it manualy)
 */
void print_out(message_type_t message_type, storage_t *storage, skier_t *skiers, int i, int shm_check);

/**
 * @brief fill `config` with arguments of program
 *
 * @param argc arguments count
 * @param argv argumnets array
 * @return config_t
 */
config_t config_setup(int argc, char **argv);

/**
 * @brief generate random number in range
 *
 * @param min
 * @param max
 * @return int
 */
int random_number(int min, int max);

/**
 * @brief close opened semaphore
 *
 */
void semaphore_destroy();

/**
 * @brief close opened shared memmory for skiers
 *
 * @param skiers_m
 * @param skiers
 */
void shm_skiers_destroy(int skiers_m, skier_t *skiers);

/**
 * @brief close opened shared memory for storage
 *
 * @param storage_m
 * @param storage
 */
void shm_storage_destroy(int storage_m, storage_t *storage);

#endif
