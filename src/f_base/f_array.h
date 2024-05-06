#ifndef F_ARRAY_H
#define F_ARRAY_H

#include <stdlib.h>

#define DoubleCapacity(x) ((x) <= 0 ? 8 : x * 2)
#define Array(type) type##_array

#define array_get(type, array, idx) type##_array##_get(array, idx)
#define array_add(type, array, data) type##_array##_add(array, data)
#define array_add_at(type, array, data, idx) type##_array##_add_at(array, data, idx)
#define array_reserve(type, array, count) type##_array##_reserve(array, count)
#define array_remove(type, array, idx) type##_array##_remove(array, idx)
#define array_free(type, array) type##_array##_free(array)

#define ArrayType(Data)\
typedef struct Data##_array {\
  u32 capacity;\
  u32 length;\
  Data* elements;\
} Data##_array;\
Data Data##_array##_get(Data##_array* array, u32 idx);\
void Data##_array##_add(Data##_array* array, Data data);\
void Data##_array##_add_at(Data##_array* array, Data data, u32 idx);\
void Data##_array##_reserve(Data##_array* array, u32 count);\
Data Data##_array##_remove(Data##_array* array, int idx);\
void Data##_array##_free(Data##_array* array);\
\
Data Data##_array##_get(Data##_array* array, u32 idx) {\
  return array->elements[idx];\
}\
\
void Data##_array##_add(Data##_array* array, Data data) {\
  if (array->length + 1 > array->capacity) {\
    void* prev = array->elements;\
    u32 new_cap = DoubleCapacity(array->capacity);\
    array->elements = calloc(new_cap, sizeof(Data));\
    memmove(array->elements, prev, array->length * sizeof(Data));\
    array->capacity = new_cap;\
    free(prev);\
  }\
  array->elements[array->length++] = data;\
}\
\
void Data##_array##_add_at(Data##_array* array, Data data, u32 idx) {\
  if (array->length + 1 > array->capacity) {\
    void* prev = array->elements;\
    u32 new_cap = DoubleCapacity(array->capacity);\
    array->elements = calloc(new_cap, sizeof(Data));\
    memmove(array->elements, prev, array->length * sizeof(Data));\
    array->capacity = new_cap;\
    free(prev);\
  }\
  memmove(array->elements + idx + 1, array->elements + idx, sizeof(Data) * (array->length - idx));\
  array->elements[idx] = data;\
  array->length++;\
}\
\
void Data##_array##_reserve(Data##_array* array, u32 count) {\
  void* prev = array->elements;\
  u32 new_cap = count;\
  array->elements = calloc(new_cap, sizeof(Data));\
  memmove(array->elements, prev, array->length * sizeof(Data));\
  array->capacity = new_cap;\
  if (prev) free(prev);\
}\
\
Data Data##_array##_remove(Data##_array* array, int idx) {\
  if (idx >= array->length || idx < 0) return (Data){0};\
  Data value = array->elements[idx];\
  if (idx == array->length - 1) {\
    array->length--;\
    return value;\
  }\
  Data* from = array->elements + idx + 1;\
  Data* to = array->elements + idx;\
  memmove(to, from, sizeof(Data) * (array->length - idx - 1));\
  array->length--;\
  return value;\
}\
\
void Data##_array##_free(Data##_array* array) {\
  array->capacity = 0;\
  array->length = 0;\
  free(array->elements);\
}

#endif // F_ARRAY_H