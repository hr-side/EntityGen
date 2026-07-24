#include <corecrt_search.h>
#include <stdio.h>
#include <stdlib.h>
#include <cJSON.h>

char *open_JSON_file(const char *file_path) {\
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        fprintf(stderr, "Error: Could not Open file %s", file_path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buf = malloc(length + 1);
    if (buf) {
        fread(buf, 1, length, file);
        buf[length] = '\0';
    }

    fclose(file);
    return buf;
}

int parse() {
    char *file_raw = open_JSON_file("./Data.json");
    if (!file_raw) return 1;
    
    cJSON *json = cJSON_Parse(file_raw);

    return 0;
}
