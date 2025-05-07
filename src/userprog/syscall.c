#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  printf ("system call!\n"); 
 // Get the system call number from the stack
 int syscall_number = *(int *)f->esp;
  
 switch (syscall_number) {
   case SYS_HALT:
     // Handle halt system call
     break;
     
   case SYS_EXIT:
     // Handle exit system call
     break;
     
   case SYS_EXEC:
     // Handle exec system call
     break;
     
   case SYS_WAIT:
     // Handle wait system call
     break;
     
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
