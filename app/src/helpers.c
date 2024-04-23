/**
 * @file helpers.c
 * @author Frantisek Spunda
 * @date 2024-23-04
 * @brief Second project to IOS
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "helpers.h"

void shm_free(pid_t child_pid, config_t config, int shmid)
{
  if (child_pid == 0)
    exit(0);

  // printf("Arguments: skiers-%i ; stops-%i ; skibusCapacity-%i ; maxComeTime-%i ; maxRideTime-%i\n",
  //        config.skiers, config.stops, config.skibusCapacity, config.maxComeTime, config.maxRideTime);

  for (int i = 0; i < config.skiers; i++)
  { // Uprav podle počtu procesů, které jsi vytvořil
    wait(NULL);
  }

  // Vyčištění sdílené paměti
  shmctl(shmid, IPC_RMID, NULL);
}

config_t config_setup(int argc, char **argv)
{
  if (argc != ARGC)
  {
    fprintf(stderr, "Not enough arguments!!\n");
    exit(1);
  }

  config_t config = {
      .skiers = atoi(argv[1]),
      .stops = atoi(argv[2]),
      .skibusCapacity = atoi(argv[3]),
      .maxComeTime = atoi(argv[4]),
      .maxRideTime = atoi(argv[5]),
  };

  return config;
}

int random_number(int min, int max)
{
  return rand() % ((max + 1) - min) + min;
}