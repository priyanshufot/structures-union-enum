#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TYPE_EMPTY,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING
} DataType;

typedef union {
    int integer_value;
    float float_value;
    char char_value;
    char *string_value;
} Data;

typedef struct {
    Data value;
    DataType type;
} Container;

void container_init(Container *container) {
    if (container != NULL) {
        container->value.string_value = NULL;
        container->type = TYPE_EMPTY;
    }
}

void container_clear(Container *container) {
    if (container == NULL) {
        return;
    }
    if (container->type == TYPE_STRING) {
        free(container->value.string_value);
    }
    container->value.string_value = NULL;
    container->type = TYPE_EMPTY;
}

int container_set_int(Container *container, int value) {
    if (container == NULL) {
        return 0;
    }
    container_clear(container);
    container->value.integer_value = value;
    container->type = TYPE_INT;
    return 1;
}

int container_set_float(Container *container, float value) {
    if (container == NULL) {
        return 0;
    }
    container_clear(container);
    container->value.float_value = value;
    container->type = TYPE_FLOAT;
    return 1;
}

int container_set_char(Container *container, char value) {
    if (container == NULL) {
        return 0;
    }
    container_clear(container);
    container->value.char_value = value;
    container->type = TYPE_CHAR;
    return 1;
}

int container_set_string(Container *container, const char *text) {
    size_t length;
    char *copy;

    if (container == NULL || text == NULL) {
        return 0;
    }
    length = strlen(text);
    if (length == (size_t)-1) {
        return 0;
    }
    copy = malloc(length + 1);
    if (copy == NULL) {
        return 0;
    }
    memcpy(copy, text, length + 1);

    container_clear(container);
    container->value.string_value = copy;
    container->type = TYPE_STRING;
    return 1;
}

int container_get_int(const Container *container, int *out_value) {
    if (container == NULL || out_value == NULL || container->type != TYPE_INT) {
        return 0;
    }
    *out_value = container->value.integer_value;
    return 1;
}

int container_get_float(const Container *container, float *out_value) {
    if (container == NULL || out_value == NULL || container->type != TYPE_FLOAT) {
        return 0;
    }
    *out_value = container->value.float_value;
    return 1;
}

int container_get_char(const Container *container, char *out_value) {
    if (container == NULL || out_value == NULL || container->type != TYPE_CHAR) {
        return 0;
    }
    *out_value = container->value.char_value;
    return 1;
}

int container_get_string(const Container *container, const char **out_value) {
    if (container == NULL || out_value == NULL || container->type != TYPE_STRING) {
        return 0;
    }
    *out_value = container->value.string_value;
    return 1;
}

const char *container_type_name(const Container *container) {
    if (container == NULL) {
        return "invalid";
    }
    switch (container->type) {
        case TYPE_EMPTY:
            return "empty";
        case TYPE_INT:
            return "integer";
        case TYPE_FLOAT:
            return "float";
        case TYPE_CHAR:
            return "character";
        case TYPE_STRING:
            return "string";
        default:
            return "invalid";
    }
}

void container_print(const Container *container) {
    if (container == NULL) {
        printf("Invalid container.\n");
        return;
    }

    switch (container->type) {
        case TYPE_EMPTY:
            printf("(empty)");
            break;
        case TYPE_INT:
            printf("%d", container->value.integer_value);
            break;
        case TYPE_FLOAT:
            printf("%.2f", container->value.float_value);
            break;
        case TYPE_CHAR:
            printf("'%c'", container->value.char_value);
            break;
        case TYPE_STRING:
            printf("\"%s\"", container->value.string_value);
            break;
        default:
            printf("(invalid type)");
    }
}

int container_copy(Container *destination, const Container *source) {
    Container temporary;
    int success = 1;

    if (destination == NULL || source == NULL) {
        return 0;
    }
    if (destination == source) {
        return 1;
    }

    container_init(&temporary);
    switch (source->type) {
        case TYPE_EMPTY:
            break;
        case TYPE_INT:
            success = container_set_int(&temporary, source->value.integer_value);
            break;
        case TYPE_FLOAT:
            success = container_set_float(&temporary, source->value.float_value);
            break;
        case TYPE_CHAR:
            success = container_set_char(&temporary, source->value.char_value);
            break;
        case TYPE_STRING:
            success = container_set_string(&temporary, source->value.string_value);
            break;
        default:
            success = 0;
    }

    if (!success) {
        container_clear(&temporary);
        return 0;
    }

    container_clear(destination);
    *destination = temporary;
    return 1;
}

int main(void) {
    Container items[6];
    Container copied_string;
    int i, integer_value;
    float float_value;
    char char_value;
    const char *string_value;

    for (i = 0; i < 6; i++) {
        container_init(&items[i]);
    }
    container_init(&copied_string);

    container_set_int(&items[0], 10);
    container_set_float(&items[1], 3.14f);
    container_set_char(&items[2], 'A');
    if (!container_set_string(&items[3], "Hello, World!")) {
        printf("Could not allocate the first string.\n");
        goto cleanup;
    }
    container_set_int(&items[4], 0);
    if (!container_set_string(&items[5], "Data")) {
        printf("Could not allocate the second string.\n");
        goto cleanup;
    }

    printf("--- Container List ---\n");
    for (i = 0; i < 6; i++) {
        printf("%d. %-10s: ", i, container_type_name(&items[i]));
        container_print(&items[i]);
        printf("\n");
    }

    printf("\n--- Type-safe Getters ---\n");
    if (container_get_int(&items[0], &integer_value)) {
        printf("Integer getter: %d\n", integer_value);
    }
    if (!container_get_int(&items[1], &integer_value)) {
        printf("Reading an integer from a float container was rejected.\n");
    }
    if (container_get_float(&items[1], &float_value)) {
        printf("Float getter: %.2f\n", float_value);
    }
    if (container_get_char(&items[2], &char_value)) {
        printf("Character getter: %c\n", char_value);
    }
    if (container_get_string(&items[3], &string_value)) {
        printf("String getter: %s\n", string_value);
        if (container_set_string(&items[3], string_value)) {
            printf("Replacing a string using its own stored pointer is safe: ");
            container_print(&items[3]);
            printf("\n");
        }
    }

    printf("\n--- Deep Copy ---\n");
    if (container_copy(&copied_string, &items[5])) {
        container_set_string(&items[5], "Changed original");
        printf("Original: ");
        container_print(&items[5]);
        printf("\nCopy: ");
        container_print(&copied_string);
        printf("\n");
    } else {
        printf("Could not copy the string container.\n");
    }

    printf("\n--- Type Changes ---\n");
    container_set_string(&items[0], "First string");
    container_print(&items[0]);
    printf(" -> ");
    container_set_int(&items[0], 25);
    container_print(&items[0]);
    printf(" -> ");
    container_set_string(&items[0], "Second string");
    container_print(&items[0]);
    printf(" -> ");
    container_set_float(&items[0], 7.5f);
    container_print(&items[0]);
    printf("\n");

    if (container_copy(&items[4], &items[4])) {
        printf("Self-copy was handled safely.\n");
    }
    container_clear(&items[4]);
    container_clear(&items[4]);
    printf("Clearing an empty container more than once is safe.\n");

cleanup:
    for (i = 0; i < 6; i++) {
        container_clear(&items[i]);
    }
    container_clear(&copied_string);
    return 0;
}
