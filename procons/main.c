#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static int data = 0;
static int is_avail = 0;
static pthread_mutex_t mutex;
static pthread_cond_t cond;

void *producer(void *arg) {
  for(int i = 0; i < 10; ++i) {
    pthread_mutex_lock(&mutex);
  
    do {
      fputs("Input number:", stdout);
    }while(scanf("%d", &data) != 1);
    is_avail = 1;
    pthread_cond_signal(&cond);
    pthread_cond_wait(&cond, &mutex);
  
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

int main(void) { // consumer
  pthread_t th;
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);
  
  pthread_create(&th, NULL, producer, NULL); // assume success

  sleep(1);
  // consumer logic
  for(int i = 0; i < 10; ++i) {
    pthread_mutex_lock(&mutex);
    // wait for data
    if(!is_avail) {
      puts("waiting for data");
      pthread_cond_wait(&cond, &mutex);
    }else{
      puts("data already present, no need to wait.");
    }
    // data ready
    printf("Data consumed: %d\n", data);
    is_avail = 0;
    pthread_cond_signal(&cond);
    
    pthread_mutex_unlock(&mutex);
  }
  
  pthread_join(th, NULL); // assume success
  return 0;
}
