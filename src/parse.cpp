#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// mkdir
#include <sys/types.h>
#include <sys/stat.h>

// ls
#include <dirent.h>

#define PSTR_SIZE 1024

typedef struct {
    int len;
    char data[PSTR_SIZE];
} PathString;

typedef enum {
    ENTRY_DIR,
    ENTRY_FILE,
} EntryType;

typedef struct {
    EntryType type;
    uint32_t len;
    uint32_t offset;
    uint32_t name_offset;
} Entry;

uint32_t get_u32(const uint8_t *data, int i) {
    uint32_t val = (data[i])
        + (data[i+1] << 8)
        + (data[i+2] << 16)
        + (data[i+3] << 24);
    return val;
}

uint8_t *read_file(const char *filename, long *out_len) {
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

size_t write_file(const char *filename, uint8_t *data, long len) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        perror("no file");
        fflush(0);
        /* printf("no file\n"); */
        return 0;
    }

    return fwrite(data, len, 1, f);
}

static void mkdir_recursive(const char *dir) {
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++)
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    mkdir(tmp, S_IRWXU);
}

char **list_dir(char *dir, int *out_len) {
    struct dirent *d;
    DIR *dh = opendir(dir);
    if (!dh) {
        *out_len = 0;
        return 0;
    }

    while ((d = readdir(dh)) != NULL) {
        // TODO
    }
}

Entry *extract_entries(const uint8_t *info_data, int offset, int count) {

    Entry *entries = (Entry*)malloc(sizeof(Entry) * count);

    for (int i = 0; i < count; i++) {

        int j = offset + i * 16;

        char ft[4] = {0};
        ft[0] = (char)info_data[j + 0];
        ft[1] = (char)info_data[j + 1];
        ft[2] = (char)info_data[j + 2];
        ft[3] = (char)info_data[j + 3];

        uint32_t v1 = get_u32(info_data, j + 4);
        uint32_t v2 = get_u32(info_data, j + 8);
        uint32_t count = get_u32(info_data, j + 12);

        Entry e;
        if (ft[0] == 'D' && ft[1] == 'I' && ft[2] == 'R' && ft[3] == 'Y') {
            e.type = ENTRY_DIR;
        } else if (ft[0] == 'F' && ft[1] == 'I' && ft[2] == 'L' && ft[3] == 'E') {
            e.type = ENTRY_FILE;
        }
        e.len = get_u32(info_data, j + 4);
        e.offset = get_u32(info_data, j + 8);
        e.name_offset = get_u32(info_data, j + 12);
        entries[i] = e;
    }

    return entries;
}

static char filename[255];
void process_entry(uint8_t *info_data, uint8_t *file_data, Entry *entries, int *i, int stack[16], int stack_pos) {
    Entry entry = entries[*i];

    stack[stack_pos] = entry.name_offset;
    stack_pos += 1;

    if (entry.type == ENTRY_DIR) {
        for (int j = 0; j < entry.len; j++) {
            int old_i = *i;
            *i = entry.offset + j;
            process_entry(info_data, file_data, entries, i, stack, stack_pos);
            *i = old_i + 1;
        }
    } else if (entry.type == ENTRY_FILE) {

        int fpos = 0;
        /* memset(filename, 0, 255); */
        for (int j = 0; j < stack_pos - 1; j++) {
            for (int c = stack[j]; info_data[c] != '\0'; c++) {
                filename[fpos] = info_data[c];
                fpos++;
            }
            filename[fpos] = '/';
            fpos++;
        }

        mkdir_recursive(filename);

        int j = stack_pos - 1;
        for (int c = stack[j]; info_data[c] != '\0'; c++) {
            filename[fpos] = info_data[c];
            fpos++;
        }
        filename[fpos] = '\0';

        printf("%s\n", filename);
        write_file(filename, &file_data[entry.offset], entry.len);
    }
}

int main(int argc, char **argv) {

    long file_data_len;
    long len;
    uint8_t *file_data = read_file("/home/paul/Downloads/li2/cabs/Game/_data/z01BPK.bob", &file_data_len);
    uint8_t *info_data = read_file("/home/paul/Downloads/li2/cabs/Game/_data/z01BPK.bod", &len);

    int i;
    i = len - 16;

    uint32_t file_offset = get_u32(info_data, len - 16);
    uint32_t file_count = get_u32(info_data, len - 16 + 4);
    uint32_t filename_offset = get_u32(info_data, len - 16 + 8);

    Entry *entries = extract_entries(info_data, file_offset, file_count);

    i = 0;
    int stack[16] = {0};
    process_entry(info_data, file_data, entries, &i, stack, 0);

    return 0;
}
