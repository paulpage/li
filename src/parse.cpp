#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define PSTR_SIZE 1024

typedef struct {
    int len;
    char data[PSTR_SIZE];
} PathString;

/* static PathString full_path = {0}; */
/* static PathString name = {0}; */

int pstr_add(PathString *dest, const char *data, int len) {
    int i;
    for (i = 0; i < len && dest->len + i < PSTR_SIZE - 1; i++) {
        dest->data[dest->len + i] = data[i];
    }
    dest->len += i;
    return i;
}

PathString pstr_from_cstr(const uint8_t *data, int *i) {
    PathString pstr = {0};
    int j;
    for (j = 0; data[*i + j] != '\0'; j++) {
        pstr.data[j] = (char)data[*i + j];
    }
    pstr.len = j;
    *i += j;
    return pstr;
}

uint32_t get_u32(const uint8_t *data, int *i) {
    uint32_t val = (data[*i])
        + (data[*i+1] << 8)
        + (data[*i+2] << 16)
        + (data[*i+3] << 24);
    *i += 4;
    return val;
}

uint8_t *read_entire_file(const char *filename, long *out_len) {
    FILE *f;
    uint8_t *buf;

    f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    *out_len = ftell(f);
    rewind(f);

    buf = (uint8_t*)malloc(*out_len * sizeof(uint8_t));
    fread(buf, *out_len, 1, f);
    fclose(f);
    return buf;
}

void extract_file(const uint8_t *data, int *i, PathString *full_path) {

    /* PathString full_path = {0}; */

    /* pstr_add(&full_path, name.data, name.len); */
    /* pstr_add(&full_path, "/", 1); */

    char ft[4] = {0};
    ft[0] = (char)data[*i + 0];
    ft[1] = (char)data[*i + 1];
    ft[2] = (char)data[*i + 2];
    ft[3] = (char)data[*i + 3];
    *i += 4;

    if (ft[0] == 'D' && ft[1] == 'I' && ft[2] == 'R' && ft[3] == 'Y') {
        printf("DIRY\n");

        uint32_t count = get_u32(data, i);
        printf("  count: %d\n", count);
        uint32_t unknown = get_u32(data, i); // unused
        printf("  unknown: %d\n", unknown);
        uint32_t folder_name_offset = get_u32(data, i);
        printf("  folder_name_offset: %d\n", folder_name_offset);

        int file_offset = *i;

        *i = folder_name_offset;

        PathString name = pstr_from_cstr(data, i);
        printf("  name: [%s]\n", name.data);


        *i = file_offset;

        for (int j = 0; j < count; j++) {
            PathString new_full_path = {0};
            pstr_add(&new_full_path, full_path->data, full_path->len);
            pstr_add(full_path, name.data, name.len);
            pstr_add(full_path, "/", 1);
            extract_file(data, i, &new_full_path);
        }

    } else if (ft[0] == 'F' && ft[1] == 'I' && ft[2] == 'L' && ft[3] == 'E') {
        /* printf("FILE\n"); */

        // TODO
        uint32_t out_file_size = get_u32(data, i);
        uint32_t out_file_offset = get_u32(data, i);
        uint32_t filename_offset = get_u32(data, i);

        int file_offset = *i;

        *i = filename_offset;
        PathString filename = pstr_from_cstr(data, i);
        *i = file_offset;
        PathString full_name = *full_path;
        pstr_add(&full_name, filename.data, filename.len);
        // TODO write file
        printf("FILE [%s] [%s] offset %d len %d\n", filename.data, full_name.data, out_file_offset, out_file_size);
    }
}

// actual file offset: 2de4

int main(int argc, char **argv) {

    long len;
    uint8_t *data = read_entire_file("/home/paul/Downloads/li2/cabs/Game/_data/z01BPK.bod", &len);

    printf("len: %ld\n", len);

    int i;
    i = len - 16;

    uint32_t file_offset = get_u32(data, &i);
    printf("file_offset: %d\n", file_offset);
    uint32_t file_count = get_u32(data, &i);
    printf("file_count: %d\n", file_count);
    uint32_t filename_offset = get_u32(data, &i);
    printf("filename_offset: %d\n", filename_offset);

    i = file_offset;

    PathString full_path = {0};

    extract_file(data, &i, &full_path);

    return 0;
}
