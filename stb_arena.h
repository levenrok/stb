#ifndef STB_ARENA
#define STB_ARENA

#include <stddef.h>
#include <stdint.h>

#ifndef STB_ARENA_DEFAULT_ALIGNMENT
#define STB_ARENA_DEFAULT_ALIGNMENT (2 * sizeof(void*))
#endif

typedef struct {
    unsigned char* buffer;
    size_t size;
    size_t offset;
    size_t prev_offset;
} Arena;

typedef enum Alignment {
    CHAR_ALIGNMENT = 1,
    SHORT_ALIGNMENT = 2,
    INT_ALIGNMENT = 4,
    LONG_ALIGNMENT = sizeof(long),
    FLOAT_ALIGNMENT = 4,
    POINTER_ALIGNMENT = sizeof(void*),
} Alignment;

typedef size_t ArenaMark;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes an arena with a given size from the heap.
 *
 * @param size Total size of the arena to create
 * @return Initialized arena
 */
Arena stb_arena_init(size_t size);

/**
 * @brief Allocates a block of memory from the arena with a specific alignment.
 *
 * @param arena Reference to a initialized arena
 * @param size Size of the allocating block
 * @param alignment Alignment of the data type
 * @return Pointer to the pushed block
 */
void* stb_arena_push_aligned(Arena* arena, size_t size, Alignment alignment);

/**
 * @def push_arena(arena, size)
 * @brief Allocates a block of memory from the arena with default alignment.
 *
 * @param arena Reference to a initialized arena
 * @param Size of the allocating block
 * @return Pointer to the pushed block
 */
#define stb_arena_push(arena, size) \
    stb_arena_push_aligned(arena, size, STB_ARENA_DEFAULT_ALIGNMENT)

/**
 * @brief Returns the current offset of the arena, which can be used as a mark
 * for later reset.
 *
 * @param arena Reference to a initialized arena
 * @return Mark of the recently pushed block
 */
ArenaMark stb_arena_get_mark(Arena* arena);

/**
 * @brief Resets the arena's offset to a previously obtained mark, effectively
 * deallocating all memory allocated after that mark.
 *
 * @param arena Reference to a initialized arena
 * @param mark Mark of the pushed block to reset to
 */
void stb_arena_reset_to_mark(Arena* arena, ArenaMark mark);

/**
 * @brief Pops the last allocation.
 * @note
 * This only works for the most recent allocation. It does not support
 * nested push/pop sequences.
 *
 * @param arena Reference to a initialized arena
 */
void stb_arena_pop(Arena* arena);

/**
 * @brief Resets the arena, making all its memory available for new allocations.
 *
 * @param arena Reference to a initialized arena
 */
void stb_arena_reset(Arena* arena);

/**
 * @brief Frees the memory block of the arena.
 *
 * @param arena Reference to a initialized arena
 */
void stb_arena_free(Arena* arena);

#ifdef __cplusplus
}
#endif

#endif  // STB_ARENA

#ifdef STB_ARENA_IMPLEMENTATION

#include <assert.h>
#include <stdlib.h>

Arena stb_arena_init(size_t size) {
    unsigned char* buffer = (unsigned char*)malloc(size);

    if (buffer) {
        return (Arena){
            .buffer = buffer,
            .size = size,
            .offset = 0,
            .prev_offset = 0,
        };
    } else {
        return (Arena){0};
    }
}

static inline uintptr_t align_forward(uintptr_t ptr, size_t alignment) {
    assert((alignment & (alignment - 1)) == 0 &&
           "alignment must be a power of 2");
    return (ptr + alignment - 1) & ~(alignment - 1);
}

void* stb_arena_push_aligned(Arena* arena, size_t size, Alignment alignment) {
    uintptr_t current_ptr = (uintptr_t)arena->buffer + arena->offset;
    uintptr_t aligned_ptr = align_forward(current_ptr, alignment);

    size_t aligned_offset = aligned_ptr - (uintptr_t)arena->buffer;

    if (aligned_offset + size > arena->size) {
        return NULL;
    }

    arena->prev_offset = arena->offset;
    arena->offset = aligned_offset + size;

    return (void*)aligned_ptr;
}

ArenaMark stb_arena_get_mark(Arena* arena) {
    return arena->offset;
}

void stb_arena_reset_to_mark(Arena* arena, ArenaMark mark) {
    assert(mark <= arena->offset &&
           "cannot reset to a mark beyond the current offset.");
    arena->offset = mark;
}

void stb_arena_pop(Arena* arena) {
    stb_arena_reset_to_mark(arena, arena->prev_offset);
}

void stb_arena_reset(Arena* arena) {
    arena->offset = 0;
    arena->prev_offset = 0;
}

void stb_arena_free(Arena* arena) {
    if (arena->buffer) {
        free(arena->buffer);
    }
    *arena = (Arena){0};
}

#endif  // STB_ARENA_IMPLEMENTATION
