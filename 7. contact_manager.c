#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> 

#define NAME_LEN  50
#define PHONE_LEN 20
#define EMAIL_LEN 50
#define FILENAME  "contacts.txt"

typedef struct Contact {
    char name[NAME_LEN];
    char phone[PHONE_LEN];
    char email[EMAIL_LEN];
    struct Contact *next;
} Contact;


static Contact *create_contact(const char *name, const char *phone, const char *email) {
    Contact *c = malloc(sizeof(Contact));
    strncpy(c->name, name, NAME_LEN - 1);   c->name[NAME_LEN - 1] = '\0';
    strncpy(c->phone, phone, PHONE_LEN - 1); c->phone[PHONE_LEN - 1] = '\0';
    strncpy(c->email, email, EMAIL_LEN - 1); c->email[EMAIL_LEN - 1] = '\0';
    c->next = NULL;
    return c;
}

static Contact *insert_sorted(Contact *head, Contact *new_contact) {
    if (head == NULL || strcasecmp(new_contact->name, head->name) < 0) {
        new_contact->next = head;
        return new_contact;
    }
    Contact *cur = head;
    while (cur->next != NULL && strcasecmp(cur->next->name, new_contact->name) < 0) {
        cur = cur->next;
    }
    new_contact->next = cur->next;
    cur->next = new_contact;
    return head;
}

static Contact *find_by_name(Contact *head, const char *name) {
    for (Contact *cur = head; cur != NULL; cur = cur->next) {
        if (strcasecmp(cur->name, name) == 0) return cur;
    }
    return NULL;
}


static Contact *delete_contact(Contact *head, const char *name, int *found) {
    Contact dummy;
    dummy.next = head;
    Contact *prev = &dummy;
    Contact *cur = head;

    *found = 0;
    while (cur != NULL) {
        if (strcasecmp(cur->name, name) == 0) {
            prev->next = cur->next;
            free(cur);
            *found = 1;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    return dummy.next;
}

static void print_contact(const Contact *c) {
    printf("  Name: %-20s Phone: %-15s Email: %s\n", c->name, c->phone, c->email);
}

static void print_all(const Contact *head) {
    if (!head) {
        printf("Address book is empty.\n");
        return;
    }
    printf("--- Contacts ---\n");
    for (const Contact *cur = head; cur != NULL; cur = cur->next) {
        print_contact(cur);
    }
}

static void free_all(Contact *head) {
    while (head != NULL) {
        Contact *tmp = head;
        head = head->next;
        free(tmp);
    }
}


static Contact *load_contacts(void) {
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) return NULL;

    Contact *head = NULL;
    char name[NAME_LEN], phone[PHONE_LEN], email[EMAIL_LEN];
    while (fscanf(fp, " %49[^|]|%19[^|]|%49[^\n]\n", name, phone, email) == 3) {
        Contact *c = create_contact(name, phone, email);
        head = insert_sorted(head, c);
    }
    fclose(fp);
    return head;
}

static void save_contacts(const Contact *head) {
    FILE *fp = fopen(FILENAME, "w");
    if (!fp) {
        perror("Error saving contacts");
        return;
    }
    for (const Contact *cur = head; cur != NULL; cur = cur->next) {
        fprintf(fp, "%s|%s|%s\n", cur->name, cur->phone, cur->email);
    }
    fclose(fp);
}


static void print_menu(void) {
    printf("\n===== Contact Manager =====\n");
    printf("1. Add Contact\n");
    printf("2. View All Contacts\n");
    printf("3. Search Contact\n");
    printf("4. Delete Contact\n");
    printf("5. Save & Exit\n");
    printf("Choice: ");
}

int main(void) {
    Contact *head = load_contacts();
    printf("Loaded contacts from %s.\n", FILENAME);

    int choice;
    char name[NAME_LEN], phone[PHONE_LEN], email[EMAIL_LEN];

    do {
        print_menu();
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); continue; }
        while (getchar() != '\n'); 

        switch (choice) {
            case 1:
                printf("Name: ");  (void)fgets(name, NAME_LEN, stdin);  name[strcspn(name, "\n")] = '\0';
                printf("Phone: "); (void)fgets(phone, PHONE_LEN, stdin); phone[strcspn(phone, "\n")] = '\0';
                printf("Email: "); (void)fgets(email, EMAIL_LEN, stdin); email[strcspn(email, "\n")] = '\0';

                if (find_by_name(head, name)) {
                    printf("A contact named '%s' already exists.\n", name);
                } else {
                    head = insert_sorted(head, create_contact(name, phone, email));
                    printf("Contact added.\n");
                }
                break;

            case 2:
                print_all(head);
                break;

            case 3: {
                printf("Name to search: ");
                (void)fgets(name, NAME_LEN, stdin);
                name[strcspn(name, "\n")] = '\0';
                Contact *found = find_by_name(head, name);
                if (found) print_contact(found);
                else printf("No contact named '%s'.\n", name);
                break;
            }

            case 4: {
                printf("Name to delete: ");
                (void)fgets(name, NAME_LEN, stdin);
                name[strcspn(name, "\n")] = '\0';
                int found;
                head = delete_contact(head, name, &found);
                printf(found ? "Contact deleted.\n" : "No contact named '%s'.\n", name);
                break;
            }

            case 5:
                save_contacts(head);
                printf("Contacts saved. Goodbye!\n");
                break;

            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 5);

    free_all(head);
    return 0;
}
