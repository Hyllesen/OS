/* Copyright (c) 1997-2014, FenixOS Developers
   All Rights Reserved.

   This file is subject to the terms and conditions defined in
   file 'LICENSE', which is part of this source code package.
 */

/*! \file kernel.c This file holds the code for the kernel. */

#include <stdint.h>
#include <instruction_wrappers.h>
#include <sysdefines.h>

/* First some declarations for data structures and functions found in the
   assembly code or the linker script. */

/*! Entry point into the kernel for system calls via sysenter. */
extern uint8_t sysenter_entry_point[];

/*! The kernel stack used when in the kernel. */
extern uint8_t kernel_stack[];

/*! Location of executable application 0. We put this into an array
    later */
extern uint8_t exec_0_start[];

/*! Location of executable application 1. We put this into an array
    later */
extern uint8_t exec_1_start[];

/*! Location of executable application 2. We put this into an array
    later */
extern uint8_t exec_2_start[];

/*! Halts the machine. Will stop the machine and only reset will wake
    it up. */
extern void halt_the_machine(void);

/*! Go to user space. */
extern void go_to_user_space(void) __attribute__ ((noreturn));

/* Declarations for functions found in other C files. */


/* Clears screen */
extern void cls();

/*! Outputs a string to the VGA screen. */
extern void
kprints(const char* const string
        /*!< Points to a null terminated string */);

/*! Outputs an unsigned 32-bit value to the VGA screen. */
extern void
kprinthex(const register uint32_t value /*! value to be printed. */);

/* Declarations for this file. */

/*! Defines a thread. */
struct thread
{
 uint32_t eax;
 uint32_t ebx;
 uint32_t esi;
 uint32_t edi;
 uint32_t ebp;
 uint32_t esp;
 uint32_t eip;
 /* The above members must be the first in the struct. Do not change the
    order. */
};

/*! Size of the array that holds starting addresses of applications */
#define EXECUTABLE_TABLE_SIZE (3)

/*! Array of starting addresses for all applications. The starting
    address is the address of the first instruction to execute in each
    program. */
uintptr_t executable_table[EXECUTABLE_TABLE_SIZE] = 
 {(uintptr_t)exec_0_start,
  (uintptr_t)exec_1_start,
  (uintptr_t)exec_2_start};

/*! Maximum number of threads in the system. */
#define MAX_THREADS 256

/*! All threads in the system. */
extern struct thread threads[MAX_THREADS];
struct thread threads[MAX_THREADS];

/*! The current thread running on the cpu. */
extern struct thread* current_thread;
struct thread* current_thread = &threads[0];

/*! Initializes the kernel. */
extern void kernel_init(register uint32_t* const multiboot_information)
 __attribute__ ((noreturn));

/*! Handles one system call. */
extern void handle_system_call(void);


/*! Maximum number of threads in the system. */
#define MAX_PROCESSES 256

/* Defines a process */
struct process {
  struct thread proc_thread;
  // address space??
};

/*! All processes in the system. */
extern struct process processes[MAX_PROCESSES];
struct process processes[MAX_PROCESSES];

/*! The current process */
extern struct process* current_process;
struct process* current_process = &processes[0];

extern int numOfProcesses;
extern int programNum;


/* Definitions. */
int numOfProcesses = 0;
int programNum = 0;

void kernel_init(register uint32_t* const multiboot_information
                                          /*!< Points to a multiboot
                                               information structure. */)
{
 /* The Multiboot specification dictates that a boot loader will leave
    information in memory for the kernel to use, see
    http://www.gnu.org/software/grub/manual/multiboot . We go through the
    information and extract the information we need. We also sanity check. */

 /* Check if the boot loader left information on memory. */
 if (!(1 & *multiboot_information))
  halt_the_machine();

 /* Check if we can use sysenter/sysret. It is highly likely that sysenter
    is supported, it has been since Pentium 2, so this is really a sanity
    check. */
 {
  uint32_t eax, ebx, ecx, edx;

  cpuid(1, &eax, &ebx, &ecx, &edx);

  /* Check if the processor says it supports sysenter. */
  if (!(0x800 & edx))
   halt_the_machine();

  /* Pentium Pro has a bug in that it errouneously says it supports
     sysenter. Check if we are running on processor such as Pentium
     Pro which does not support sysenter. */
  if ((((eax & 0x00000F00) >> 8) == 6) &&
     (((eax & 0x000000F0) >> 4) < 3) &&
     ((eax & 0x0000000F) < 3))
   halt_the_machine();
 }

 /* Set up segment selectors. This is a x86 specific concept. Selectors
    are used to control execution modes of the processor and to control
    how data is accessed. Selectors are controlled through the GDT. The
    processor has to be given the size and pointer to the GDT. */
 {
  const static uint32_t gdt[10] = {0, 0, /* null */
                                   0xffff, 0x00cf9a00, /* kernel code */
                                   0xffff, 0x00cf9200, /* kernel data */
                                   0xffff, 0x00cffa00, /* user code */
                                   0xffff, 0x00cff200  /* user data */};

  /* Install it. */
  lgdt(sizeof(gdt) - 1, (uintptr_t) gdt);
  /* Install the local descriptor table. This is related to the GDT and
     installed as a safety/stability precaution. */
  lldt(0);

  /* Make the processor use the new GDT. */
  __asm volatile ("mov %%bx, %%ds\n \
                   mov %%bx, %%es\n \
                   mov %%bx, %%fs\n \
                   mov %%bx, %%gs\n \
                   mov %%bx, %%ss\n \
                   ljmp $8,$1f\n \
                   1:" : : "b" (16) :);
 }

 /* Set up support for sysenter. */
 /* The base code segment selector. This is used to set the other selectors. */
 wrmsr(0x174, 8, 0);
 /* Kernel stack pointer. We keep the top 4 bytes set to zero to keep GDB
    happy. */
 wrmsr(0x175, (uintptr_t)kernel_stack - 4, 0);
 /* The entry point for sysenter. We will end up there at system calls. */
 wrmsr(0x176, (uintptr_t)sysenter_entry_point, 0);

 /* clear the screen (Nicklas' edit) */
 cls();
 kprints("The kernel has booted!\n");

 current_process->proc_thread = threads[0];
 current_thread = &current_process->proc_thread;
 current_thread->eip = executable_table[0];
 

 /* Set up the first thread. For now we do not set up a process. That is
   for you to do later. */
 //threads[0].eip = executable_table[0];

 
 /* Go to user space. */
 go_to_user_space();
}

void handle_system_call(void)
{
 switch (current_thread->eax)
 {
  case SYSCALL_VERSION:
  {
   current_thread->eax = 0x00010000;
   break;
  }

  case SYSCALL_PRINTS:
  {
   /*kprints("SYSCALL_PRINTS WAS CALLED!\n");*/
  //krpints string argument fra %edi
   kprints((char *)current_thread->edi);
   break;
  }

  case SYSCALL_CREATEPROCESS:
  {
    //kprints("SYSCALL_CREATEPROCESS WAS CALLED!\n");
    /*
      One system call, createprocess, creates a new process with a single thread
      The createprocess system call takes a parameter in the edi register . This parameter is
      an integer which is an index into the array of executable programs. The program will be
      started as a process by the system call.
    */
    // when thread is created, all registers in it must be initialized
    
    // Increment process counter
    numOfProcesses++;

    // Giv den næste proces den næste tråd
    processes[numOfProcesses].proc_thread = threads[numOfProcesses];

    // Sæt current_process til den nye proces
    current_process = &processes[numOfProcesses];

    // Aflæs hvilket program der skal startes
    programNum = current_thread->edi;

    // Return ALL_OK or ERROR
    current_thread->eax = ALL_OK;
    //current_thread->eax = ERROR;

    // Sæt current_thread til current_process' tråd
    current_thread = &current_process->proc_thread;

    // Indlæs program i current_thread
    current_thread->eip = executable_table[programNum];    

    break;
  }

/*
terminates the current thread as well as process if it has no other threads.
The terminate system call terminates the currently running thread. The system call
does not take any parameters and will never return to the caller. It does thus not return
any values.
*/
  case SYSCALL_TERMINATE:
  {
    numOfProcesses--;

    // Sæt current_process til den næste proces i "stakken"
    current_process = &processes[numOfProcesses];

    // Sæt current_thread til current_process' tråd
    current_thread = &current_process->proc_thread;

    break;
  }

  default:
  {
   /* Unrecognized system call. Not good. */
   current_thread->eax = ERROR_ILLEGAL_SYSCALL;
  }
 }

 go_to_user_space();
}