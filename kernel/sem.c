#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"

#define MAX_SEMS 4

int sems[MAX_SEMS];    // Cantidad de "asientos" disponibles para cada semaforo
struct spinlock lk[1]; // Lock para sincronizar accesos a los semaforos
int lkinit[1] = {0};   // Esta inicializado el spinlock?

// Inicializar el módulo
void sem_init(void) {
  initlock(lk, "sems_lock");
  *lkinit = 1;
  for (unsigned int i = 0; i < MAX_SEMS; i++) {
    sems[i] = 0;
  }
}

/*
 * Abre y/o inicializa el semáforo “sem” con un valor
 *arbitrario “value”
 */
int sem_open(int sem, int value) {
  acquire(lk);
  sems[sem] = value;
  release(lk);
  return sem;
}

/*
 * Libera el semáforo “sem”
 */

int sem_close(int sem) { return sem; }

/*
 * Incrementa el semáforo ”sem” desbloqueando los procesos
 * cuando su valor es 0.
 */
int sem_up(int sem) {
  acquire(lk);
  // Si estaban bloqueados(el sem es 0) despertamos a todos los procesos
  // esperando a ese sem
  if (sems[sem] == 0) {
    wakeup(&sems[sem]);
  }
  ++sems[sem];
  release(lk);
  return sem;
}

/*
 * Decrementa el semáforo ”sem” bloqueando los procesos
 * cuando su valor es 0.
 */
int sem_down(int sem) {
  acquire(lk);
  // Mientras el sem sea 0 entonces dormimos al proceso
  // Usamos un while en vez de un if pq al despertar a los procesos
  // hay que volver a ver si alguien mas no se quedo con el ultimo lugar(hizo
  // que el sem sea 0)
  while (sems[sem] == 0) {
    // Detalle 1: el segundo argumento de sleep es un lock a releasear,
    // esto es para que el proceso dormido no se quede con el lock y nadie
    // mas pueda tocar lo que sea que estamos protegiendo con el lock.
    // Detalle 2: No es necesario hacer acquire otra vez, ya que sleep lo hace
    // cuando despiertan al proceso, es decir, que despertar es parte de
    // dormir(?
    sleep(&sems[sem], lk);
  }
  --sems[sem];
  release(lk);
  return sem;
}
