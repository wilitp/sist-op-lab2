#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void pingpong(int n, int is_ping, int my_sem, int other_sem) {

  while(0 < n) {
    // Esperamos a que se pueda hacer algo
    sem_down(my_sem);
    // Printeamos lo que corresponda
    if(is_ping) {
      printf("Thread %d: Ping\n", is_ping);
    } else {
      printf("Thread %d: Pong\n\n", is_ping);
    }
    --n;
    sem_up(other_sem);
  }
}


int main(int argc, char **argv) {


  if(argc < 2) {
    fprintf(2, "La concha de tu madre\n");
    exit(1);
  }
  int n = atoi(argv[1]);

  int sem_ping = sem_open(0, 1);
  int sem_pong = sem_open(1, 0);

  if(fork() == 0) {
    // Va a printear n pongs
    pingpong(n, 0, sem_pong, sem_ping);
  } else {
    // Va a printear n pings
    pingpong(n, 1, sem_ping, sem_pong);
    wait(0);
  }

  sem_close(sem_ping);
  sem_close(sem_pong);

  return 0;
}

