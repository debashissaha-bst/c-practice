
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 100
#define NAME_LEN     50
#define FILENAME     "students.dat"

typedef struct {
    int  id;
    char name[NAME_LEN];
    int  age;
    float gpa;
} Student;




static int load_students(Student *arr, int max_count) {
    FILE *fp = fopen(FILENAME, "rb");
    if (!fp) return 0; 
    int count = (int)fread(arr, sizeof(Student), max_count, fp);
    fclose(fp);
    return count;
}


static void save_students(const Student *arr, int count) {
    FILE *fp = fopen(FILENAME, "wb");
    if (!fp) {
        perror("Error opening file for write");
        return;
    }
    fwrite(arr, sizeof(Student), count, fp);
    fclose(fp);
}



static int find_index_by_id(const Student *arr, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (arr[i].id == id) return i;
    }
    return -1;
}

static void add_student(Student *arr, int *count) {
    if (*count >= MAX_STUDENTS) {
        printf("Storage full, cannot add more students.\n");
        return;
    }
    Student s;
    printf("Enter ID: ");
    if (scanf("%d", &s.id) != 1) { while (getchar() != '\n'); return; }

    if (find_index_by_id(arr, *count, s.id) != -1) {
        printf("A student with ID %d already exists.\n", s.id);
        return;
    }

    printf("Enter Name: ");
    if (scanf(" %49[^\n]", s.name) != 1) return;
    printf("Enter Age: ");
    if (scanf("%d", &s.age) != 1) return;
    printf("Enter GPA: ");
    if (scanf("%f", &s.gpa) != 1) return;

    arr[*count] = s;
    (*count)++;
    printf("Student added successfully.\n");
}

static void print_student(const Student *s) {
    printf("ID: %-5d Name: %-20s Age: %-4d GPA: %.2f\n",
           s->id, s->name, s->age, s->gpa);
}

static void view_all(const Student *arr, int count) {
    if (count == 0) {
        printf("No records to display.\n");
        return;
    }
    printf("\n--- All Student Records (%d) ---\n", count);
    for (int i = 0; i < count; i++) print_student(&arr[i]);
}

static void search_student(const Student *arr, int count) {
    int id;
    printf("Enter ID to search: ");
    if (scanf("%d", &id) != 1) return;
    int idx = find_index_by_id(arr, count, id);
    if (idx == -1) {
        printf("No student found with ID %d.\n", id);
    } else {
        print_student(&arr[idx]);
    }
}

static void update_student(Student *arr, int count) {
    int id;
    printf("Enter ID to update: ");
    if (scanf("%d", &id) != 1) return;
    int idx = find_index_by_id(arr, count, id);
    if (idx == -1) {
        printf("No student found with ID %d.\n", id);
        return;
    }
    Student *s = &arr[idx]; 
    printf("Enter new Name: ");
    if (scanf(" %49[^\n]", s->name) != 1) return;
    printf("Enter new Age: ");
    if (scanf("%d", &s->age) != 1) return;
    printf("Enter new GPA: ");
    if (scanf("%f", &s->gpa) != 1) return;
    printf("Record updated.\n");
}

static void delete_student(Student *arr, int *count) {
    int id;
    printf("Enter ID to delete: ");
    if (scanf("%d", &id) != 1) return;
    int idx = find_index_by_id(arr, *count, id);
    if (idx == -1) {
        printf("No student found with ID %d.\n", id);
        return;
    }
    
    for (int i = idx; i < *count - 1; i++) {
        arr[i] = arr[i + 1];
    }
    (*count)--;
    printf("Record deleted.\n");
}

static void print_menu(void) {
    printf("\n===== Student Record Management =====\n");
    printf("1. Add Student\n");
    printf("2. View All Students\n");
    printf("3. Search Student by ID\n");
    printf("4. Update Student\n");
    printf("5. Delete Student\n");
    printf("6. Save & Exit\n");
    printf("Choice: ");
}

int main(void) {
    Student students[MAX_STUDENTS];
    int count = load_students(students, MAX_STUDENTS);
    printf("Loaded %d existing record(s) from %s.\n", count, FILENAME);

    int choice;
    do {
        print_menu();
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); continue; }

        switch (choice) {
            case 1: add_student(students, &count); break;
            case 2: view_all(students, count); break;
            case 3: search_student(students, count); break;
            case 4: update_student(students, count); break;
            case 5: delete_student(students, &count); break;
            case 6:
                save_students(students, count);
                printf("Saved %d record(s) to %s. Goodbye!\n", count, FILENAME);
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 6);

    return 0;
}
