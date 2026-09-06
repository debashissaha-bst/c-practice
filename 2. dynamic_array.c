

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int *data;      
    size_t size;     
    size_t capacity;  
} DynamicArray;


static void da_init(DynamicArray *da, size_t initial_capacity) {
    if (initial_capacity == 0) initial_capacity = 4;
    da->data = malloc(initial_capacity * sizeof(int));
    if (!da->data) {
        fprintf(stderr, "Fatal: malloc failed in da_init\n");
        exit(EXIT_FAILURE);
    }
    da->size = 0;
    da->capacity = initial_capacity;
}


static void da_grow(DynamicArray *da) {
    size_t new_capacity = da->capacity * 2;
    int *new_data = realloc(da->data, new_capacity * sizeof(int));
    if (!new_data) {
        fprintf(stderr, "Fatal: realloc failed in da_grow\n");
        exit(EXIT_FAILURE);
    }
    da->data = new_data;
    da->capacity = new_capacity;
    printf("[grow] capacity expanded to %zu\n", da->capacity);
}

static void da_push(DynamicArray *da, int value) {
    if (da->size == da->capacity) {
        da_grow(da);
    }
    da->data[da->size++] = value;
}


static int da_pop(DynamicArray *da) {
    if (da->size == 0) {
        fprintf(stderr, "Error: pop from empty array\n");
        exit(EXIT_FAILURE);
    }
    return da->data[--da->size];
}


static void da_insert(DynamicArray *da, size_t index, int value) {
    if (index > da->size) {
        fprintf(stderr, "Error: insert index out of range\n");
        return;
    }
    if (da->size == da->capacity) {
        da_grow(da);
    }
    for (size_t i = da->size; i > index; i--) {
        da->data[i] = da->data[i - 1];
    }
    da->data[index] = value;
    da->size++;
}


static void da_remove_at(DynamicArray *da, size_t index) {
    if (index >= da->size) {
        fprintf(stderr, "Error: remove index out of range\n");
        return;
    }
    for (size_t i = index; i < da->size - 1; i++) {
        da->data[i] = da->data[i + 1];
    }
    da->size--;
}


static void da_shrink_to_fit(DynamicArray *da) {
    if (da->size == da->capacity) return;
    size_t new_capacity = da->size == 0 ? 1 : da->size;
    int *new_data = realloc(da->data, new_capacity * sizeof(int));
    if (new_data || new_capacity == 0) {
        da->data = new_data;
        da->capacity = new_capacity;
    }
}

static void da_print(const DynamicArray *da) {
    printf("[ ");
    for (size_t i = 0; i < da->size; i++) {
        printf("%d ", da->data[i]);
    }
    printf("] (size=%zu, capacity=%zu)\n", da->size, da->capacity);
}


static void da_free(DynamicArray *da) {
    free(da->data);
    da->data = NULL;
    da->size = 0;
    da->capacity = 0;
}

int main(void) {
    DynamicArray da;
    da_init(&da, 2); 

    printf("Pushing 0..9:\n");
    for (int i = 0; i < 10; i++) {
        da_push(&da, i);
    }
    da_print(&da);

    printf("\nInserting 99 at index 3:\n");
    da_insert(&da, 3, 99);
    da_print(&da);

    printf("\nRemoving element at index 0:\n");
    da_remove_at(&da, 0);
    da_print(&da);

    printf("\nPopping last element: %d\n", da_pop(&da));
    da_print(&da);

    printf("\nShrinking to fit:\n");
    da_shrink_to_fit(&da);
    da_print(&da);

    da_free(&da);
    printf("\nArray freed. Program complete.\n");
    return 0;
}
