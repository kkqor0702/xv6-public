#include "types.h"
#include "stat.h"
#include "user.h"

int main(void){
  int pid;
  
  for (int pr = 1; pr <= 6; pr++){
    pid = fork();
    if(pid == 0){
      set_proc_priority(getpid(), pr);
      for (;;){
        printf(1, "pID: %d, with priority %d\n", getpid(), pr);
        for (volatile int i = 0; i < 100000; i++);
      }
    }
  }
  
  for (int i = 0; i < 6; i++){
    wait();
  }
  
  exit();
}
