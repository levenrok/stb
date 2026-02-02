#include "unity.h"

#define STB_ARENA_IMPLEMENTATION
#include "../arena.h"

Arena arena;

void setUp() {
    arena = init_arena(1024);
}

void tearDown() {
    free_arena(&arena);
}

void test_allocation() {
    arena.offset = 0;
    arena.prev_offset = 0;

    int* test_int = (int*)push_arena(&arena, sizeof(int));
    TEST_ASSERT_NOT_NULL(test_int);

    *test_int = 123;
    TEST_ASSERT_EQUAL_INT(123, *test_int);
    TEST_ASSERT_EQUAL(4, arena.offset);

    typedef struct TestStruct {
        int x;
        float y;
    } TestStruct;

    TestStruct* test_struct =
        (TestStruct*)push_arena(&arena, sizeof(TestStruct));
    TEST_ASSERT_NOT_NULL(test_struct);

    test_struct->x = 42;
    test_struct->y = 3.14f;
    TEST_ASSERT_EQUAL_INT(42, test_struct->x);
    TEST_ASSERT_EQUAL_FLOAT(3.14, test_struct->y);
    TEST_ASSERT_EQUAL(24, arena.offset);
}

void test_deallocation() {
    arena.offset = 0;
    arena.prev_offset = 0;

    char* test_char = (char*)push_arena(&arena, sizeof(char));
    TEST_ASSERT_NOT_NULL(test_char);
    *test_char = 'a';
    TEST_ASSERT_EQUAL_CHAR('a', *test_char);
    TEST_ASSERT_EQUAL(1, arena.offset);

    pop_arena(&arena);
    TEST_ASSERT_EQUAL(0, arena.offset);

    int* test_int = (int*)push_arena(&arena, sizeof(int));
    TEST_ASSERT_NOT_NULL(test_int);
    *test_int = 123;
    TEST_ASSERT_EQUAL_INT(123, *test_int);
    TEST_ASSERT_EQUAL(4, arena.offset);

    float* test_float = (float*)push_arena(&arena, sizeof(float));
    TEST_ASSERT_NOT_NULL(test_float);
    *test_float = 3.14f;
    TEST_ASSERT_EQUAL_FLOAT(3.14, *test_float);
    TEST_ASSERT_EQUAL(20, arena.offset);

    pop_arena(&arena);
    TEST_ASSERT_EQUAL(4, arena.offset);
}

void test_alignment() {
    arena.offset = 0;
    arena.prev_offset = 0;

    char* test_char =
        (char*)push_arena_aligned(&arena, sizeof(char), CHAR_ALIGNMENT);
    TEST_ASSERT_NOT_NULL(test_char);
    *test_char = 'a';
    TEST_ASSERT_EQUAL(1, arena.offset);
    TEST_ASSERT_EQUAL(0, arena.prev_offset);

    char* test_char2 =
        (char*)push_arena_aligned(&arena, sizeof(char), CHAR_ALIGNMENT);
    TEST_ASSERT_NOT_NULL(test_char2);
    *test_char2 = 'b';
    TEST_ASSERT_EQUAL(2, arena.offset);
    TEST_ASSERT_EQUAL(1, arena.prev_offset);

    int* test_int =
        (int*)push_arena_aligned(&arena, sizeof(int), INT_ALIGNMENT);
    TEST_ASSERT_NOT_NULL(test_int);
    *test_int = 123;
    TEST_ASSERT_EQUAL(8, arena.offset);
    TEST_ASSERT_EQUAL(2, arena.prev_offset);
}

void test_mark() {
    arena.offset = 0;
    arena.prev_offset = 0;

    char* test_char = (char*)push_arena(&arena, sizeof(char));
    TEST_ASSERT_NOT_NULL(test_char);
    *test_char = 'a';
    TEST_ASSERT_EQUAL(1, arena.offset);

    int* test_int = (int*)push_arena(&arena, sizeof(int));
    TEST_ASSERT_NOT_NULL(test_int);
    *test_int = 123;
    TEST_ASSERT_EQUAL(20, arena.offset);

    ArenaMark test_int_mark = get_arena_mark(&arena);
    TEST_ASSERT_EQUAL(20, test_int_mark);

    float* test_float = (float*)push_arena(&arena, sizeof(float));
    TEST_ASSERT_NOT_NULL(test_float);
    *test_float = 3.14f;
    TEST_ASSERT_EQUAL_FLOAT(3.14, *test_float);
    TEST_ASSERT_EQUAL(36, arena.offset);

    float* test_float2 = (float*)push_arena(&arena, sizeof(float));
    TEST_ASSERT_NOT_NULL(test_float2);
    *test_float2 = 1.61f;
    TEST_ASSERT_EQUAL_FLOAT(1.61, *test_float2);
    TEST_ASSERT_EQUAL(52, arena.offset);

    reset_to_arena_mark(&arena, test_int_mark);
    TEST_ASSERT_EQUAL(20, arena.offset);
}

void test_reset() {
    arena.offset = 0;
    arena.prev_offset = 0;

    char* test_char = (char*)push_arena(&arena, sizeof(char));
    TEST_ASSERT_NOT_NULL(test_char);
    *test_char = 'a';
    TEST_ASSERT_EQUAL(1, arena.offset);

    reset_arena(&arena);
    TEST_ASSERT_EQUAL(0, arena.offset);
    TEST_ASSERT_EQUAL(0, arena.prev_offset);

    float* test_float = (float*)push_arena(&arena, sizeof(float));
    TEST_ASSERT_NOT_NULL(test_float);
    *test_float = 3.14f;
    TEST_ASSERT_EQUAL_FLOAT(3.14, *test_float);
    TEST_ASSERT_EQUAL(4, arena.offset);

    reset_arena(&arena);
    TEST_ASSERT_EQUAL(0, arena.offset);
    TEST_ASSERT_EQUAL(0, arena.prev_offset);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_allocation);
    RUN_TEST(test_deallocation);
    RUN_TEST(test_alignment);
    RUN_TEST(test_mark);
    RUN_TEST(test_reset);

    return UNITY_END();
}
