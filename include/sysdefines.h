/* Copyright (c) 1997-2014, FenixOS Developers
   All Rights Reserved.

   This file is subject to the terms and conditions defined in
   file 'LICENSE', which is part of this source code package.
 */

/*! \file sysdefines.h
 *  This file defines constants and data types used by system calls.
 *
 */

#ifndef _SYSDEFINES_H_
#define _SYSDEFINES_H_

/* Constant declarations. */

/*! Return code when system call returns normally. */
#define ALL_OK                  (0)
/*! Return code when system call returns with an error. */
#define ERROR                   (-1)
/*! Return code when system call is unknown. */
#define ERROR_ILLEGAL_SYSCALL   (-2)

/*! System call that returns the version of the kernel. */
#define SYSCALL_VERSION         (0)
/*! System call that prints a string. */
#define SYSCALL_PRINTS          (1)
/*! System call that prints a hexadecimal value. */
#define SYSCALL_PRINTHEX        (2)

/*! System call that allocates a memory block. The length of the requested
    memory block is passed in edi. The system call returns the address or an
    error code. */
#define SYSCALL_ALLOCATE        (4)

/*! System call that frees a memory block allocated through the allocate
    system call. The address of the memory block is passed in edi. The
    system call returns  ALL_OK if successful or an error code if
    unsuccessful. */
#define SYSCALL_FREE            (5)

/*! System call that terminates the currently running
 *  thread. It takes no parameters. Terminates the process
 *  when there are no threads left. */
#define SYSCALL_TERMINATE       (6)

/*! System call that creates a new process with one single
 *  thread. The program used is the executable whose index is
 *  passed in edi. */
#define SYSCALL_CREATEPROCESS   (7)

/*! System call that will temporarily move the calling thread from the running
    to the ready state. The system call does not take any parameters and returns
    ALL_OK.*/
#define SYSCALL_YIELD           (8)

#endif
