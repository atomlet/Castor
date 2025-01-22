#pragma once
#include "types.h"


// Vector is a dynamic array that automatically grows to accommodate more 
// object.
// The object of the Vector can be of any type, but you must provide the 
// size of the object (in bytes).
// You may also provide a VectorInterface to perform deep operations on the 
// object, such as deep copying and memory release.
typedef struct Vector Vector;

// VectorInterface is used to perform deep operations on the objects of the
// vector, such as deep copying and releasing memory.
typedef struct VectorInterface VectorInterface;

// VectorOptions is used to configure the initialization options when 
// constructing a Vector.
typedef struct VectorOptions VectorOptions;


// Member:
// - copy:
//    This function performs a deep copy of an object. When copying, you must
//    set the value of the object manually.
//    For example, if the object type is char*, you need to allocate memory 
//    and copy the string, then assign the address of the new string to `dst`.
//    If any errors or failures occur during the copy process, this function 
//    should return false.
// - release:
//    This function performs a deep release of an object. You do not need to
//    release the memory of the object itself.
//    For example, if the object type is char*, you only need to free the
//    memory of the string.
struct VectorInterface {
  bool (*copy)(void* dst, void* src);
  void (*release)(void* src);
};

// Member:
// - capacity:
//    Initial capacity of the Vector (in terms of the number of objects).
// - interface:
//    Optional interface for the Vector, can be set to nullptr.
struct VectorOptions {
  usize            capacity;
  VectorInterface* interface;
};


// Constructs a new Vector with the given object size and initialization 
// options.
[[nodiscard, gnu::malloc]] 
Vector* vector_construct(
  const usize         object_size, 
  const VectorOptions options
);

// Returns the object at the specified index in the Vector.
void* vector_get(Vector*, const usize);

// Checks whether the Vector is empty.
bool vector_empty(const Vector*);

// Applies the provided callback function to each object in the Vector.
void vector_walk(const Vector*, void (*)(void*));

// Resets the Vector but does not free its memory.
// If you need to release the Vector's memory, use vector_release.
void vector_reset(Vector*);

// Releases the memory occupied by the Vector.
// If you want to reset the Vector without releasing memory, use vector_reset.
void vector_release(Vector*);

// Releases all memory used by the Vector, including the Vector itself.
// If a release function is available in the interface, vector_release will
// invoke it to deeply release each object.
void vector_destruct(Vector*);

// Expands the Vector by n objects (in terms of object count).
bool vector_grow(Vector*, const usize);

// Appends an object to the end of the Vector.
bool vector_push_back(Vector*, void*);

// Inserts an object at the beginning of the Vector.
bool vector_push_front(Vector*, void*);

// Removes the last object from the Vector.
bool vector_discard_back(Vector*);

// Removes the first object from the Vector.
bool vector_discard_front(Vector*);

// Removes an object at the specified index from the Vector.
bool vector_discard(Vector*, const usize);

// Pops the last object from the Vector and stores it in the provided pointer.
bool vector_pop_back(Vector*, void*);

// Pops the first object from the Vector and stores it in the provided pointer.
bool vector_pop_front(Vector*, void*);

// Pops the object at the specified index from the Vector and stores it in 
// the provided pointer.
bool vector_pop(Vector*, void*, const usize);

// Sets the value of the object at the specified index in the Vector.
bool vector_set(Vector*, const usize, void*);

// Inserts an object at the specified index in the Vector.
bool vector_insert(Vector*, const usize, void*);

// Returns a copy of the Vector.
// Parameters:
// - shrink_to_fit: 
//    If true, the new vector will have its capacity shrunk to fit its 
//    current size.
// Details:
// - If the copy function is available in the interface, vector_copy will call
//   it to deeply copy each object.
// - If vector_copy returns false, vector_copy will initialize the objects 
//   using memset.
[[nodiscard, gnu::malloc]] 
Vector* vector_copy(Vector*, const bool shrink_to_fit);

// Returns a pointer to the last object in the vector.
// If the vector is empty, returns nullptr.
void* vector_get_back(const Vector*);

// Returns a pointer to the first object in the vector.
// If the vector is empty, returns nullptr.
void* vector_get_front(const Vector*);