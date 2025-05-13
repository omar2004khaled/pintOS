#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include <stdbool.h>
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/process.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "devices/input.h"
#include "devices/shutdown.h"
#include "userprog/pagedir.h"
#include "threads/vaddr.h"
#include "userprog/syscall.h"
void syscall_init (void);
static void syscall_handler (struct intr_frame *f) ;
void halt();
void exit(int status);
static void check_address(void *addr);
static int convert_vaddr(void *vaddr);
static void check_string(void* str);
static void check_buffer(void *buffer, int size);
static void syscall_halt(void);
static int syscall_wait(tid_t tid);
static void syscall_exit(int status);
bool create(char *file_name, int initial_size) ;
bool remove(char *file_name) ;
int open(char *file_name) ;
int filesize(int fd) ;
int procces_add_file(struct file *f);
static int write(int fd, char *buffer, unsigned size) ;
static int read(int fd, char *buffer, unsigned size) ;
static void close(int fd);
static struct proccess_file *get_pf(int fd);

#endif /* userprog/syscall.h */
