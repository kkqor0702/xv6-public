// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

uint num_free_pages; //전역변수 선언
uint pgrefcount[PHYSTOP >> PTXSHIFT]; //전역변수 선언

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);
  num_free_pages = 0; // 0으로 초기화
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  // pgrefcount 배열 0으로 초기화
  for (int i = 0; i < (PHYSTOP >> PTXSHIFT); i++){
    pgrefcount[i] = 0;
  }
  
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    kfree(p);
}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  // 1. 물리 주소 구하기
  uint pa = V2P(v);

  // 2. [핵심 로직] 참조 카운트 관리
  // kinit(부팅) 과정에서는 ref_count가 0일 수 있으므로, 
  // 0보다 클 때만 감소시켜야 안전합니다.
  if(get_refcount(pa) > 0) {
      dec_refcount(pa);
  }

  // 3. [방어 로직] 아직 누군가 쓰고 있다면(참조 > 0), 절대 해제하면 안 됨!
  if(get_refcount(pa) > 0) {
      return; // 함수 종료 (메모리 해제 안 함)
  }

  // --- 아래는 참조 카운트가 0일 때만 실행됨 (진짜 해제) ---

  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  
  // num_free_pages 관리 (1단계 과제 내용)
  num_free_pages++; 

  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;

    pgrefcount[V2P(r) >> PTXSHIFT] = 1;   //refcount 1로 설정
    num_free_pages--; // --
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

//1단계
int
getNumFreePages(void)
{
    acquire(&kmem.lock);
    struct run *r = kmem.freelist;
    int count = 0;

    while(r){
        count++;
        r = r->next;
    }

    release(&kmem.lock);
    return count;
}

//2-2 함수 구현
int
get_refcount(uint pa)
{
  return pgrefcount[pa >> PTXSHIFT];
}

void
inc_refcount(uint pa)
{
  pgrefcount[pa >> PTXSHIFT]++;
}

void
dec_refcount(uint pa)
{
  uint idx = pa >> PTXSHIFT;
  if (pgrefcount[idx] > 0)
    pgrefcount[idx]--;
}
