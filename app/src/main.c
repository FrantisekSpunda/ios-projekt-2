/**
 * @file main.c
 * @author Your name
 * @date 2024-23-04
 * @brief Description of your project
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    // fork()
#include <sys/types.h> // pid_t
#include <semaphore.h> // semafor
#include <sys/shm.h>   // share memory

#include "main.h"

typedef struct
{
  int skiers;
  int stops;
  int skibusCapacity;
  int maxComeTime;
  int maxRideTime;
} arguments_t;

typedef enum
{
  WAITING,
  BREAKFAST,
  ONRIDE,
} skier_state_t;

typedef struct
{
  int id;
  skier_state_t state;
} skier_t;

#define SHARED_MEMORY_SIZE 1024

int main(int argc, char **argv)
{

  /****** args ******/
  if (argc != 6)
  {
    fprintf(stderr, "Not enough arguments!!\n");
    return 1;
  }

  arguments_t config = {
      .skiers = atoi(argv[1]),
      .stops = atoi(argv[2]),
      .skibusCapacity = atoi(argv[3]),
      .maxComeTime = atoi(argv[4]),
      .maxRideTime = atoi(argv[5]),
  };
  /****** args ******/

  /****** shm ******/
  key_t key = ftok("/tmp", 'A');
  int shmid = shmget(key, SHARED_MEMORY_SIZE, IPC_CREAT | 0666);

  if (shmid == -1)
  {
    perror("shmget");
    return 1;
  }

  /****** shm ******/

  pid_t child_pid;

  // skiers
  for (int i = 0; i < config.skiers; i++)
  {
    child_pid = fork();

    if (child_pid == 0)
      break;
    if (child_pid < 0)
    {
      fprintf(stderr, "Error while forking with error \"%i\"", child_pid);
      return 1;
    }
  }

  // IN FORKS

  int *shared_memory = shmat(shmid, NULL, 0);

  if (shared_memory == (char *)-1)
  {
    fprintf(stderr, "shmat");
    return 1;
  }

  *shared_memory = *shared_memory + 1;

  printf("memory %i\n", *shared_memory);
  printf("bobky %i,%i,%i,%i,%i\n", config.skiers, config.stops, config.skibusCapacity, config.maxComeTime, config.maxRideTime);

  shmdt(shared_memory);

  if (child_pid == 0)
    return 0;

  for (int i = 0; i < 3; i++)
  { // Uprav podle počtu procesů, které jsi vytvořil
    wait(NULL);
  }

  // Vyčištění sdílené paměti
  shmctl(shmid, IPC_RMID, NULL);
  return 0;
}