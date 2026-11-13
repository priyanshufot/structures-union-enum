// Structures - 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_SIZE 50
#define MAX_CAPACITY 10000
#define LINE_SIZE 256

typedef struct {
    int roll_no;
    char name[NAME_SIZE];
    int age;
    float gpa;
} Student;

int read_line(const char *prompt, char *text, size_t size) {
    size_t length;
    int ch;

    printf("%s", prompt);
    if (fgets(text, (int)size, stdin) == NULL) {
        return 0;
    }

    length = strlen(text);
    if (length > 0 && text[length - 1] == '\n') {
        text[length - 1] = '\0';
    } else {
        ch = getchar();
        if (ch != '\n' && ch != EOF) {
            while (ch != '\n' && ch != EOF) {
                ch = getchar();
            }
            printf("Input is too long. Please try again.\n");
            return -1;
        }
    }

    return 1;
}

int parse_whole_number(const char *text, long *value) {
    const char *cursor = text;
    long result = 0;
    int digits = 0;

    while (*cursor == ' ' || *cursor == '\t') {
        cursor++;
    }
    if (*cursor == '+') {
        cursor++;
    }
    while (*cursor >= '0' && *cursor <= '9') {
        int digit = *cursor - '0';
        if (result > (2147483647L - digit) / 10L) {
            return 0;
        }
        result = result * 10L + digit;
        digits++;
        cursor++;
    }
    while (*cursor == ' ' || *cursor == '\t' || *cursor == '\r') {
        cursor++;
    }
    if (digits == 0 || *cursor != '\0') {
        return 0;
    }
    *value = result;
    return 1;
}

int read_long_value(const char *prompt, long *value) {
    char line[LINE_SIZE];
    int status = read_line(prompt, line, sizeof(line));

    if (status != 1) {
        return status;
    }
    if (!parse_whole_number(line, value)) {
        printf("Please enter a valid whole number between 0 and 2147483647.\n");
        return -1;
    }

    return 1;
}

double read_decimal_value(const char *prompt, int *status) {
    char line[LINE_SIZE];
    char *end;
    double value;

    *status = read_line(prompt, line, sizeof(line));
    if (*status != 1) {
        return 0.0;
    }

    value = strtod(line, &end);
    while (*end == ' ' || *end == '\t' || *end == '\r') {
        end++;
    }
    if (line[0] == '\0' || *end != '\0' || value != value || value > 1.0e100 || value < -1.0e100) {
        printf("Please enter a valid number.\n");
        *status = -1;
        return 0.0;
    }

    return value;
}

int find_student(const Student *students, int count, int roll_no) {
    int i;

    for (i = 0; i < count; i++) {
        if (students[i].roll_no == roll_no) {
            return i;
        }
    }

    return -1;
}

int grow_students(Student **students, int *capacity) {
    int new_capacity;
    Student *temp;

    if (*capacity >= MAX_CAPACITY) {
        printf("The student list has reached its limit.\n");
        return 0;
    }

    new_capacity = (*capacity == 0) ? 2 : *capacity * 2;
    if (new_capacity > MAX_CAPACITY) {
        new_capacity = MAX_CAPACITY;
    }
    if ((size_t)new_capacity > ((size_t)-1) / sizeof(Student)) {
        printf("Requested size is too large.\n");
        return 0;
    }

    temp = realloc(*students, (size_t)new_capacity * sizeof(Student));
    if (temp == NULL) {
        printf("Memory allocation failed. Existing records are safe.\n");
        return 0;
    }

    *students = temp;
    *capacity = new_capacity;
    return 1;
}

int add_student(Student **students, int *count, int *capacity) {
    long roll, age;
    double gpa;
    char name[LINE_SIZE];
    int status;

    if (read_long_value("Roll number: ", &roll) != 1 || roll < 1 || roll > 2147483647L) {
        printf("Roll number must be a positive integer.\n");
        return 0;
    }
    if (find_student(*students, *count, (int)roll) != -1) {
        printf("That roll number already exists.\n");
        return 0;
    }

    status = read_line("Student name: ", name, sizeof(name));
    if (status != 1 || name[0] == '\0' || strlen(name) >= NAME_SIZE) {
        printf("Name must contain 1 to 49 characters.\n");
        return 0;
    }

    if (read_long_value("Age (16-100): ", &age) != 1 || age < 16 || age > 100) {
        printf("Age must be between 16 and 100.\n");
        return 0;
    }

    gpa = read_decimal_value("GPA (0.00-4.00): ", &status);
    if (status != 1 || gpa < 0.0 || gpa > 4.0) {
        printf("GPA must be between 0.00 and 4.00.\n");
        return 0;
    }

    if (*count == *capacity && !grow_students(students, capacity)) {
        return 0;
    }

    (*students)[*count].roll_no = (int)roll;
    strcpy((*students)[*count].name, name);
    (*students)[*count].age = (int)age;
    (*students)[*count].gpa = (float)gpa;
    (*count)++;
    printf("Student %ld added successfully.\n", roll);

    return 1;
}

void display_students(const Student *students, int count) {
    int i;

    if (count == 0) {
        printf("No student records found.\n");
        return;
    }

    printf("\n%-10s %-25s %-8s %-6s\n", "Roll No.", "Name", "Age", "GPA");
    for (i = 0; i < count; i++) {
        printf("%-10d %-25s %-8d %-6.2f\n", students[i].roll_no,
               students[i].name, students[i].age, students[i].gpa);
    }
}

void search_student(const Student *students, int count) {
    long roll;
    int index;

    if (read_long_value("Enter roll number to search: ", &roll) != 1 || roll < 1 || roll > 2147483647L) {
        printf("Invalid roll number.\n");
        return;
    }

    index = find_student(students, count, (int)roll);
    if (index == -1) {
        printf("Student not found.\n");
    } else {
        printf("Roll: %d | Name: %s | Age: %d | GPA: %.2f\n",
               students[index].roll_no, students[index].name,
               students[index].age, students[index].gpa);
    }
}

void update_student(Student *students, int count) {
    long roll, age;
    double gpa;
    char name[LINE_SIZE];
    int index, status;

    if (read_long_value("Enter roll number to update: ", &roll) != 1 || roll < 1 || roll > 2147483647L) {
        printf("Invalid roll number.\n");
        return;
    }
    index = find_student(students, count, (int)roll);
    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    status = read_line("New name: ", name, sizeof(name));
    if (status != 1 || name[0] == '\0' || strlen(name) >= NAME_SIZE) {
        printf("Name must contain 1 to 49 characters.\n");
        return;
    }
    if (read_long_value("New age (16-100): ", &age) != 1 || age < 16 || age > 100) {
        printf("Age must be between 16 and 100.\n");
        return;
    }
    gpa = read_decimal_value("New GPA (0.00-4.00): ", &status);
    if (status != 1 || gpa < 0.0 || gpa > 4.0) {
        printf("GPA must be between 0.00 and 4.00.\n");
        return;
    }

    strcpy(students[index].name, name);
    students[index].age = (int)age;
    students[index].gpa = (float)gpa;
    printf("Student %ld updated successfully.\n", roll);
}

void delete_student(Student *students, int *count) {
    long roll;
    int index, i;

    if (read_long_value("Enter roll number to delete: ", &roll) != 1 || roll < 1 || roll > 2147483647L) {
        printf("Invalid roll number.\n");
        return;
    }
    index = find_student(students, *count, (int)roll);
    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    for (i = index; i < *count - 1; i++) {
        students[i] = students[i + 1];
    }
    (*count)--;
    printf("Student %ld deleted successfully.\n", roll);
}

void sort_students(Student *students, int count) {
    int i, j;

    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (students[j].gpa < students[j + 1].gpa ||
                (students[j].gpa == students[j + 1].gpa && students[j].roll_no > students[j + 1].roll_no)) {
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
    printf("Records sorted by GPA (highest first). The main list is reordered.\n");
}

void show_gpa_summary(const Student *students, int count) {
    int i, highest = 0, lowest = 0;
    double total = 0.0;

    if (count == 0) {
        printf("No GPA summary is available for an empty list.\n");
        return;
    }

    for (i = 0; i < count; i++) {
        total += students[i].gpa;
        if (students[i].gpa > students[highest].gpa) {
            highest = i;
        }
        if (students[i].gpa < students[lowest].gpa) {
            lowest = i;
        }
    }

    printf("Highest GPA: %s (%.2f)\n", students[highest].name, students[highest].gpa);
    printf("Lowest GPA: %s (%.2f)\n", students[lowest].name, students[lowest].gpa);
    printf("Average GPA: %.2f\n", total / count);
}

void show_menu(void) {
    printf("\n--- Student Records ---\n");
    printf("1. Add student\n2. Display students\n3. Search by roll number\n");
    printf("4. Update student\n5. Delete student\n6. GPA summary\n");
    printf("7. Sort by GPA\n0. Exit\n");
}

int main(void) {
    Student *students;
    int count = 0, capacity;
    long initial_capacity, choice;

    if (read_long_value("Initial capacity (1-10000): ", &initial_capacity) != 1 ||
        initial_capacity < 1 || initial_capacity > MAX_CAPACITY) {
        printf("Invalid initial capacity.\n");
        return 1;
    }

    capacity = (int)initial_capacity;
    if ((size_t)capacity > ((size_t)-1) / sizeof(Student)) {
        printf("Requested size is too large.\n");
        return 1;
    }
    students = malloc((size_t)capacity * sizeof(Student));
    if (students == NULL) {
        printf("Could not allocate the student list.\n");
        return 1;
    }

    do {
        show_menu();
        if (read_long_value("Choose an option: ", &choice) != 1) {
            printf("Input ended.\n");
            break;
        }

        switch (choice) {
            case 1:
                add_student(&students, &count, &capacity);
                break;
            case 2:
                display_students(students, count);
                break;
            case 3:
                search_student(students, count);
                break;
            case 4:
                update_student(students, count);
                break;
            case 5:
                delete_student(students, &count);
                break;
            case 6:
                show_gpa_summary(students, count);
                break;
            case 7:
                sort_students(students, count);
                break;
            case 0:
                printf("Closing student records.\n");
                break;
            default:
                printf("Invalid menu option.\n");
        }
    } while (choice != 0);

    free(students);
    return 0;
}
