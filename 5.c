// Structures - 2
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RECTANGLES 1000
#define LINE_SIZE 256
#define MAX_DIMENSION 1000000000000.0
#define SQUARE_TOLERANCE 0.000001

typedef struct {
    int id;
    double width;
    double height;
    double area;
    double perimeter;
} Rectangle;

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
            printf("Input is too long.\n");
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
        printf("Enter a valid whole number between 0 and 2147483647.\n");
        return -1;
    }

    return 1;
}

double read_number(const char *prompt, int *status) {
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
    if (line[0] == '\0' || *end != '\0' || value != value ||
        value <= 0.0 || value > MAX_DIMENSION) {
        printf("Enter a finite positive number (up to 1e12).\n");
        *status = -1;
        return 0.0;
    }

    return value;
}

void update_geometry(Rectangle *rectangle) {
    rectangle->area = rectangle->width * rectangle->height;
    rectangle->perimeter = 2.0 * (rectangle->width + rectangle->height);
}

int find_rectangle(const Rectangle *items, int count, int id) {
    int i;

    for (i = 0; i < count; i++) {
        if (items[i].id == id) {
            return i;
        }
    }

    return -1;
}

int grow_rectangles(Rectangle **items, int *capacity) {
    int new_capacity;
    Rectangle *temp;

    if (*capacity >= MAX_RECTANGLES) {
        printf("Rectangle limit reached.\n");
        return 0;
    }
    new_capacity = (*capacity == 0) ? 2 : *capacity * 2;
    if (new_capacity > MAX_RECTANGLES) {
        new_capacity = MAX_RECTANGLES;
    }
    if ((size_t)new_capacity > ((size_t)-1) / sizeof(Rectangle)) {
        printf("Requested size is too large.\n");
        return 0;
    }

    temp = realloc(*items, (size_t)new_capacity * sizeof(Rectangle));
    if (temp == NULL) {
        printf("Memory allocation failed. Existing rectangles are safe.\n");
        return 0;
    }
    *items = temp;
    *capacity = new_capacity;
    return 1;
}

int read_rectangle(Rectangle *rectangle) {
    long id;
    int status;
    double width, height;

    if (read_long_value("Rectangle ID: ", &id) != 1 || id < 1 || id > 2147483647L) {
        printf("ID must be a positive integer.\n");
        return 0;
    }
    rectangle->id = (int)id;
    width = read_number("Width: ", &status);
    if (status != 1) {
        return 0;
    }
    height = read_number("Height: ", &status);
    if (status != 1) {
        return 0;
    }

    rectangle->width = width;
    rectangle->height = height;
    update_geometry(rectangle);
    return 1;
}

void add_rectangle(Rectangle **items, int *count, int *capacity) {
    Rectangle item;

    if (read_rectangle(&item) != 1) {
        return;
    }
    if (find_rectangle(*items, *count, item.id) != -1) {
        printf("That rectangle ID already exists.\n");
        return;
    }
    if (*count == *capacity && !grow_rectangles(items, capacity)) {
        return;
    }

    (*items)[*count] = item;
    (*count)++;
    printf("Rectangle %d added.\n", item.id);
}

void display_rectangles(const Rectangle *items, int count) {
    int i;

    if (count == 0) {
        printf("No rectangles in the inventory.\n");
        return;
    }

    printf("\n%-8s %-12s %-12s %-12s %-12s\n", "ID", "Width", "Height", "Area", "Perimeter");
    for (i = 0; i < count; i++) {
        printf("%-8d %-12.2f %-12.2f %-12.2f %-12.2f\n", items[i].id,
               items[i].width, items[i].height, items[i].area, items[i].perimeter);
    }
}

void search_rectangle(const Rectangle *items, int count) {
    long id;
    int index;

    if (read_long_value("ID to search: ", &id) != 1 || id < 1 || id > 2147483647L) {
        printf("Invalid ID.\n");
        return;
    }
    index = find_rectangle(items, count, (int)id);
    if (index == -1) {
        printf("Rectangle not found.\n");
    } else {
        printf("ID %d: %.2f x %.2f, area %.2f, perimeter %.2f\n",
               items[index].id, items[index].width, items[index].height,
               items[index].area, items[index].perimeter);
    }
}

void update_rectangle(Rectangle *items, int count) {
    long id;
    int index, status;
    double width, height;

    if (read_long_value("ID to update: ", &id) != 1 || id < 1 || id > 2147483647L) {
        printf("Invalid ID.\n");
        return;
    }
    index = find_rectangle(items, count, (int)id);
    if (index == -1) {
        printf("Rectangle not found.\n");
        return;
    }
    width = read_number("New width: ", &status);
    if (status != 1) {
        return;
    }
    height = read_number("New height: ", &status);
    if (status != 1) {
        return;
    }

    items[index].width = width;
    items[index].height = height;
    update_geometry(&items[index]);
    printf("Rectangle %ld updated.\n", id);
}

void delete_rectangle(Rectangle *items, int *count) {
    long id;
    int index, i;

    if (read_long_value("ID to delete: ", &id) != 1 || id < 1 || id > 2147483647L) {
        printf("Invalid ID.\n");
        return;
    }
    index = find_rectangle(items, *count, (int)id);
    if (index == -1) {
        printf("Rectangle not found.\n");
        return;
    }
    for (i = index; i < *count - 1; i++) {
        items[i] = items[i + 1];
    }
    (*count)--;
    printf("Rectangle %ld deleted.\n", id);
}

int is_square(const Rectangle *rectangle) {
    double difference = rectangle->width - rectangle->height;
    double larger = rectangle->width > rectangle->height ? rectangle->width : rectangle->height;

    if (difference < 0.0) {
        difference = -difference;
    }
    return difference <= SQUARE_TOLERANCE * larger;
}

void show_statistics(const Rectangle *items, int count) {
    int i, largest = 0, smallest = 0;
    double total = 0.0;

    if (count == 0) {
        printf("No statistics are available for an empty inventory.\n");
        return;
    }
    for (i = 0; i < count; i++) {
        total += items[i].area;
        if (items[i].area > items[largest].area) {
            largest = i;
        }
        if (items[i].area < items[smallest].area) {
            smallest = i;
        }
    }

    printf("Largest area: ID %d (%.2f)\n", items[largest].id, items[largest].area);
    printf("Smallest area: ID %d (%.2f)\n", items[smallest].id, items[smallest].area);
    printf("Total area: %.2f\nAverage area: %.2f\n", total, total / count);
}

void show_classification(const Rectangle *items, int count) {
    int i;

    for (i = 0; i < count; i++) {
        printf("ID %d: %s\n", items[i].id, is_square(&items[i]) ? "Square" : "Non-square");
    }
}

void sort_rectangles(Rectangle *items, int count) {
    int i, j;

    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (items[j].area > items[j + 1].area ||
                (items[j].area == items[j + 1].area && items[j].id > items[j + 1].id)) {
                Rectangle temp = items[j];
                items[j] = items[j + 1];
                items[j + 1] = temp;
            }
        }
    }
    printf("Inventory sorted by area (smallest first). The main list is reordered.\n");
}

void filter_rectangles(const Rectangle *items, int count) {
    char line[LINE_SIZE];
    char *end;
    double threshold;
    int i, found = 0;

    if (read_line("Show areas greater than: ", line, sizeof(line)) != 1) {
        return;
    }
    threshold = strtod(line, &end);
    while (*end == ' ' || *end == '\t' || *end == '\r') {
        end++;
    }
    if (line[0] == '\0' || *end != '\0' || threshold != threshold || threshold > 1.0e100 || threshold < -1.0e100) {
        printf("Invalid threshold.\n");
        return;
    }

    for (i = 0; i < count; i++) {
        if (items[i].area > threshold) {
            printf("ID %d: %.2f x %.2f, area %.2f\n", items[i].id,
                   items[i].width, items[i].height, items[i].area);
            found = 1;
        }
    }
    if (!found) {
        printf("No rectangle has an area above that threshold.\n");
    }
}

void show_menu(void) {
    printf("\n--- Rectangle Inventory ---\n");
    printf("1. Add\n2. Display\n3. Search\n4. Update\n5. Delete\n");
    printf("6. Statistics\n7. Square check\n8. Sort by area\n9. Filter by area\n0. Exit\n");
}

int main(void) {
    Rectangle *items;
    long initial_count, choice;
    int count, capacity, i;

    if (read_long_value("Initial number of rectangles (1-1000): ", &initial_count) != 1 ||
        initial_count < 1 || initial_count > MAX_RECTANGLES) {
        printf("Invalid number of rectangles.\n");
        return 1;
    }
    count = 0;
    capacity = (int)initial_count;
    if ((size_t)capacity > ((size_t)-1) / sizeof(Rectangle)) {
        printf("Requested size is too large.\n");
        return 1;
    }
    items = malloc((size_t)capacity * sizeof(Rectangle));
    if (items == NULL) {
        printf("Could not allocate rectangle inventory.\n");
        return 1;
    }

    for (i = 0; i < capacity; i++) {
        Rectangle item;
        printf("\nRectangle %d of %d\n", i + 1, capacity);
        if (read_rectangle(&item) != 1) {
            printf("Could not read this rectangle. Starting with the records entered so far.\n");
            break;
        }
        if (find_rectangle(items, count, item.id) != -1) {
            printf("Duplicate ID. This rectangle was skipped.\n");
            i--;
            continue;
        }
        items[count++] = item;
    }

    do {
        show_menu();
        if (read_long_value("Choose an option: ", &choice) != 1) {
            printf("Input ended.\n");
            break;
        }
        switch (choice) {
            case 1:
                add_rectangle(&items, &count, &capacity);
                break;
            case 2:
                display_rectangles(items, count);
                break;
            case 3:
                search_rectangle(items, count);
                break;
            case 4:
                update_rectangle(items, count);
                break;
            case 5:
                delete_rectangle(items, &count);
                break;
            case 6:
                show_statistics(items, count);
                break;
            case 7:
                if (count == 0) {
                    printf("No rectangles to classify.\n");
                } else {
                    show_classification(items, count);
                }
                break;
            case 8:
                sort_rectangles(items, count);
                break;
            case 9:
                filter_rectangles(items, count);
                break;
            case 0:
                printf("Closing rectangle inventory.\n");
                break;
            default:
                printf("Invalid menu option.\n");
        }
    } while (choice != 0);

    free(items);
    return 0;
}
