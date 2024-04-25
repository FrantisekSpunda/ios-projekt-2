/**
 * @file main.c
 * @author Your name
 * @date 2024-25-04
 * @brief Description of your project
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "main.h"
#include "helpers.h"
#include <asm-generic/fcntl.h>

sem_t *storage_access;

int main(int argc, char **argv)
{
  /****** file inicialization ******/
  FILE *file = fopen("proj2.out", "w");
  if (file == NULL)
  {
    printf("Soubor se nepodařilo otevřít.\n");
    return 1;
  }
  /****** file inicialization ******/

  storage_access = sem_open("/fjesiofjesio", O_CREAT | O_EXCL, 0644, 1);
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
  key_t key1;
  key_t key2;
  if (-1 != open("/tmp/foo", O_CREAT, 0777))
  {
    key1 = ftok("/tmp/foo", 0);
  }
  else
  {
    perror("open");
    exit(1);
  }
  if (-1 != open("/tmp/foo2", O_CREAT, 0777))
  {
    key2 = ftok("/tmp/foo2", 0);
  }
  else
  {
    perror("open");
    exit(1);
  }

  int skiers_m = shmget(key1, sizeof(skier_t) * config.skiers, IPC_CREAT | 0666);
  if (skiers_m == -1)
  {
    fprintf(stderr, "Error \"shmget\" for skiers_m\n");
    shmctl(skiers_m, IPC_RMID, NULL);
    exit(1);
  }

  skier_t *skiers = (skier_t *)shmat(skiers_m, NULL, 0);
  if (skiers == (skier_t *)-1)
  {
    fprintf(stderr, "Error \"shmat\" for skiers\n");
    shmctl(skiers_m, IPC_RMID, NULL);
    exit(1);
  }

  int storage_m = shmget(key2, sizeof(storage_t), IPC_CREAT | 0666);
  if (storage_m == -1)
  {
    fprintf(stderr, "Error \"shmget\" for storage_m\n");
    shmctl(storage_m, IPC_RMID, NULL);
    shmctl(skiers_m, IPC_RMID, NULL);
    exit(1);
  }

  storage_t *storage = (storage_t *)shmat(storage_m, NULL, 0);
  if (storage == (storage_t *)-1)
  {
    fprintf(stderr, "Error \"shmat\" for storage\n");
    shmctl(storage_m, IPC_RMID, NULL);
    shmctl(skiers_m, IPC_RMID, NULL);
    exit(1);
  }
  /****** shared memory ******/

  if (process_id == 0) // only in main process
  {
    sem_wait(storage_access);
    storage->counter = 1;

    for (int i = 0; i < config.skiers; i++)
    {
      skiers[i].id = i + 1;
      skiers[i].state = BREAKFAST;
      skiers[i].stop_id = random_number(0, config.stops - 1);
    }
    sem_post(storage_access);
  }
  if (process_id == 1)
  {
    sem_wait(storage_access);
    printf("%i: BUS: started\n", storage->counter);
    storage->counter++;
    sem_post(storage_access);

    run_skibus(skiers, storage, config);
  }

  if (process_id > 1) // only not in main process
  {
    run_skier(process_id - 2, skiers, storage, config);
  }

  /************ cleaing *************/
  if (child_pid == 0)
    exit(0);

  for (int i = 0; i < config.skiers + 1; i++)
  { // Uprav podle počtu procesů, které jsi vytvořil
    wait(NULL);
  }

  sem_close(storage_access);
  sem_unlink("/fjesiofjesio");

  shmdt(storage);
  shmdt(skiers);
  shmctl(storage_m, IPC_RMID, NULL);
  shmctl(skiers_m, IPC_RMID, NULL);

  return 0;
}

void run_skibus(skier_t *skiers, storage_t *storage, config_t config)
{
  sem_wait(storage_access);
  storage->skibus_filled = 0;
  sem_post(storage_access);

  for (int stop = 0; stop < config.stops; stop++)
  {
    usleep(random_number(999, 1000));

    sem_wait(storage_access);
    printf("%i: BUS: arrived to %i\n", storage->counter, stop);
    storage->counter++;
    sem_post(storage_access);

    for (int i = 0; i < config.skiers; i++)
    {
      sem_wait(storage_access);
      // printf("---skier id %i\n", skiers[i].id);
      if (skiers[i].state == WAITING && skiers[i].stop_id == stop && storage->skibus_filled < config.skibusCapacity)
      {
        skiers[i].state = ONRIDE;
        printf("%i: L %i: boarding\n", storage->counter, skiers[i].id);
        storage->counter++;
        storage->skibus_filled++;
      }
      sem_post(storage_access);
    }

    sem_wait(storage_access);
    printf("%i: BUS: leaving %i\n", storage->counter, stop);
    storage->counter++;
    sem_post(storage_access);
  }

  sem_wait(storage_access);

  printf("%i: BUS: arrived to final\n", storage->counter);
  storage->counter++;

  for (int i = 0; i < config.skiers; i++)
  {
    if (skiers[i].state == ONRIDE)
    {
      skiers[i].state = IN_FINISH;
      printf("%i: L %i: going to ski\n", storage->counter, skiers[i].id);
      storage->counter++;
    }
  }

  printf("%i: BUS: leaving final\n", storage->counter);
  storage->counter++;

  int run_again = 0;

  for (int i = 0; i < config.skiers; i++)
  {
    if (skiers[i].state != IN_FINISH)
    {
      run_again = 1;
      break;
    }
  }
  sem_post(storage_access);

  if (run_again)
    run_skibus(skiers, storage, config);
  else
  {
    sem_wait(storage_access);
    printf("%i: BUS: finish\n", storage->counter);
    storage->counter++;
    sem_post(storage_access);
  }
}

void run_skier(int skier_id, skier_t *skiers, storage_t *storage, config_t config)
{
  sem_wait(storage_access);
  printf("%i: L %i: started\n", storage->counter, skiers[skier_id].id);
  storage->counter++;
  sem_post(storage_access);
  // usleep(1000);
  // usleep(random_number(0, config.maxComeTime));
  usleep(random_number(0, 1000));

  // printf("run skier %i\n", skier_id);
  sem_wait(storage_access);
  skiers[skier_id].state = WAITING;
  printf("%i: L %i: arrived to %i\n", storage->counter, skiers[skier_id].id, skiers[skier_id].stop_id);
  storage->counter++;
  sem_post(storage_access);
  // exit(0);
}