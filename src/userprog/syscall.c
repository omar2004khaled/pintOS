#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/process.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"   

static void syscall_handler (struct intr_frame *);

/* prototypes */
static void syscall_halt(void);
static int syscall_wait(tid_t tid);
static struct proccess_file *get_pf(int fd);
static int write(int fd, char *buffer, unsigned size);

struct lock file_lock;
/*

static void syscall_exit(int status);
//and the rest of the system calls .....

*/
void
syscall_init (void) 
{
  lock_init(&file_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void check_address(void *addr)
{
  if (addr == NULL || !is_user_vaddr(addr) || addr < (void *)0x08048000)              //found in threads/vaddr.h
  { 
    syscall_exit(-1);
  }
}
static int convert_vaddr(void *vaddr)
{
  check_address(vaddr);
  int* adrr = pagedir_get_page(thread_current()->pagedir, vaddr);
  if (adrr == NULL )
  {
    syscall_exit(-1);
  }

  return adrr;
}
// checks every char has valid address
static void check_string(void* str)
{
  char* s = str;
  while(true){
    if (s == 0) 
      break;
    // check if the address is valid
    s = *(char*)convert_vaddr((void*) str++);

  }
  
}

static void check_buffer(void *buffer, int size)
{
  char* it = (char* ) buffer;
  for (int i = 0; i < size; i++)
  {
    check_address((void*) it);
    it++;
  }
}
static void syscall_halt(void) 
{
  printf("(halt) begin\n");
  shutdown_power_off();
}

static int syscall_wait(tid_t tid) 
{
  return process_wait(tid);
}

static void syscall_exit(int status){
  struct thread *cur = thread_current();
  if (cur->parent != NULL) {
    cur->parent->child_exit_status = status;
  }
  // printf("%s: exit(%d)\n", thread_name(), status);
  thread_exit();
}

bool create(char *file_name, int initial_size) {
  bool success;
  lock_acquire(&file_lock);
  success = filesys_create(file_name, initial_size);
  lock_release(&file_lock);
  return success;
  
}

bool remove(char *file_name) {
  bool success;
  lock_acquire(&file_lock);
  success = filesys_remove(file_name);
  lock_release(&file_lock);
  return success;

}

int open(char *file_name) {
  lock_acquire(&file_lock);
  struct file *file = filesys_open(file_name);
  lock_release(&file_lock);
  int file_fd; // file discriptor (unique id)
  lock_acquire(&file_lock);
  if (file != NULL) {
    file_fd = procces_add_file(file);
  }
  else {
    file_fd = -1;
  }
  lock_release(&file_lock);

  return file_fd;
}

int filesize(int fd) {

  struct process_file *pf = get_pf(fd);
  if (pf == NULL) {
    return -1;
  }  
  
  int file_len = 0;
  lock_acquire(&file_lock);
  file_len= file_length(pf->file);
  lock_release(&file_lock);
  return file_len;
    
}

int procces_add_file(struct file *f){
  struct thread *thread = thread_current();

	struct process_file *process_file = malloc(sizeof(struct process_file)); // new pf to add to the fd table
	if (process_file == NULL) {
		return -1; 
	}

	process_file->fd = thread->fd;
  process_file->file = f;
  thread->fd++; 
	list_push_back(&thread->file_list, &process_file->elem); // add process file (opened file info) to fd table
	
  return process_file->fd;

}

static int write(int fd, char *buffer, unsigned size) {

  if (fd == 1) {
    lock_acquire(&file_lock);
    putbuf(buffer, size);
    lock_release(&file_lock);
    return size;
  } 
  else if (fd == 0) { // this is stdin
   
  }
  else {
    struct process_file* pf = get_pf(fd);
    if (pf == NULL) {
      return -1;
    }
    
    int size_written = 0;
    lock_acquire(&file_lock);
    size_written = file_write(pf->file, buffer, size);
    lock_release(&file_lock);

    return size_written;
  }
}

static int read(int fd, char *buffer, unsigned size) {

  if (fd == 1) {
    
  } 
  else if (fd == 0) { // this is stdin
    int res = size;
    while(size-- > 0){
      
      lock_acquire(&file_lock);
      char in = input_getc();
      lock_release(&file_lock);
      buffer += in; 
    }
    return res;
  }
  else {
    struct process_file* pf = get_pf(fd);
    if (pf == NULL) {
      return -1;
    }
    
    int size_read = 0;
    lock_acquire(&file_lock);
    size_read = file_read(pf->file, buffer, size);
    lock_release(&file_lock);

    return size_read;
  }
}

static void close(int fd){

  // search for file (given fd) in process file list (fd table) and close it
  struct process_file *pf = get_pf(fd);
  if (pf == NULL) {  
    return;
  }

  lock_acquire(&file_lock);
  file_close(pf->file);
  list_remove(&pf->elem);
  lock_release(&file_lock);

  free(pf);

  return;
}

// searches in the file lsit in current thread for the file with the given fd
static struct proccess_file *get_pf(int fd){ //get process file and optionally remove it from the list
  struct thread *curr_thread = thread_current();
  struct list_elem *it = list_begin(&curr_thread->file_list);
  while (it != list_end(&curr_thread->file_list)){
    struct process_file *pf = list_entry(it, struct process_file, elem);
    if (pf->fd == fd){
      return pf;
    }
  }

  return NULL;

}


static void
syscall_handler (struct intr_frame *f) 
{
  //printf ("system call!\n"); 
 // Get the system call number from the stack
 int syscall_number = *(int *) convert_vaddr((void *) f->esp);
//  int syscall_number = *(int *)f->esp;
//  check_address(f->esp);

 // load_arg:
 // int* ptr1 = (int*)f->esp + 1;

 switch (syscall_number) {
   case SYS_HALT:{
     syscall_halt();
     break;
   }

   case SYS_EXIT:{
    int status = *((int *)f->esp + 1);
    f->eax = status; 
    syscall_exit(status);
     break;
   } 
   case SYS_EXEC: {
    char* filename = (char*)(*((int*)f->esp + 1));
    check_string(filename);
    
    // Create a copy of the filename in kernel space
    char *fn_copy = palloc_get_page(0);
    if (fn_copy == NULL) {
        f->eax = -1;
        break;
    }
    strlcpy(fn_copy, filename, PGSIZE);
    
    f->eax = process_execute(fn_copy);
    palloc_free_page(fn_copy);
    break;
}
     case SYS_WAIT: {
      tid_t tid = *(tid_t *)(f->esp + 4);
      f->eax = process_wait(tid);
      break;
    }
    
   // TEST CASES DONE
   case SYS_CREATE:{
    // Handle create system call
    // 2 args const char *file, unsigned initial_size
     char* file = (char* ) *((int *) f->esp + 1);
     int initial_size_ptr = (unsigned) *((int *)f->esp + 2);
    //  printf("File name and size: %s %d\n", file, initial_size_ptr);
     convert_vaddr((void *)file);
    //  convert_vaddr((void *)initial_size_ptr);
    //  check_string(file);
    //  printf("create syscall (passed checks): %s\n", file);
     bool success = create(file, initial_size_ptr);

     f->eax = success;

     break;
   }
  
   // Under testing
   case SYS_REMOVE:{
     // Handle remove system call
     char* file = (char *)(*((int *)f->esp + 1));
     convert_vaddr((void *)file);

     f->eax = remove(file);

     break;
   }

   // TEST CASES DONE
   case SYS_OPEN:{
     // Handle open system call
     //////// IMPORTANT /////// -> When a thread/process exits, all files it opened must be closed, the list file_list must be freed should help
     char *file = (char *)(*((int *)f->esp + 1));
    //  check_string((void*) file);
     convert_vaddr((void *)file);


     int fd = open(file);

     f->eax = fd;
     
     break;
   }

   // Under testing
   case SYS_FILESIZE:{
     // Handle filesize system call
     int* fd_ptr = (int *)f->esp + 1;
     convert_vaddr((void *) fd_ptr);

     f->eax = filesize(*fd_ptr);
     if (f->eax == -1) {
       syscall_exit(-1);
     }

     break;
  }
   case SYS_READ:{
     // Handle read system call
     // 3 args
    int fd = *((int *)f->esp + 1);
    char *buffer = (char *)(*((int *)f->esp + 2));
    unsigned size = *((unsigned *)f->esp + 3);
    convert_vaddr((void *)buffer); 

    if(fd ==1)
     syscall_exit(-1);

    f->eax = read(fd, buffer, size);  
     break;
   }

   case SYS_WRITE:{
    int fd = *((int *)f->esp + 1);
    char *buffer = (char *)(*((int *)f->esp + 2));
    unsigned size = *((unsigned *)f->esp + 3);
    // convert_vaddr((void *)fd);
    convert_vaddr((void *)buffer);
    // convert_vaddr((void *)size);

     f->eax = write(fd, buffer, size);
     break;
   }

   case SYS_SEEK:{
     // Handle seek system call
     // 2 args
     int fd = *((int *)f->esp + 1);
     unsigned position = *((unsigned *)f->esp + 2);
     struct process_file *pf = get_pf(fd);
     if (pf == NULL) {
       f->eax = -1;
     }
     else{
      lock_acquire(&file_lock);
      file_seek(pf->file, position);
      f->eax = position;
      lock_release(&file_lock);      
     }
     break;
   }

   case SYS_TELL:{
     // Handle tell system call
      int fd = *((int *)f->esp + 1);
      struct process_file *pf = get_pf(fd);
      if (pf == NULL) {
        f->eax = -1;
      }
      else{
        lock_acquire(&file_lock);
        f->eax = file_tell(pf->file);
        lock_release(&file_lock);      
      }
     break;
   }

   // Under testing
   case SYS_CLOSE:{
     // Handle close system call
     int* fd_ptr = (int *)f->esp + 1;
     convert_vaddr((void *) fd_ptr);

     close(*fd_ptr);

     break;
   }

   default:
    //  printf("Unknown system call: %d\n", syscall_number);
    //  thread_exit ();
    syscall_exit(-1);
     break;
 }
 
 //thread_exit ();
}
