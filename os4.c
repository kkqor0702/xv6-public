#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
    int *x = malloc(sizeof(int));  // 동적 메모리 할당
    *x = 1;

    printf(1, "Before fork: x=%d, address : %p\n", *x, x);

    int pid = fork();
    
    // child process
    if (pid == 0) {
        printf(1, "Before Child : x=%d address : %p\n", *x, x);
        *x = 2;  // 쓰기: CoW 발생
        printf(1, "CoW !");
        printf(1, "After Child : x=%d address : %p\n", *x, x);
        exit();
    }

    // parent process
    wait();  // 자식 종료 대기
    printf(1, "Parent : x=%d address : %p\n", *x, x);

    free(x);
    exit();
}

