#ifndef _helpers_H_
#define _helpers_H_

#include "main.h"

void shm_free(pid_t child_pid, config_t config, int shmid);
config_t config_setup(int argc, char **argv);
int random_number(int min, int max);

#endif
