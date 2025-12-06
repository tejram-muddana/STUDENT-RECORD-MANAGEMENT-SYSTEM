#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STUD_FILE "students.txt"
#define MAX_LOGIN_TRIES 3

typedef struct {
    int roll;
    char name[50];
    float mark;
} Student;

/* built-in users: username, password, role */
typedef struct {
    char user[32];
    char pass[32];
    char role[16];
} User;

User users_db[] = {
    {"admin", "admin123", "admin"},
    {"staff", "staff123", "staff"},
    {"guest", "guest123", "guest"}
};
const int users_count = sizeof(users_db) / sizeof(users_db[0]);

char activeUser[50];
char activeRole[20];

/* ---------------------- Login ---------------------- */
int attempt_login() {
    char inU[50], inP[50];

    printf("WELCOME TO THE STUDENT MANAGEMENT SYSTEM\n\n");

    for (int attempt = 1; attempt <= MAX_LOGIN_TRIES; ++attempt) {
        printf("--- Login Attempt %d of %d ---\n", attempt, MAX_LOGIN_TRIES);

        printf("USERNAME: ");
        if (scanf("%49s", inU) != 1) return 0;

        printf("PASSWORD: ");
        if (scanf("%49s", inP) != 1) return 0;

        for (int i = 0; i < users_count; ++i) {
            if (strcmp(inU, users_db[i].user) == 0 && strcmp(inP, users_db[i].pass) == 0) {
                strncpy(activeUser, users_db[i].user, sizeof(activeUser)-1);
                strncpy(activeRole, users_db[i].role, sizeof(activeRole)-1);
                activeUser[sizeof(activeUser)-1] = '\0';
                activeRole[sizeof(activeRole)-1] = '\0';
                printf("\nSuccessfully logged in as: %s (%s)\n", activeUser, activeRole);
                return 1;
            }
        }

        printf("\nInvalid username or password.\n\n");
    }

    printf("\nMaximum login attempts exceeded. Exiting.\n");
    return 0;
}

/* ---------------------- Student helpers ---------------------- */
void add_student() {
    Student s;
    printf("Roll: ");
    if (scanf("%d", &s.roll) != 1) return;
    printf("Name: ");
    scanf(" %49[^\n]", s.name);
    printf("Mark: ");
    if (scanf("%f", &s.mark) != 1) return;

    FILE *f = fopen(STUD_FILE, "a");
    if (!f) { printf("Cannot open student file for writing.\n"); return; }
    fprintf(f, "%d %s %.2f\n", s.roll, s.name, s.mark);
    fclose(f);

    printf("Student added!\n");
}

void display_all() {
    FILE *f = fopen(STUD_FILE, "r");
    if (!f) {
        printf("\n--- All Students ---\n");
        printf("Roll\tName\tMark\n");
        printf("----\t----\t----\n");
        printf("---------------------\n");
        printf("No student file!\n");
        return;
    }

    int roll;
    char name[50];
    float mark;

    printf("\n--- All Students ---\n");
    printf("Roll\tName\tMark\n");
    printf("----\t----\t----\n");
    while (fscanf(f, "%d %49s %f", &roll, name, &mark) == 3) {
        printf("%d\t%s\t%.2f\n", roll, name, mark);
    }
    printf("---------------------\n");
    fclose(f);
}

void search_by_roll() {
    int target;
    printf("Enter roll to search: ");
    if (scanf("%d", &target) != 1) return;

    FILE *f = fopen(STUD_FILE, "r");
    if (!f) { printf("No student file!\n"); return; }

    int roll;
    char name[50];
    float mark;
    int found = 0;
    while (fscanf(f, "%d %49s %f", &roll, name, &mark) == 3) {
        if (roll == target) {
            printf("\n--- Student Details ---\n");
            printf("Roll: %d\n", roll);
            printf("Name: %s\n", name);
            printf("Mark: %.2f\n", mark);
            printf("-----------------------\n");
            found = 1;
            break;
        }
    }
    fclose(f);
    if (!found) printf("Student with roll %d not found!\n", target);
}

void update_student() {
    int target;
    printf("Enter roll to update: ");
    if (scanf("%d", &target) != 1) return;

    FILE *f = fopen(STUD_FILE, "r");
    FILE *tmp = fopen("temp.txt", "w");
    if (!f || !tmp) { if(f) fclose(f); if(tmp) fclose(tmp); return; }

    int roll;
    char name[50];
    float mark;
    int updated = 0;

    while (fscanf(f, "%d %49s %f", &roll, name, &mark) == 3) {
        if (roll == target) {
            printf("New Name: ");
            scanf(" %49[^\n]", name);
            printf("New Mark: ");
            if (scanf("%f", &mark) != 1) mark = 0.0f;
            updated = 1;
        }
        fprintf(tmp, "%d %s %.2f\n", roll, name, mark);
    }

    fclose(f);
    fclose(tmp);
    remove(STUD_FILE);
    rename("temp.txt", STUD_FILE);

    if (updated) printf("Student updated!\n");
    else printf("Roll not found!\n");
}

void delete_student() {
    int target;
    printf("\nEnter roll to delete: ");
    if (scanf("%d", &target) != 1) return;

    FILE *f = fopen(STUD_FILE, "r");
    FILE *tmp = fopen("temp.txt", "w");
    if (!f || !tmp) { if(f) fclose(f); if(tmp) fclose(tmp); return; }

    int roll;
    char name[50];
    float mark;
    int removed = 0;

    while (fscanf(f, "%d %49s %f", &roll, name, &mark) == 3) {
        if (roll == target) {
            removed = 1;
            continue; /* skip writing this record */
        }
        fprintf(tmp, "%d %s %.2f\n", roll, name, mark);
    }

    fclose(f);
    fclose(tmp);
    remove(STUD_FILE);
    rename("temp.txt", STUD_FILE);

    if (removed) printf("Student deleted!\n");
    else printf("Roll not found!\n");
}

/* ---------------------- Menus ---------------------- */
void admin_menu() {
    while (1) {
        printf("\nADMIN MENU\n");
        printf("1.Add Student\n");
        printf("2.Display All Students\n");
        printf("3.Search Student by Roll\n");
        printf("4.Update Student\n");
        printf("5.Delete Student\n");
        printf("6.Logout\n");

        int choice;
        if (scanf("%d", &choice) != 1) { break; }

        switch (choice) {
            case 1: add_student(); break;
            case 2: display_all(); break;
            case 3: search_by_roll(); break;
            case 4: update_student(); break;
            case 5: delete_student(); break;
            default: return;
        }
    }
}

void staff_menu() {
    while (1) {
        printf("\nSTAFF MENU\n");
        printf("1.Add Student\n");
        printf("2.Display All Students\n");
        printf("3.Search Student by Roll\n");
        printf("4.Update Student\n");
        printf("5.Logout\n");

        int choice;
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1: add_student(); break;
            case 2: display_all(); break;
            case 3: search_by_roll(); break;
            case 4: update_student(); break;
            default: return;
        }
    }
}

void guest_menu() {
    while (1) {
        printf("\nGUEST MENU\n");
        printf("1.Display All Students\n");
        printf("2.Search Student by Roll\n");
        printf("3.Logout\n");

        int choice;
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1: display_all(); break;
            case 2: search_by_roll(); break;
            default: return;
        }
    }
}

/* ---------------------- Main ---------------------- */
int main(void) {
    if (!attempt_login()) {
        return 0;
    }

    if (strcmp(activeRole, "admin") == 0) admin_menu();
    else if (strcmp(activeRole, "staff") == 0) staff_menu();
    else guest_menu();

    printf("Logged out. Goodbye!\n");
    return 0;
}

