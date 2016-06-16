#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static pthread_t th;
static pthread_cond_t cond;
static pthread_mutex_t mutex;

void *fun(void *arg) {

  puts("sleep for 1 second");
  // to wake up
  pthread_mutex_lock(&mutex);
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
  
  return NULL;
}

int main(void) {

  pthread_cond_init(&cond, NULL);
  pthread_mutex_init(&mutex, NULL);

  pthread_create(&th, NULL, fun, NULL);

  // make signal call first
  sleep(1);
  pthread_mutex_lock(&mutex);
  puts("wait to be waken up");
  pthread_cond_wait(&cond, &mutex);
  puts("I'm waken up");
  pthread_mutex_unlock(&mutex);

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
  
  pthread_join(th, NULL);
  
  return 0;
}
