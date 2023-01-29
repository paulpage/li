#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    float x, y, z;
    uint8_t unknown[0x14];
} Position;

typedef struct {
    float x, y, z;
} Normal;

typedef struct {
    uint32_t index_offset;
    uint32_t index_count;
    uint32_t vert_offset;
    uint32_t vert_count;
} Group;

typedef struct {
    char VERT[4];
    uint32_t vert_count;
    Position *positions;
    Normal *normals;

    char INDL[4];
    uint32_t index_count;
    uint16_t *indices;

    char GRPL[4];
    uint32_t group_count;
    Group *groups;
} Mesh;

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

Mesh load_mesh(uint8_t *data, long len) {
    Mesh mesh = {0};

    int i = 0;

    // Positions and normals --------------------
    memcpy(mesh.VERT, &data[0], 4);
    i += 4;

    mesh.vert_count = get_u32(data, i);
    i += 4;

    Position *positions = (Position*)malloc(mesh.vert_count * sizeof(Position));
    Normal *normals = (Normal*)malloc(mesh.vert_count * sizeof(Normal));

    for (int j = 0; j < mesh.vert_count; j++) {
        memcpy(&positions[j], &data[i], sizeof(Position));
        i += sizeof(Position);
    }
    for (int j = 0; j < mesh.vert_count; j++) {
        memcpy(&normals[j], &data[i], sizeof(Normal));
        i += sizeof(Normal);
    }

    mesh.positions = positions;
    mesh.normals = normals;

    // Indices --------------------
    memcpy(mesh.INDL, &data[i], 4);
    i += 4;

    mesh.index_count = get_u32(data, i);
    i += 4;

    uint16_t *indices = (uint16_t*)malloc(mesh.index_count * sizeof(uint16_t));

    for (int j = 0; j < mesh.index_count; j++) {
        memcpy(&indices[j], &data[i], sizeof(uint16_t));
        i += sizeof(uint16_t);
    }

    mesh.indices = indices;

    // Groups --------------------
    memcpy(mesh.GRPL, &data[i], 4);
    i += 4;

    mesh.group_count = get_u32(data, i);
    i += 4;

    Group *groups = (Group*)malloc(mesh.group_count * sizeof(Group));

    for (int j = 0; j < mesh.group_count; j++) {
        memcpy(&groups[j], &data[i], sizeof(Group));
        i += sizeof(Group);
    }

    mesh.groups = groups;

    return mesh;
}

int main(int argc, char **argv) {

    long len = 0;
    uint8_t *data = read_file("_data/z02LGI/meshes/Ambulance_lod1.msh", &len);
    Mesh mesh = load_mesh(data, len);

    printf("Loaded model with %d verts, %d indices, %d groups\n", mesh.vert_count, mesh.index_count, mesh.group_count);

    return 0;
}
