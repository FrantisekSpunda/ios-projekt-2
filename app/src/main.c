/**
 * @file main.c
 * @author František Špunda
 * @date 2024-28-04
 * @brief IOS projekt 2 - VUT FIT
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "main.h"

sem_t *storage_access;
FILE *file;

int main(int argc, char **argv)
{
  config_t config = config_setup(argc, argv);

  /****** file inicialization ******/
  file = fopen("proj2.out", "w");
  if (file == NULL)
  {
    printf("Soubor se nepodařilo otevřít.\n");
    return 1;
  }
  /****** file inicialization ******/

  /******* semaphore inicialization ***************/
  storage_access = sem_open("/semaphore", O_CREAT | O_EXCL, 0644, 1);
  if (storage_access == SEM_FAILED)
  {
    fprintf(stderr, "Error sem_open for storage_access\n");
    semaphore_destroy();
    fclose(file);
    exit(1);
  }
  /******* semaphore inicialization ***************/

  /****** processes inicialization ******/
  pid_t child_pid;
  int process_id = 0;

  for (int i = 0; i < config.skiers + 1; i++)
  {
    child_pid = fork();
    srand(time(NULL) ^ getpid());

    if (child_pid == 0)
      break;
    else if (child_pid < 0)
    {
      fprintf(stderr, "Error \"fork\" with numb. \"%i\"\n", child_pid);
      semaphore_destroy();
      fclose(file);
      exit(1);
    }
    else
    {
      process_id++;
    }
  }
  /****** processes inicialization ******/

  /****** shared memory ******/
  key_t key1;
  key_t key2;
  skier_t *skiers;
  storage_t *storage;

  if (-1 != open("/tmp/foo", O_CREAT, 0777))
    key1 = ftok("/tmp/foo", 0);
  else
  {
    fprintf(stderr, "Error open key 1\n");
    semaphore_destroy();
    fclose(file);
    exit(1);
  }
  if (-1 != open("/tmp/foo2", O_CREAT, 0777))
    key2 = ftok("/tmp/foo2", 0);
  else
  {
    fprintf(stderr, "Error open key 2\n");
    semaphore_destroy();
    fclose(file);
    exit(1);
  }

  int skiers_m = shmget(key1, sizeof(skier_t) * config.skiers, IPC_CREAT | 0666);
  if (skiers_m == -1)
  {
    fprintf(stderr, "Error \"shmget\" for skiers_m\n");
    semaphore_destroy();
    fclose(file);
    shm_skiers_destroy(skiers_m, NULL);
    exit(1);
  }

  skiers = (skier_t *)shmat(skiers_m, NULL, 0);
  if (skiers == (skier_t *)-1)
  {
    fprintf(stderr, "Error \"shmat\" for skiers\n");
    semaphore_destroy();
    fclose(file);
    shm_skiers_destroy(skiers_m, skiers);
    exit(1);
  }

  int storage_m = shmget(key2, sizeof(storage_t), IPC_CREAT | 0666);
  if (storage_m == -1)
  {
    fprintf(stderr, "Error \"shmget\" for storage_m\n");
    semaphore_destroy();
    fclose(file);
    shm_storage_destroy(storage_m, NULL);
    shm_skiers_destroy(skiers_m, skiers);
    exit(1);
  }

  storage = (storage_t *)shmat(storage_m, NULL, 0);
  if (storage == (storage_t *)-1)
  {
    fprintf(stderr, "Error \"shmat\" for storage\n");
    semaphore_destroy();
    fclose(file);
    shm_storage_destroy(storage_m, storage);
    shm_skiers_destroy(skiers_m, skiers);
    exit(1);
  }
  /****** shared memory ******/

  /****** fill shared memory, run function for processes ******/
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
    print_out(M_SKIBUS_START, storage, skiers, 0, 1);
    run_skibus(skiers, storage, config);
  }

  if (process_id > 1) // only not in main process
  {
    run_skier(process_id - 2, skiers, storage, config);
  }
  /****** fill shared memory, run function for processes ******/

  /************ cleaing *************/
  if (child_pid == 0)
  {
    fclose(file);
    sem_close(storage_access);
    shmdt(storage);
    shmdt(skiers);

    exit(0);
  }

  // wait for child processes to end
  for (int i = 0; i < config.skiers + 1; i++)
  {
    wait(NULL);
  }

  fclose(file);
  semaphore_destroy();
  shm_storage_destroy(storage_m, storage);
  shm_skiers_destroy(skiers_m, skiers);
  /************ cleaing *************/

  return 0;
}

void run_skibus(skier_t *skiers, storage_t *storage, config_t config)
{
  // empty the bus on start
  sem_wait(storage_access);
  storage->skibus_filled = 0;
  sem_post(storage_access);

  // drive to all stops
  for (int stop = 0; stop < config.stops; stop++)
  {
    // time between stops
    usleep(random_number(0, config.maxRideTime));
    print_out(M_SKIBUS_ARRIVED, storage, skiers, stop, 1);

    // skiers board the bus
    for (int i = 0; i < config.skiers; i++)
    {
      sem_wait(storage_access);

      // condition for skier to board the bus
      if (skiers[i].state == WAITING && skiers[i].stop_id == stop && storage->skibus_filled < config.skibusCapacity)
      {
        skiers[i].state = ONRIDE;
        print_out(M_SKIER_BOARDING, storage, skiers, i, 0);
        storage->skibus_filled++;
      }
      sem_post(storage_access);
    }

    print_out(M_SKIBUS_LEAVING, storage, skiers, stop, 1);
  }

  sem_wait(storage_access);

  print_out(M_SKIBUS_ARRIVED_TO_FINAL, storage, skiers, 0, 0);

  // skiers get off the buss
  for (int i = 0; i < config.skiers; i++)
  {
    if (skiers[i].state == ONRIDE)
    {
      skiers[i].state = IN_FINISH;
      print_out(M_SKIER_SKIING, storage, skiers, i, 0);
    }
  }

  print_out(M_SKIBUS_LEAVING_FINAL, storage, skiers, 0, 0);

  int run_again = 0;

  // skibus will go another round if there aren't some skiers in finish
  for (int i = 0; i < config.skiers; i++)
    if (skiers[i].state != IN_FINISH)
    {
      run_again = 1;
      break;
    }
  sem_post(storage_access);

  if (run_again)
    run_skibus(skiers, storage, config);
  else
    print_out(M_SKIBUS_FINISH, storage, skiers, 0, 1);
}

void run_skier(int skier_id, skier_t *skiers, storage_t *storage, config_t config)
{
  print_out(M_SKIER_START, storage, skiers, skier_id, 1);
  // time that skier arrive to stop
  usleep(random_number(0, config.maxComeTime));

  sem_wait(storage_access);
  // ariving skier to stop
  skiers[skier_id].state = WAITING;
  print_out(M_SKIER_ARRIVED, storage, skiers, skier_id, 0);
  sem_post(storage_access);
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

  if (config.skiers < 0 || config.skiers > 20000)
  {
    fprintf(stderr, "Number of skiers must be between 0 and 20000\n");
    exit(1);
  }

  if (config.stops < 1 || config.stops > 10)
  {
    fprintf(stderr, "Number of stops must be between 1 and 10\n");
    exit(1);
  }

  if (config.skibusCapacity < 10 || config.stops > 100)
  {
    fprintf(stderr, "Capacity of skibus must be between 10 and 100\n");
    exit(1);
  }

  if (config.maxComeTime < 0 || config.maxComeTime > 10000)
  {
    fprintf(stderr, "Max time, that skier is having breakfast, must be between 0 and 10000\n");
    exit(1);
  }

  if (config.maxRideTime < 0 || config.maxRideTime > 1000)
  {
    fprintf(stderr, "Max time between two stops must be between 0 and 1000\n");
    exit(1);
  }

  return config;
}

int random_number(int min, int max)
{
  return rand() % ((max + 1) - min) + min;
}

void print_out(message_type_t message_type, storage_t *storage, skier_t *skiers, int i, int shm_check)
{
  if (shm_check)
    sem_wait(storage_access);

  switch (message_type)
  {
  case M_SKIER_START:
    printf("%i: L %i: started\n", storage->counter, skiers[i].id);
    fprintf(file, "%i: L %i: started\n", storage->counter, skiers[i].id);
    break;

  case M_SKIER_ARRIVED:
    printf("%i: L %i: arrived to %i\n", storage->counter, skiers[i].id, skiers[i].stop_id + 1);
    fprintf(file, "%i: L %i: arrived to %i\n", storage->counter, skiers[i].id, skiers[i].stop_id + 1);
    break;
  case M_SKIER_BOARDING:
    printf("%i: L %i: boarding\n", storage->counter, skiers[i].id);
    fprintf(file, "%i: L %i: boarding\n", storage->counter, skiers[i].id);
    break;
  case M_SKIER_SKIING:
    printf("%i: L %i: going to ski\n", storage->counter, skiers[i].id);
    fprintf(file, "%i: L %i: going to ski\n", storage->counter, skiers[i].id);
    break;
  case M_SKIBUS_START:
    printf("%i: BUS: started\n", storage->counter);
    fprintf(file, "%i: BUS: started\n", storage->counter);
    break;
  case M_SKIBUS_ARRIVED:
    printf("%i: BUS: arrived to %i\n", storage->counter, i + 1);
    fprintf(file, "%i: BUS: arrived to %i\n", storage->counter, i + 1);
    break;
  case M_SKIBUS_LEAVING:
    printf("%i: BUS: leaving %i\n", storage->counter, i + 1);
    fprintf(file, "%i: BUS: leaving %i\n", storage->counter, i + 1);
    break;
  case M_SKIBUS_ARRIVED_TO_FINAL:
    printf("%i: BUS: arrived to final\n", storage->counter);
    fprintf(file, "%i: BUS: arrived to final\n", storage->counter);
    break;
  case M_SKIBUS_LEAVING_FINAL:
    printf("%i: BUS: leaving final\n", storage->counter);
    fprintf(file, "%i: BUS: leaving final\n", storage->counter);
    break;
  case M_SKIBUS_FINISH:
    printf("%i: BUS: finish\n", storage->counter);
    fprintf(file, "%i: BUS: finish\n", storage->counter);
    break;
  }

  fflush(file);
  storage->counter++;

  if (shm_check)
    sem_post(storage_access);
}

void semaphore_destroy()
{
  sem_close(storage_access);
  sem_unlink("/semaphore");
}

void shm_skiers_destroy(int skiers_m, skier_t *skiers)
{
  if (skiers != NULL)
    shmdt(skiers);
  shmctl(skiers_m, IPC_RMID, NULL);
}

void shm_storage_destroy(int storage_m, storage_t *storage)
{
  if (storage != NULL)
    shmdt(storage);
  shmctl(storage_m, IPC_RMID, NULL);
}