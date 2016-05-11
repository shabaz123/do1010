/********************************************************
 * do1010
 *
 * Rev 1.0 May 2016 shabaz
 *
 * A program designed to run at 1.2GHz on a Pi 3
 * Doesn't really work as well at slower clock speeds
 * All it is supposed to do is display 101010...
 *
 * To compile:  gcc do1010.c -lpthread -o do1010
 * To run: ./do1010
 ********************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <errno.h>

#define PERIOD 2E7

int var1;
int var2; // the thing that causes issues..
int mysync; // used for synchronisation
 
 
void* 
perform_work(void* argument)
{
  int num;
  unsigned long iter;
  int* badloc;
  int ret;
  struct timespec a;
  unsigned int msec=999;

  badloc=0;
 
  num = *((int *) argument);

  if (num==0)
  {
    while(1)
    {
      while(mysync==0);
      mysync=0;
      for (iter=0; iter<PERIOD; iter++)
      {
        // do some useless work, prevent compiler optimising it out
        var1++;
      }
      var2++;
      for (iter=0; iter<PERIOD; iter++)
      {
        // do some useless work, prevent compiler optimising it out
        var1++;
      }
      var2--;
    } // end while(1)
  }
  else
  {
    while(1)
    {
      mysync=1;
      a.tv_nsec=((long)(msec))*1E6d;
      a.tv_sec=0;
      if ((ret = nanosleep(&a, NULL)) != 0)
      {
        fprintf(stderr, "delay_ms error: %s\n", strerror(errno));
      }
      var2++;
      if (var2==2)
      {
        var2=0;
      }
      
      printf("%d", var2);
      fflush(stdout);
      switch (var2)
      {
        case 0:
          break;
        case 1:
          break;
        default: // default never happens, right
        *badloc=1; // we are not allowed to write to this location!!
        break;
     }
    } // end while(1)

  }
  
   // will not return 
   return NULL;
}
 
int
main(void)
{
   pthread_t threads[2];
   int thread_args[2];
   int result_code, index;

   var1=0;
   var2=0;
   mysync=0;
 
   for (index = 0; index < 2; index++) {
      thread_args[index] = index;
      result_code = pthread_create(&threads[index], NULL, perform_work, (void *) &thread_args[index]);
      assert(0 == result_code);
   }
 
   // wait for each thread to complete, will not happen
   for (index = 0; index < 2; index++) {
      // block until thread 'index' completes
      result_code = pthread_join(threads[index], NULL);
      printf("In main: thread %d has completed\n", index);
      assert(0 == result_code);
   }
 
   return(0);
}

