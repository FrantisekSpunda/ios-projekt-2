/**
 * @file helpers.c
 * @author Your name
 * @date 2024-26-04
 * @brief Description of your project
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "helpers.h"

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