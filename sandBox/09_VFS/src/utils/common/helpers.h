#ifndef INCLUDE_HELPERS_H
#define INCLUDE_HELPERS_H

#pragma once
#include "types.h"

/* Helper function to convert interger to string, used in logger */
s8int *integer_to_string(u32int number);

/* Helper function to get physical end address of multiboot modules */
void get_multiboot_info(u32int mboot_ptr, u32int *initrdPhysicalStart,
                        u32int *multibootPhysicalEnd, u32int *modsCount);

/* Helper function to get address of multiboot strcut */
u32int get_multiboot_address();

/* Helper function to calculate length of string, used in logger */
u32int custom_strlen(const s8int *str);

/* Helper function for memset, used in paging and heap */
void custom_memset(u8int *address, u32int val, u32int size);

/** custom_memcpy:
 * Helper function for memcpy, used in initrd. This is a very basic
 * implementation and assumes we are not copying overlapping memories and
 * unaligned memory copying is not optimized
 *
 * @param destination Destination address
 * @param source      Source address
 * @param size        Number of bytes to be copied
 */
void custom_memcpy(u8int *destination, const u8int *source, u32int size);

/** custom_strcpy:
 * Helper function for strcpy, used in initrd. This is a very basic
 * implementation and appends '\0' in the end. Boundary conditions that are not
 * handled are similiar to that of strcpy
 *
 * @param destination Destination address
 * @param source      Source address
 * @param size        Number of bytes to be copied
 */
void custom_strcpy(s8int *destination, const s8int *source);

/** custom_strcmp:
 * Helper function for strcmp, used in initrd. This is a very basic
 * implementation just compares and return 0 is match 1 otherwise, parameter has
 * additional flag ignore case because in the file system same filename with
 * different case is not allowed.
 *
 * @param  stringFirst   Pointer to first string
 * @param  stringSecond  Pointer to second string
 * @param  ignoreCase    compare case-insensitively is set
 * @return size          0 if match, 1 otherwise
 */
u8int custom_strcmp(const s8int *stringFirst, const s8int *stringSecond,
                    u8int ignoreCase);

#endif /* INCLUDE_HELPERS_H */
