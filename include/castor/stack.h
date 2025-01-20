#pragma once

#include "types.h"
#include "vector.h"

// Represents a stack implemented using a dynamic vector.
// The stack grows dynamically and can store objects of any size.
typedef struct Stack Stack;

// Constructs a new stack with the specified object size and vector options.
// Parameters:
// - object_size: Size of each object in bytes.
// - options: Configuration options for the underlying vector.
// Returns:
// - A pointer to the newly created Stack, or nullptr if allocation fails.
[[nodiscard]]
Stack* stack_construct(const u32 object_size, VectorOptions options);

// Destroys the stack and frees its memory.
void stack_destruct(Stack*);

// Pushes an object onto the stack.
// Parameters:
// - object: A pointer to the object to push.
bool stack_push(Stack*, void* object);

// Pops the top object from the stack.
// Parameters:
// - object: A pointer to a memory location where the popped object will be stored.
bool stack_pop(Stack*, void* object);

// Checks if the stack is empty.
bool stack_empty(const Stack*);

// Retrieves the top object of the stack without removing it.
// Returns:
// - A pointer to the top object, or nullptr if the stack is empty.
void* stack_peek(const Stack*);

// Creates a deep copy of the given stack.
// Parameters:
// - this: The stack to copy.
// - shrink_to_fit: If true, the new stack's underlying vector will have its 
//                  capacity shrunk to fit its current size.
// Returns:
// - A pointer to the newly created stack, or nullptr if allocation fails.
[[nodiscard]]
Stack* stack_copy(const Stack*, const bool shrink_to_fit);