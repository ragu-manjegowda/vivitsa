#ifndef INCLUDE_HELPERS_H
#define INCLUDE_HELPERS_H

#pragma once
#include "types.h"

/* Helper function to convert interger to string, used in logger */
int8_t *integer_to_string(uint32_t number);

/* Helper function to get physical end address of multiboot modules */
void get_multiboot_info(uint32_t mboot_ptr, uint32_t *initrdPhysicalStart,
                        uint32_t *multibootPhysicalEnd, uint32_t *modsCount);

/* Helper function to get address of multiboot strcut */
uint32_t get_multiboot_address();

/* Helper function to calculate length of string, used in logger */
uint32_t custom_strlen(const int8_t *str);

/* Helper function for memset, used in paging and heap */
void custom_memset(uint8_t *address, uint32_t val, uint32_t size);

/** custom_memcpy:
 * Helper function for memcpy, used in initrd. This is a very basic
 * implementation and assumes we are not copying overlapping memories and
 * unaligned memory copying is not optimized
 *
 * @param destination Destination address
 * @param source      Source address
 * @param size        Number of bytes to be copied
 */
void custom_memcpy(uint8_t *destination, const uint8_t *source, uint32_t size);

/** custom_strcpy:
 * Helper function for strcpy, used in initrd. This is a very basic
 * implementation and appends '\0' in the end. Boundary conditions that are not
 * handled are similiar to that of strcpy
 *
 * @param destination Destination address
 * @param source      Source address
 * @param size        Number of bytes to be copied
 */
void custom_strcpy(int8_t *destination, const int8_t *source);

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
uint8_t custom_strcmp(const int8_t *stringFirst, const int8_t *stringSecond,
                    uint8_t ignoreCase);

#endif /* INCLUDE_HELPERS_H */
