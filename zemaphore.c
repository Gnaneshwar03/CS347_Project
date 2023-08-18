#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include "zemaphore.h"

void zem_init(zem_t *s, int value) {
  s->counter = value;
  //printf("present value init: %d\n",s->counter);
  pthread_cond_init(&s->c,NULL);
  pthread_mutex_init(&s->m,NULL);
}

void zem_down(zem_t *s) {
  pthread_mutex_lock(&s->m);
    s->counter--;
    if(s->counter < 0){
        pthread_cond_wait(&s->c,&s->m);
    }
    //printf("present value down: %d\n",s->counter);
  pthread_mutex_unlock(&s->m);
}

void zem_up(zem_t *s) {
  pthread_mutex_lock(&s->m);
    s->counter++;
    if(s->counter<=0)
      pthread_cond_signal(&s->c);
    //printf("present value up: %d\n",s->counter);
  pthread_mutex_unlock(&s->m);
}
