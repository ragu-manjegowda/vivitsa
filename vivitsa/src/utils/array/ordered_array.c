#include "ordered_array.h"
#include <helpers.h>
#include <kheap.h>
#include <logger.h>

ordered_array_t create_ordered_array(uint32_t maxSize,
                                     compare_predicate_t compare) {
  ordered_array_t toRet;
  toRet.array = (type_t *)kmalloc(maxSize * sizeof(type_t));
  custom_memset((uint8_t *)toRet.array, 0, maxSize * sizeof(type_t));
  toRet.size = 0;
  toRet.maxSize = maxSize;
  toRet.compare = compare;
  return toRet;
}

ordered_array_t place_ordered_array(void *address, uint32_t maxSize,
                                    compare_predicate_t compare) {
  ordered_array_t toRet;
  toRet.array = (type_t *)address;
  custom_memset((uint8_t *)toRet.array, 0, maxSize * sizeof(type_t));
  toRet.size = 0;
  toRet.maxSize = maxSize;
  toRet.compare = compare;
  return toRet;
}

void destroy_ordered_array(ordered_array_t *array __attribute__((unused))) {
  kfree(array->array);
}

void insert_ordered_array(type_t item, ordered_array_t *array) {
  if (!array->compare) {
    print_screen(
        "\nError: insert_ordered_array : No compare function provided\n");
    return;
  }

  uint32_t iterator = 0;
  while ((iterator < array->size) &&
         (array->compare(array->array[iterator], item))) {
    iterator++;
  }

  // If we don't find spot in between to insert, insert to the end
  if (iterator == array->size) {
    array->array[array->size++] = item;
  }
  // if We find spot insert and shift all the elements to right
  else {
    type_t tmp = array->array[iterator];
    array->array[iterator] = item;
    while (iterator < array->size) {
      iterator++;
      type_t tmp2 = array->array[iterator];
      array->array[iterator] = tmp;
      tmp = tmp2;
    }
    array->size++;
  }
}

type_t peek_ordered_array(uint32_t index, ordered_array_t *array) {
  if (index >= array->size) {
    print_screen("\nError: peek_ordered_array : Index out of bound\n");
    return 0;
  }
  return array->array[index];
}

void remove_ordered_array(uint32_t index, ordered_array_t *array) {
  while (index < array->size) {
    array->array[index] = array->array[index + 1];
    index++;
  }
  array->size--;
}
