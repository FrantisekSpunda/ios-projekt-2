/**
 * @file main.c
 * @author Frantisek Spunda
 * @date 2024-23-04
 * @brief Second project to IOS
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "main.h"
#include "helpers.h"
#include <asm-generic/fcntl.h>

sem_t *skier_wait;

int main(int argc, char **argv)
{
  skier_wait = sem_open("/my_semaphore", O_CREAT | O_EXCL, 0644, 1);
  config_t config = config_setup(argc, argv);

  /****** fork inicialization ******/
  pid_t child_pid;
  int process_id = 0;

  for (int i = 0; i < config.skiers + 1; i++)
  {
    child_pid = fork();

    if (child_pid == 0)
      break;
    else if (child_pid < 0)
    {
      fprintf(stderr, "Error \"fork\" with numb. \"%i\"", child_pid);
      exit(1);
    }
    else
    {
      process_id++;
      srand(time(NULL) * (getpid() + process_id));
    }
  }
  /****** fork inicialization ******/

  /****** shared memory ******/
  key_t key = ftok("/tmp", 'A');

  int skiers_m = shmget(key, sizeof(skier_t) * config.skiers, IPC_CREAT | 0666);
  if (skiers_m == 1)
  {
    fprintf(stderr, "Error \"shmget\"");
    exit(1);
  }

  skier_t *skiers = shmat(skiers_m, NULL, 0);
  if (skiers == -1)
  {
    fprintf(stderr, "Error \"shmget\"");
    exit(1);
  }

  if (process_id == 0) // only in main process
  {

    for (int i = 0; i < config.skiers; i++)
    {
      skiers[i].id = i + 1;
      skiers[i].state = BREAKFAST;
      skiers[i].stop_id = random_number(0, 9);
    }
  }
  /****** shared memory ******/

  if (process_id == 1)
  {
    printf("SKIBUS START\n");
    run_skibus(skiers, config);
  }

  if (process_id > 1) // only not in main process
  {
    printf("SKIER START\n");
    run_skier(process_id - 1, skiers, config);
  }

  sem_close(skier_wait);
  sem_unlink("/my_semaphore");
  shmdt(skiers);
  shm_free(child_pid, config, skiers_m);

  return 0;
}

void run_skibus(skier_t *skiers, config_t config)
{

  for (int stop = 1; stop < config.stops + 1; stop++)
  {
    usleep(random_number(0, config.maxRideTime));

    sem_wait(skier_wait);
    for (int i = 0; i < config.skiers; i++)
    {
      if (skiers[i].state == WAITING && skiers[i].stop_id == stop)
      {
        skiers[i].state = ONRIDE;
        printf("skier %i on stop %i in skibus %i\n", skiers[i].id, stop, skiers[i].state);
      }
    }
    sem_post(skier_wait);
  }

  sem_wait(skier_wait);
  for (int i = 0; i < config.skiers; i++)
  {
    if (skiers[i].state == ONRIDE)
    {
      skiers[i].state = IN_FINISH;
      printf("skier %i in finish\n", skiers[i].id);
    }
  }

  for (int i = 0; i < config.skiers; i++)
  {
    if (skiers[i].state != IN_FINISH)
    {
      run_skibus(skiers, config);
      break;
    }
  }
  sem_post(skier_wait);
}

void run_skier(int skier_id, skier_t *skiers, config_t config)
{
  usleep(random_number(0, config.maxComeTime));

  sem_wait(skier_wait);

  skiers[skier_id].state = WAITING;
  printf("skier %i on stop %i\n", skiers[skier_id].id, skiers[skier_id].stop_id);

  sem_post(skier_wait);
}