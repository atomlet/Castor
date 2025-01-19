#include "castor/vector.h"
#include "castor/types.h"
#include <stdlib.h>
#include <string.h>

// Check if the method is available in the vector's interface
#define VECTOR_INTERFACE_OK(vector, method) \
  (vector->interface != nullptr && vector->interface->method != nullptr)

struct Vector {
  u8 *content;
  usize object_size;
  usize capacity;
  usize count;
  // Interface for custom operations (e.g., release)
  VectorInterface *interface;
};

static Vector *vector_init(Vector *this, const usize capacity) {
  this->content = (u8 *)malloc(capacity * this->object_size);
  if (this->content == nullptr) {
    return nullptr;
  }
  this->capacity = capacity;
  return this;
}

Vector *vector_construct(const usize object_size, const VectorOptions options) {
  Vector *this = NEW(Vector);
  if (this == nullptr) {
    return nullptr;
  }
  this->interface = options.interface;
  this->object_size = object_size;
  if (options.capacity > 0 && !vector_init(this, options.capacity)) {
    free(this);
    return nullptr;
  }
  return this;
}

static void *vector_get_unsafe(const Vector *this, const usize index) {
  return this->content + index * this->object_size;
}

void *vector_get(Vector *this, const usize index) {
  if (index >= this->count) {
    return nullptr;
  }
  return this->content + index * this->object_size;
}

bool vector_empty(const Vector *this) { return this->count == 0; }

static bool vector_unallocated(const Vector *this) {
  return this->content == nullptr;
}

void vector_walk(const Vector *this, void (*callback)(void *object)) {
  if (vector_empty(this)) {
    return;
  }
  for (usize i = 0; i < this->count; i++) {
    callback(vector_get_unsafe(this, i));
  }
}

void vector_reset(Vector *this) {
  if (vector_empty(this)) {
    return;
  }
  // If a release function is provided in the interface, call it for each
  // element
  if (VECTOR_INTERFACE_OK(this, release)) {
    vector_walk(this, this->interface->release);
  }
  this->count = 0;
}

void vector_release(Vector *this) {
  if (vector_unallocated(this)) {
    return;
  }
  vector_reset(this);
  // Free the allocated memory for vector content
  free(this->content);
  this->content = nullptr;
  this->capacity = 0;
}

void vector_destruct(Vector *this) {
  if (this == nullptr) {
    return;
  }
  vector_release(this);
  // Free the vector object itself
  free(this);
}

static bool vector_resize(Vector *this, const usize new_capacity) {
  u8 *new_content =
      (u8 *)realloc(this->content, new_capacity * this->object_size);
  if (new_content == nullptr) {
    return false;
  }
  this->content = new_content;
  this->capacity = new_capacity;
  return true;
}

bool vector_grow(Vector *this, const usize n) {
  usize new_capacity = this->capacity + n;
  // Default to 16 if new capacity is zero
  new_capacity = new_capacity ? new_capacity : 16;
  if (vector_unallocated(this)) {
    return vector_init(this, new_capacity);
    ;
  }
  return vector_resize(this, new_capacity);
}

static bool vector_full(const Vector *this) {
  return this->count == this->capacity;
}

bool vector_push_back(Vector *this, void *object) {
  if (vector_full(this)) {
    if (!vector_grow(this, this->capacity)) {
      return false;
    }
  }
  void *dest = vector_get_unsafe(this, this->count);
  memcpy(dest, object, this->object_size);
  this->count++;
  return true;
}

bool vector_push_front(Vector *this, void *object) {
  if (vector_full(this)) {
    if (!vector_grow(this, this->capacity)) {
      return false;
    }
  }
  // Move all elements one position forward to make space at the front
  void *dest = vector_get_unsafe(this, 1);
  void *src = this->content;

  // Shift elements forward
  memmove(dest, src, this->count * this->object_size);

  // Copy the new element to the front
  memcpy(src, object, this->object_size);
  this->count++;

  return true;
}

bool vector_discard_back(Vector *this) {
  if (vector_empty(this)) {
    return false;
  }
  // If a release function is provided in the interface, call it for the back
  // element
  if (VECTOR_INTERFACE_OK(this, release)) {
    void *object = vector_get_unsafe(this, this->count - 1);
    this->interface->release(object);
  }
  this->count--;
  return true;
}

bool vector_discard_front(Vector *this) {
  if (vector_empty(this)) {
    return false;
  }

  // If a release function is provided in the interface, call it for the front
  // element
  if (VECTOR_INTERFACE_OK(this, release)) {
    void *object = vector_get_unsafe(this, 0);
    this->interface->release(object);
  }

  // Move all elements one position forward to discard the front element
  void *dest = this->content;
  void *src = this->content + this->object_size;

  // Shift elements forward
  memmove(dest, src, this->count * this->object_size);
  this->count--;

  return true;
}

bool vector_discard(Vector *this, const usize index) {
  if (vector_empty(this) || index >= this->count) {
    return false;
  }

  // If a release function is provided in the interface, call it for the element
  // at the specified index
  if (VECTOR_INTERFACE_OK(this, release)) {
    void *object = vector_get_unsafe(this, index);
    this->interface->release(object);
  }

  // Shift all elements after the removed one to fill the gap
  void *dest = vector_get_unsafe(this, index);
  void *src = dest + this->object_size;
  memmove(dest, src, (this->count - index - 1) * this->object_size);
  this->count--;

  return true;
}

bool vector_pop_back(Vector *this, void *object) {
  if (vector_empty(this)) {
    return false;
  }
  // Copy the element to the provided 'object' buffer
  void *src = vector_get_unsafe(this, this->count - 1);
  memcpy(object, src, this->object_size);
  this->count--;
  return true;
}

bool vector_pop_front(Vector *this, void *object) {
  if (vector_empty(this)) {
    return false;
  }

  // Copy the element to the provided 'object' buffer
  void *src = vector_get_unsafe(this, 0);
  memcpy(object, src, this->object_size);

  // Shift all elements forward to fill the gap left by the removed element
  memmove(src, src + this->object_size, this->count * this->object_size);
  this->count--;

  return true;
}

bool vector_pop(Vector *this, void *object, const usize index) {
  if (vector_empty(this) || index >= this->count) {
    return false;
  }

  // Copy the element to the provided 'object' buffer
  void *src = vector_get_unsafe(this, index);
  memcpy(object, src, this->object_size);

  // Shift all elements after the removed one to fill the gap
  void *dest = vector_get_unsafe(this, index);
  void *next = src + this->object_size;
  memmove(dest, next, (this->count - index - 1) * this->object_size);
  this->count--;

  return true;
}

bool vector_set(Vector *this, const usize index, void *object) {
  if (vector_empty(this) || index >= this->count) {
    return false;
  }
  void *dest = vector_get_unsafe(this, index);
  memcpy(dest, object, this->object_size);
  return true;
}

bool vector_insert(Vector *this, const usize index, void *object) {
  if (vector_empty(this) || index >= this->count) {
    return false;
  }

  if (vector_full(this)) {
    if (!vector_grow(this, this->capacity)) {
      return false;
    }
  }

  // Shift all elements after the insertion point to the right
  void *dest = vector_get_unsafe(this, index);
  void *src = dest + this->object_size;
  memmove(src, dest, (this->count - index) * this->object_size);

  // Insert the new element
  memcpy(dest, object, this->object_size);
  this->count++;

  return true;
}

Vector* vector_copy(Vector *this, const bool shrink_to_fit) {
  VectorOptions options = {
    .capacity = shrink_to_fit ? this->count : this->capacity,
    .interface = this->interface,
  };

  if (shrink_to_fit && vector_empty(this)) {
    options.capacity = 0;
    return vector_construct(this->object_size, options);
  }

  Vector* v = vector_construct(this->object_size, options);
  if (v == nullptr) {
    return nullptr;
  }

  v->count = this->count;

  if (!VECTOR_INTERFACE_OK(v, copy)) {
    memcpy(v->content, this->content, this->count * this->object_size);
    v->count = this->count;
    return v;
  }

  for (usize i = 0; i < this->count; i++) {
    void *src = vector_get_unsafe(this, i);
    void *dest = vector_get_unsafe(v, i);
    if (!v->interface->copy(dest, src)) {
      memset(dest, 0, this->object_size);
    }
  }

  return v;
}