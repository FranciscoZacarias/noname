#ifndef F_ARRAY_H
#define F_ARRAY_H

#define Array(type) type##_array

#define CreateArrayType(type)\
\
typedef struct type##_array {\
  u64 capacity;\
  u64 length;\
  type* elements;\
} type##_array; \
\


#endif // F_ARRAY_H