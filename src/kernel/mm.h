/* Copyright (c) 1997-2014, FenixOS Developers
   All Rights Reserved.

   This file is subject to the terms and conditions defined in
   file 'LICENSE', which is part of this source code package.
 */

/*! \file mm.h Memory management skeleton header file. */

#ifndef _MM_H_
#define _MM_H_

#include <stdint.h>

/**
 * @name    embedded_malloc
 * @brief   Allocate at least size contiguous bytes of memory and return a pointer to the first byte.
 */
void* embedded_malloc(size_t size);

/**
 * @name    embedded_free
 * @brief   Frees previously allocated memory and make it available for subsequent calls to embedded_malloc.
 */
void embedded_free(void *ptr);

/**
 * @name    initialize
 * @brief   Initializes the memory system.
 */
void initialize();

/**
 * @name    The lowest address of the memory you will manage
 * @brief   This points to the lowest address of memory you will manage
 */
extern uintptr_t lowest_available_physical_memory;


/**
 * @name    The highest address of the memory you will manage
 * @brief   This points to the highest address of memory you will manage
 */
extern uintptr_t top_of_available_physical_memory;

#endif
