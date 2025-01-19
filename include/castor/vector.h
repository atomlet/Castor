#pragma once

#include "types.h"

// Vector is a dynamic array that automatically grows to accommodate more elements.
// The elements of the Vector can be of any type, but you must provide the size of 
// the element (in bytes).
// You may also provide a VectorInterface to perform deep operations on the elements, 
// such as deep copying and memory release.
typedef struct Vector Vector;

// VectorInterface is used to perform deep operations on the elements of the Vector, 
// such as deep copying and releasing memory.
typedef struct VectorInterface VectorInterface;

// VectorOptions is used to configure the initialization options when constructing 
// a Vector.
typedef struct VectorOptions VectorOptions;


struct VectorInterface {
  // This function performs a deep copy of an element. When copying, you must
  // set the value of the element manually.
  // For example, if the element type is char*, you need to allocate memory 
  // and copy the string,
  // then assign the address of the new string to `dst`.
  // If any errors or failures occur during the copy process, this function 
  // should return false.
  bool (*copy)(void *dst, void *src);

  // This function performs a deep release of an element. You do not need to 
  // release the memory of the element itself.
  // For example, if the element type is char*, you only need to free the memory
  // of the string.
  void (*release)(void *src);
};

struct VectorOptions {
  // Initial capacity of the Vector (in terms of the number of elements).
  usize capacity;

  // Optional interface for the Vector, can be set to nullptr.
  VectorInterface* interface;
};


// Constructs a new Vector with the given element size and initialization options.
[[nodiscard]]
Vector* vector_construct(const usize object_size, const VectorOptions options);

// Returns the element at the specified index in the Vector.
void* vector_get(Vector*, const usize index);

// Checks whether the Vector is empty.
bool vector_empty(const Vector*);

// Applies the provided callback function to each element in the Vector.
void vector_walk(const Vector*, void (*callback)(void*));

// Resets the Vector but does not free its memory.
// If you need to release the Vector's memory, use vector_release.
void vector_reset(Vector*);

// Releases the memory occupied by the Vector.
// If you want to reset the Vector without releasing memory, use vector_reset.
void vector_release(Vector*);

// Releases all memory used by the Vector, including the Vector itself.
// If a release function is available in the interface, vector_release will
// invoke it to deeply release each element.
void vector_destruct(Vector*);

// Expands the Vector by n elements (in terms of element count).
bool vector_grow(Vector*, const usize n);

// Appends an element to the end of the Vector.
bool vector_push_back(Vector*, void*);

// Inserts an element at the beginning of the Vector.
bool vector_push_front(Vector*, void*);

// Removes the last element from the Vector.
bool vector_discard_back(Vector*);

// Removes the first element from the Vector.
bool vector_discard_front(Vector*);

// Removes an element at the specified index from the Vector.
bool vector_discard(Vector*, const usize index);

// Pops the last element from the Vector and stores it in the provided pointer.
bool vector_pop_back(Vector*, void*);

// Pops the first element from the Vector and stores it in the provided pointer.
bool vector_pop_front(Vector*, void*);

// Pops the element at the specified index from the Vector and stores it in the 
// provided  pointer.
bool vector_pop(Vector*, void*, const usize index);

// Sets the value of the element at the specified index in the Vector.
bool vector_set(Vector*, const usize index, void*);

// Inserts an element at the specified index in the Vector.
bool vector_insert(Vector*, const usize index, void*);

// Returns a copy of the Vector.
// If the copy function is available in the interface, vector_copy will call it 
// to deeply copy each element.
// If vector_copy returns false, vector_copy will initialize the elements using
// memset.
[[nodiscard]] 
Vector* vector_copy(Vector*, const bool shrink_to_fit);
