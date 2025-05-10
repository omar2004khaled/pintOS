#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/process.h"
#include "threads/vaddr.h"
static void syscall_handler (struct intr_frame *);

/* prototypes */
static void syscall_halt(void);
static int syscall_wait(tid_t tid);
/*

static void syscall_exit(int status);
//and the rest of the system calls .....

*/
void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}
static void syscall_halt(void) 
{
  shutdown_power_off();
}
static int syscall_wait(tid_t tid) 
{
  return process_wait(tid);
}
static void check_address(void *addr)
{
  if (addr == NULL || !is_user_vaddr(addr))              //found in threads/vaddr.h
  { 
    thread_exit();   //terminate if invalid
  }
}
static void syscall_exit(int status){
  struct thread *cur = thread_current();
  if(cur->parent !=NULL)
    cur->parent->child_exit_status = status; 
  process_exit();
}

static void
syscall_handler (struct intr_frame *f) 
{
  //printf ("system call!\n"); 
 // Get the system call number from the stack
 
 int syscall_number = *(int *)f->esp;
 check_address(f->esp);

 switch (syscall_number) {
   case SYS_HALT:
     syscall_halt();
     break;
     
   case SYS_EXIT:{
    int status = *((int *)f->esp + 1);
    f->eax = status; 
    syscall_exit(status);
     break;
   } 
   case SYS_EXEC:
   {
    char* filename =(char*) (*((int *)f->esp + 1));
    f->eax = process_execute(filename);
      break;
   } 
     case SYS_WAIT: {
      tid_t tid = *(tid_t *)(f->esp + 4);
      f->eax = process_wait(tid);
      break;
    }
    
     
   case SYS_CREATE:
     // Handle create system call
     break;
     
   case SYS_REMOVE:
     // Handle remove system call
     break;
     
   case SYS_OPEN:
     // Handle open system call
     break;
     
   case SYS_FILESIZE:
     // Handle filesize system call
     break;
     
   case SYS_READ:
     // Handle read system call
     break;
     
   case SYS_WRITE:
     // Handle write system call
     break;
     
   case SYS_SEEK:
     // Handle seek system call
     break;
     
   case SYS_TELL:
     // Handle tell system call
     break;
     
   case SYS_CLOSE:
     // Handle close system call
     break;
     
   default:
     printf("Unknown system call: %d\n", syscall_number);
     thread_exit ();
     break;
 }
 
 thread_exit ();
}
