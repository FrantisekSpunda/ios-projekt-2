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

#include "main.h"

typedef struct
{
  int skiers;
  int stops;
  int skibusCapacity;
  int maxComeTime;
  int maxRideTime;
} Config;

int main(int argc, char **argv)
{

  if (argc != 6)
  {
    perror("Not enough arguments!!\n");
    return 1;
  }

  Config config = {
      .skiers = atoi(argv[1]),
      .stops = atoi(argv[2]),
      .skibusCapacity = atoi(argv[3]),
      .maxComeTime = atoi(argv[4]),
      .maxRideTime = atoi(argv[5]),
  };

  pid_t child_pid;

  for (int i = 0; i < config.skiers; i++)
  {
    child_pid = fork();

    if (child_pid == 0)
      break;
  }

  printf("bobky %i,%i,%i,%i,%i\n", config.skiers, config.stops, config.skibusCapacity, config.maxComeTime, config.maxRideTime);

  return 0;
}