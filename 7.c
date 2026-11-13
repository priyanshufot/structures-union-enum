#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TASK_TEXT_SIZE 100
#define LINE_SIZE 256
#define MAX_TASKS 10000

typedef enum {
    TASK_PENDING,
    TASK_DONE
} TaskStatus;

typedef struct {
    int id;
    char description[TASK_TEXT_SIZE];
    TaskStatus status;
} Task;

typedef enum {
    CMD_ADD,
    CMD_DELETE,
    CMD_VIEW,
    CMD_UPDATE,
    CMD_COMPLETE,
    CMD_FIND,
    CMD_HELP,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

int read_line(const char *prompt, char *text, size_t size) {
    size_t length;
    int ch;

    if (prompt != NULL) {
        printf("%s", prompt);
    }
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
            printf("Input is too long.\n");
            return -1;
        }
    }

    return 1;
}

int parse_task_id(const char *text, int *id) {
    const char *cursor = text;
    long value = 0;
    int digits = 0;

    while (*cursor == ' ' || *cursor == '\t') {
        cursor++;
    }
    if (*cursor == '+') {
        cursor++;
    }
    while (*cursor >= '0' && *cursor <= '9') {
        int digit = *cursor - '0';
        if (value > (2147483647L - digit) / 10L) {
            return 0;
        }
        value = value * 10L + digit;
        digits++;
        cursor++;
    }
    while (*cursor == ' ' || *cursor == '\t' || *cursor == '\r') {
        cursor++;
    }
    if (digits == 0 || *cursor != '\0' || value < 1) {
        return 0;
    }
    *id = (int)value;
    return 1;
}

int read_id(const char *prompt, int *id) {
    char line[LINE_SIZE];

    if (read_line(prompt, line, sizeof(line)) != 1) {
        return 0;
    }
    if (!parse_task_id(line, id)) {
        printf("Enter a valid positive task ID.\n");
        return 0;
    }

    return 1;
}

char upper_ascii(char ch) {
    if (ch >= 'a' && ch <= 'z') {
        return (char)(ch - 'a' + 'A');
    }
    return ch;
}

int same_command(const char *left, const char *right) {
    while (*left != '\0' && *right != '\0') {
        if (upper_ascii(*left) != upper_ascii(*right)) {
            return 0;
        }
        left++;
        right++;
    }
    return *left == '\0' && *right == '\0';
}

CommandType command_from_text(const char *text) {
    if (same_command(text, "ADD")) {
        return CMD_ADD;
    }
    if (same_command(text, "DELETE")) {
        return CMD_DELETE;
    }
    if (same_command(text, "VIEW")) {
        return CMD_VIEW;
    }
    if (same_command(text, "UPDATE")) {
        return CMD_UPDATE;
    }
    if (same_command(text, "COMPLETE")) {
        return CMD_COMPLETE;
    }
    if (same_command(text, "FIND")) {
        return CMD_FIND;
    }
    if (same_command(text, "HELP")) {
        return CMD_HELP;
    }
    if (same_command(text, "EXIT")) {
        return CMD_EXIT;
    }
    return CMD_UNKNOWN;
}

int find_task(const Task *tasks, int count, int id) {
    int i;

    for (i = 0; i < count; i++) {
        if (tasks[i].id == id) {
            return i;
        }
    }
    return -1;
}

int grow_tasks(Task **tasks, int *capacity) {
    int new_capacity;
    Task *temp;

    if (*capacity >= MAX_TASKS) {
        printf("Task limit reached.\n");
        return 0;
    }
    new_capacity = (*capacity == 0) ? 2 : *capacity * 2;
    if (new_capacity > MAX_TASKS) {
        new_capacity = MAX_TASKS;
    }
    if ((size_t)new_capacity > ((size_t)-1) / sizeof(Task)) {
        printf("Requested size is too large.\n");
        return 0;
    }

    temp = realloc(*tasks, (size_t)new_capacity * sizeof(Task));
    if (temp == NULL) {
        printf("Could not grow the task list. Existing tasks are safe.\n");
        return 0;
    }
    *tasks = temp;
    *capacity = new_capacity;
    return 1;
}

void add_task(Task **tasks, int *count, int *capacity) {
    Task task;
    int status;

    if (!read_id("Task ID: ", &task.id)) {
        return;
    }
    if (find_task(*tasks, *count, task.id) != -1) {
        printf("That task ID already exists.\n");
        return;
    }

    status = read_line("Description: ", task.description, sizeof(task.description));
    if (status != 1 || task.description[0] == '\0') {
        printf("Description cannot be empty and must fit in 99 characters.\n");
        return;
    }
    task.status = TASK_PENDING;

    if (*count == *capacity && !grow_tasks(tasks, capacity)) {
        return;
    }
    (*tasks)[*count] = task;
    (*count)++;
    printf("Task %d added.\n", task.id);
}

void view_tasks(const Task *tasks, int count) {
    int i;

    if (count == 0) {
        printf("The task list is empty.\n");
        return;
    }
    printf("\n%-8s %-35s %-10s\n", "ID", "Description", "Status");
    for (i = 0; i < count; i++) {
        printf("%-8d %-35s %-10s\n", tasks[i].id, tasks[i].description,
               tasks[i].status == TASK_DONE ? "DONE" : "PENDING");
    }
}

void delete_task(Task *tasks, int *count) {
    int id, index, i;

    if (!read_id("Task ID to delete: ", &id)) {
        return;
    }
    index = find_task(tasks, *count, id);
    if (index == -1) {
        printf("Task %d was not found.\n", id);
        return;
    }
    for (i = index; i < *count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    (*count)--;
    printf("Task %d deleted.\n", id);
}

void update_task(Task *tasks, int count) {
    int id, index, status;
    char description[TASK_TEXT_SIZE];

    if (!read_id("Task ID to update: ", &id)) {
        return;
    }
    index = find_task(tasks, count, id);
    if (index == -1) {
        printf("Task %d was not found.\n", id);
        return;
    }
    status = read_line("New description: ", description, sizeof(description));
    if (status != 1 || description[0] == '\0') {
        printf("Description cannot be empty and must fit in 99 characters.\n");
        return;
    }
    strcpy(tasks[index].description, description);
    printf("Task %d updated.\n", id);
}

void complete_task(Task *tasks, int count) {
    int id, index;

    if (!read_id("Task ID to complete: ", &id)) {
        return;
    }
    index = find_task(tasks, count, id);
    if (index == -1) {
        printf("Task %d was not found.\n", id);
        return;
    }
    tasks[index].status = TASK_DONE;
    printf("Task %d marked as done.\n", id);
}

void find_and_show_task(const Task *tasks, int count) {
    int id, index;

    if (!read_id("Task ID to find: ", &id)) {
        return;
    }
    index = find_task(tasks, count, id);
    if (index == -1) {
        printf("Task %d was not found.\n", id);
    } else {
        printf("ID: %d | Description: %s | Status: %s\n", tasks[index].id,
               tasks[index].description, tasks[index].status == TASK_DONE ? "DONE" : "PENDING");
    }
}

void show_help(void) {
    printf("\nADD      - add a task\n");
    printf("DELETE   - delete a task by ID\n");
    printf("VIEW     - display all tasks\n");
    printf("UPDATE   - change a task description\n");
    printf("COMPLETE - mark a task as done\n");
    printf("FIND     - find a task by ID\n");
    printf("HELP     - show this help\n");
    printf("EXIT     - close the task manager\n");
}

int main(void) {
    Task *tasks = NULL;
    int count = 0, capacity = 0;
    int keep_running = 1;
    char command_text[LINE_SIZE];

    printf("Task Manager - type HELP to see commands.\n");
    while (keep_running) {
        CommandType command;
        int status = read_line("\nCommand: ", command_text, sizeof(command_text));

        if (status == 0) {
            printf("Input ended.\n");
            break;
        }
        if (status == -1) {
            continue;
        }

        command = command_from_text(command_text);
        switch (command) {
            case CMD_ADD:
                add_task(&tasks, &count, &capacity);
                break;
            case CMD_DELETE:
                delete_task(tasks, &count);
                break;
            case CMD_VIEW:
                view_tasks(tasks, count);
                break;
            case CMD_UPDATE:
                update_task(tasks, count);
                break;
            case CMD_COMPLETE:
                complete_task(tasks, count);
                break;
            case CMD_FIND:
                find_and_show_task(tasks, count);
                break;
            case CMD_HELP:
                show_help();
                break;
            case CMD_EXIT:
                printf("Task manager closed.\n");
                keep_running = 0;
                break;
            case CMD_UNKNOWN:
            default:
                printf("Unknown command. Type HELP to see available commands.\n");
        }
    }

    free(tasks);
    return 0;
}
