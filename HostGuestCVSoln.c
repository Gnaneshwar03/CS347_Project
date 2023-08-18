#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
//#include <wait.h>
#include <pthread.h>

int total_guests;
int guests_entered;
int present_guest_count;
int isDoorOpen;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_host = PTHREAD_COND_INITIALIZER;
pthread_cond_t cv_guest = PTHREAD_COND_INITIALIZER;

void openDoor(){
  pthread_mutex_lock(&m2);
    if(isDoorOpen==0){
      isDoorOpen = 1;
      printf("DOOR HAS BEEN OPENED\n");
    }
  pthread_mutex_unlock(&m2);
}

void enterHouse(int thread_id){
  pthread_mutex_lock(&m2);
    if(isDoorOpen==0){
      // this part should never occur: error 
      printf("ERROR: THE GUEST #%d HAS TRIED TO ENTER, DOOR IS NOT OPEN\n",thread_id);
      exit(1);
    }
    else{
      printf("THE GUEST #%d HAS ENTERED THE ROOM, DOOR IS OPEN\n",thread_id);
    }
  pthread_mutex_unlock(&m2);
}

void guestArrived(int thread_id){
  pthread_mutex_lock(&m2);
    if(isDoorOpen==0){
      printf("THE GUEST #%d HAS ARRIVED, DOOR IS NOT OPEN\n",thread_id);
    }
    else{
      // this part should never occur: error N guests only
      printf("ERROR: THE GUEST #%d HAS ARRIVED, DOOR IS OPEN\n",thread_id);
      exit(1);
    }
  pthread_mutex_unlock(&m2);
}

// host routine
void *host_req(void *data)
{
  while(1)
    {
        if(guests_entered>=total_guests) break;
        pthread_mutex_lock(&m);
        while(present_guest_count<total_guests){
          if(guests_entered>=total_guests) break;
          pthread_cond_wait(&cv_host, &m);
        }
        openDoor();
        pthread_cond_signal(&cv_guest);
        pthread_mutex_unlock(&m);
    }
  return 0;
}

//write function to be run by guest threads
void *guest_req(void *data){
    int thread_id = *((int *)data);
    {
      pthread_mutex_lock(&m);
      guestArrived(thread_id);
      present_guest_count++;
      if(present_guest_count==total_guests){
        pthread_cond_signal(&cv_host);
      }
      pthread_cond_wait(&cv_guest,&m);
      guests_entered++;
      pthread_cond_signal(&cv_guest); // comment for incorrectness
      pthread_mutex_unlock(&m);
      enterHouse(thread_id);
    }
}

int main(int argc, char *argv[])
{
  int *guest_thread_id;
  pthread_t *guest_thread, host_thread;

  present_guest_count = 0, guests_entered=0;
  isDoorOpen = 0;
  int i;
  
   if (argc < 2) {
    printf("./host_guest #total_guests eg:./host-guest 10\n");
    exit(1);
  }
  else {
    total_guests = atoi(argv[1]);
  }

   //create host
   pthread_create(&host_thread, NULL, host_req, NULL);

   //create guest threads
   guest_thread_id = (int *)malloc(sizeof(int) * total_guests);
   guest_thread = (pthread_t *)malloc(sizeof(pthread_t) * total_guests);
  
  for (i = 0; i < total_guests; i++)
    guest_thread_id[i] = i;

  for (i = 0; i < total_guests; i++)
    pthread_create(&guest_thread[i], NULL, guest_req, (void *)&guest_thread_id[i]);
  //wait for all threads to complete

  pthread_join(host_thread,NULL);

  for (i=0; i < total_guests; i++){
      pthread_join(guest_thread[i],NULL);
  }
  /*----Deallocating Buffers---------------------*/
  free(guest_thread_id);
  free(guest_thread);
  return 0;
}
