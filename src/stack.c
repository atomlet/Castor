#include "castor/stack.h"
#include "castor/vector.h"
#include "castor/types.h"
#include <stdlib.h>


struct Stack {
  Vector* vector;
};


Stack* stack_construct(const usize object_size, VectorOptions options) {
  Stack* stack = calloc(1, sizeof(Stack));
  if (stack == nullptr) {
    return nullptr;
  }

  stack->vector = vector_construct(object_size, options);
  if (stack->vector == nullptr) {
    free(stack);
    return nullptr;
  }
  
  return stack;
}

void stack_destruct(Stack* this) {
  if (this == nullptr) {
    return;
  }

  vector_destruct(this->vector);
  free(this);
}

bool stack_push(Stack* this, void* object) {
  return vector_push_back(this->vector, object);
}

bool stack_pop(Stack* this, void* object) {
  return vector_pop_back(this->vector, object);
}

bool stack_empty(const Stack* this) {
  return vector_empty(this->vector);
}

void* stack_peek(const Stack* this) {
  return vector_get_back(this->vector);
}

Stack* stack_copy(const Stack* this, const bool shrink_to_fit) {
  Stack* clone = calloc(1, sizeof(Stack));
  if (clone == nullptr) {
    return nullptr;
  }

  clone->vector = vector_copy(this->vector, shrink_to_fit);
  if (clone->vector == nullptr) {
    free(clone);
    return nullptr;
  }

  return clone;
}