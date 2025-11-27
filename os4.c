// os4.c — 가장 간단한 Copy-on-Write 테스트
#include "types.h"
#include "stat.h"
#include "user.h"

int main(void)
{
  printf(1, "=== COW TEST ===\n");

  // 하나의 페이지를 확보
  char *p = sbrk(4096);
  p[0] = 'A';

  int pid = fork();
  if(pid < 0){
    printf(1, "fork failed\n");
    exit();
  }

  if(pid == 0){
    // ★ child가 값을 바꾸면 COW가 일어나야 함
    p[0] = 'B';
    printf(1, "child value = %c\n", p[0]);
    exit();
  }

  wait(); // child 종료 기다림
  printf(1, "parent value = %c\n", p[0]);

  printf(1, "=== TEST END ===\n");
  exit();
}