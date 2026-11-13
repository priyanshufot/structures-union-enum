// Unions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int integer;
    float decimal;
    char text[20];
} StructData;

typedef union {
    int integer;
    float decimal;
    char text[20];
} UnionData;

typedef enum {
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_TEXT,
    VALUE_INVALID
} ValueType;

typedef struct {
    UnionData value;
    ValueType type;
} TaggedValue;

void set_integer(TaggedValue *record, int value) {
    record->value.integer = value;
    record->type = VALUE_INT;
}

void set_float(TaggedValue *record, float value) {
    record->value.decimal = value;
    record->type = VALUE_FLOAT;
}

int set_text(TaggedValue *record, const char *text) {
    if (record == NULL || text == NULL || strlen(text) >= sizeof(record->value.text)) {
        return 0;
    }

    strcpy(record->value.text, text);
    record->type = VALUE_TEXT;
    return 1;
}

void display_tagged_value(const TaggedValue *record) {
    if (record == NULL) {
        printf("Invalid record pointer.\n");
        return;
    }

    switch (record->type) {
        case VALUE_INT:
            printf("Integer: %d\n", record->value.integer);
            break;
        case VALUE_FLOAT:
            printf("Float: %.2f\n", record->value.decimal);
            break;
        case VALUE_TEXT:
            printf("Text: %s\n", record->value.text);
            break;
        default:
            printf("Invalid value type.\n");
    }
}

const char *type_name(ValueType type) {
    switch (type) {
        case VALUE_INT:
            return "integer";
        case VALUE_FLOAT:
            return "float";
        case VALUE_TEXT:
            return "text";
        default:
            return "invalid";
    }
}

void show_layout(void) {
    StructData structure_value;
    UnionData union_value;
    size_t structure_offset_int, structure_offset_float, structure_offset_text;
    size_t union_offset_int, union_offset_float, union_offset_text;

    structure_offset_int = (size_t)((char *)&structure_value.integer - (char *)&structure_value);
    structure_offset_float = (size_t)((char *)&structure_value.decimal - (char *)&structure_value);
    structure_offset_text = (size_t)((char *)&structure_value.text - (char *)&structure_value);
    union_offset_int = (size_t)((char *)&union_value.integer - (char *)&union_value);
    union_offset_float = (size_t)((char *)&union_value.decimal - (char *)&union_value);
    union_offset_text = (size_t)((char *)&union_value.text - (char *)&union_value);

    printf("\n--- Memory Layout ---\n");
    printf("sizeof(StructData): %zu bytes\n", sizeof(StructData));
    printf("_Alignof(StructData): %zu bytes\n", (size_t)_Alignof(StructData));
    printf("sizeof(UnionData): %zu bytes\n", sizeof(UnionData));
    printf("_Alignof(UnionData): %zu bytes\n", (size_t)_Alignof(UnionData));
    printf("sizeof(TaggedValue): %zu bytes\n", sizeof(TaggedValue));

    printf("\nStructure member addresses and offsets:\n");
    printf("integer: %p, offset %zu\n", (void *)&structure_value.integer, structure_offset_int);
    printf("decimal: %p, offset %zu\n", (void *)&structure_value.decimal, structure_offset_float);
    printf("text:    %p, offset %zu\n", (void *)structure_value.text, structure_offset_text);

    printf("\nUnion member addresses and offsets:\n");
    printf("integer: %p, offset %zu\n", (void *)&union_value.integer, union_offset_int);
    printf("decimal: %p, offset %zu\n", (void *)&union_value.decimal, union_offset_float);
    printf("text:    %p, offset %zu\n", (void *)union_value.text, union_offset_text);

    structure_value.integer = 10;
    structure_value.decimal = 220.5f;
    strcpy(structure_value.text, "Structure example");
    printf("\nStructure values: %d, %.1f, %s\n", structure_value.integer,
           structure_value.decimal, structure_value.text);

    union_value.integer = 10;
    printf("Union integer: %d\n", union_value.integer);
    union_value.decimal = 220.5f;
    printf("Union float: %.1f\n", union_value.decimal);
    strcpy(union_value.text, "Union example");
    printf("Union text: %s\n", union_value.text);

    printf("\nMemory for 100 structure records: %zu bytes\n", 100 * sizeof(StructData));
    printf("Memory for 100 union records: %zu bytes\n", 100 * sizeof(UnionData));
    printf("Offsets are calculated from the start of each object. Padding may be added to meet alignment.\n");
}

int main(void) {
    TaggedValue records[5];
    TaggedValue invalid_record;
    int i;

    show_layout();

    set_integer(&records[0], 10);
    set_float(&records[1], 220.5f);
    if (!set_text(&records[2], "Union Programming")) {
        printf("Could not store record 3.\n");
        return 1;
    }
    set_integer(&records[3], 42);
    if (!set_text(&records[4], "FCP Assignment")) {
        printf("Could not store record 5.\n");
        return 1;
    }

    printf("\n--- Tagged Records ---\n");
    for (i = 0; i < 5; i++) {
        printf("Record %d (%s): ", i + 1, type_name(records[i].type));
        display_tagged_value(&records[i]);
    }

    printf("\n--- Type Changes ---\n");
    set_integer(&records[0], 100);
    display_tagged_value(&records[0]);
    if (set_text(&records[0], "Changed to text")) {
        display_tagged_value(&records[0]);
    }
    set_float(&records[0], 9.75f);
    display_tagged_value(&records[0]);

    if (set_text(&records[4], "1234567890123456789")) {
        printf("A 19-character string was accepted: ");
        display_tagged_value(&records[4]);
    }
    if (!set_text(&records[2], "This text is longer than nineteen characters")) {
        printf("Long text was rejected as expected; the old value remains: ");
        display_tagged_value(&records[2]);
    }

    invalid_record.type = VALUE_INVALID;
    printf("\nInvalid tag check: ");
    display_tagged_value(&invalid_record);

    printf("\nObservation: union members share the same memory, so only one value is active at a time.\n");
    printf("A structure stores its members separately, so its size is usually larger.\n");
    printf("Actual sizes and padding depend on the compiler and platform.\n");

    return 0;
}
