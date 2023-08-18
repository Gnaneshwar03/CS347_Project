#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "zemaphore.h"

#define TOTAL_GUESTS 10

zem_t zem_host,zem_guest;

void openDoor(){
    printf("THE DOOR HAS BEEN OPENED\n");
}

void enterHouse(int thread_id){
    printf("THE GUEST #%d HAS ENTERED\n",thread_id);
}

void guestArrived(int thread_id){
    printf("THE GUEST #%d HAS ARRIVED\n",thread_id);
}

// host routine
void *host_req(void *data)
{
  for(int i=0;i<TOTAL_GUESTS;i++){
      zem_down(&zem_host);
  }
  openDoor();
  zem_up(&zem_guest);
  return 0;
}

//write function to be run by guest threads
void *guest_req(void *data){
    int thread_id = *((int *)data);
    {
       guestArrived(thread_id);
       zem_up(&zem_host);
       zem_down(&zem_guest);
       enterHouse(thread_id);
       zem_up(&zem_guest);
    }
  return 0;
}

int main(int argc, char *argv[])
{
  int guest_thread_id[TOTAL_GUESTS];
  pthread_t guest_thread[TOTAL_GUESTS], host_thread;
  int i;
  
  zem_init(&zem_host,0);
  zem_init(&zem_guest,0);
   //create host
   pthread_create(&host_thread, NULL, host_req, NULL);

   //create guest threads
  
  for (i = 0; i < TOTAL_GUESTS; i++)
    guest_thread_id[i] = i;

  for (i = 0; i < TOTAL_GUESTS; i++)
    pthread_create(&guest_thread[i], NULL, guest_req, (void *)&guest_thread_id[i]);
  //wait for all threads to complete

  pthread_join(host_thread,NULL);

  for (i=0; i < TOTAL_GUESTS; i++){
      pthread_join(guest_thread[i],NULL);
  }
  return 0;
}
