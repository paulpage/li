#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Str {
    char *s;
    long len;
} Str;
#define S(s) (Str){(char*)s, sizeof(s)-1}

FILE *pjp_fopen(const char *filename, const char *mode) {
    FILE *f;
#if defined (_MSC_VER) && _MSC_VER >= 1400
    if (0 != fopen_s(&f, filename, mode)) {
        f = 0;
    }
#else
    f = fopen(filename, mode);
#endif
    return f;
}

Str read_file(const char *filename) {
    Str out;

    FILE *f = pjp_fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    out.len = ftell(f);
    rewind(f);

    out.s = (char*)malloc(out.len * sizeof(char));
    fread(out.s, out.len, 1, f);
    fclose(f);
    return out;
}

bool str_startswith(Str a, Str b) {
    int len = a.len < b.len ? a.len : b.len;
    for (int i = 0; i < len; i++) {
        if (a.s[i] != b.s[i]) {
            return false;
        }
    }
    return true;
}

Str str_off(Str s, int off) {
    return (Str){s.s+off, s.len-off};
}

Str str_cut(Str s, int start, int end) {
    return (Str){s.s+start, (end - start)};
}

void str_print(Str s) {
    fwrite(s.s, s.len, 1, stdout);
}

