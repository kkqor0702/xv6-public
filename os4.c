#include "types.h"
#include "stat.h"
#include "user.h"

// 전역 변수
int data = 2021;

int main(void) {
    printf(1, "=== CoW Test ===\n");

    printf(1, "free pages : %d\n", getNumFreePages());

    if(fork() == 0){
        // 자식 프로세스
        printf(1, "after fork - free pages: %d\n", getNumFreePages());

        // 읽기만 하면 페이지 복사 안 됨
        int val = data;
        printf(1, "val : %d, just read - free pages: %d\n", val, getNumFreePages());

        // 쓰기 하면 페이지 복사 발생
        data = 1673;
        printf(1, "data : %d, after write - free pages: %d\n", data, getNumFreePages());

        exit();
    }

    wait();
    printf(1, "=== Test End ===\n");
    exit();
}
