#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FLT_MAX 340282346638528859811704183484516925440.0f

Model load_obj(const char *filename) {
    Model model = {0};
    model.meshCount = 1;
    model.materialCount = 0;
    model.meshes = (Mesh*)calloc(model.meshCount, sizeof(Mesh));
    model.meshMaterial = (int*)calloc(model.meshCount, sizeof(int));

    Mesh *m = &(model.meshes[0]);

    // Read the file to a buffer
    FILE *f;
    f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    size_t file_len = (size_t)ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buffer;
    buffer = (char *) malloc(file_len + 1);
    file_len = fread(buffer, 1, file_len, f);
    buffer[file_len] = 0;
    fclose(f);

    int vertex_i = 0, tex_coord_i = 0, normal_i = 0, triangle_i = 0;

    // Search through the buffer for 'f' strings at the beginning of lines to
    // see how many faces there are
    bool search = true;
    for (int i = 0; i < file_len - 3; i++) {
        if (search) {
            if (search && buffer[i] == 'f' && buffer[i + 1] == ' ') {
                m->triangleCount++;
            }
            search = false;
        }

        if (buffer[i] == '\n') {
            search = true;
        }
    }
    m->vertexCount = m->triangleCount * 3;

    int vertex_count = 0, texcoord_count = 0, normal_count = 0;
    float *vertices = (float*)calloc(m->triangleCount * 3 * 3, sizeof(float));
    float *texcoords = (float*)calloc(m->triangleCount * 3 * 2, sizeof(float));
    float *normals = (float*)calloc(m->triangleCount * 3 * 3, sizeof(float));

    m->vertices = (float*)calloc(m->vertexCount * 3, sizeof(float));
    m->texcoords = (float*)calloc(m->vertexCount * 2, sizeof(float));
    m->normals = (float*)calloc(m->vertexCount * 3, sizeof(float));
    m->vboId = (unsigned int *)calloc( 7, sizeof(unsigned int));

    int dbg_i = 0; // TODO remove

    char line[256];
    char number_buf[64];
    int offset = 0;
    for (;;) {

        // Get the line
        memset(line, 0, 256);
        bool end = false;
        int i = 0;
        for (;;) {
            if (buffer[offset] == '\n') {
                offset++;
                break;
            } else if (buffer[offset] == '\r') {
                // skip
            } else if (buffer[offset] == '\0') {
                offset++;
                end = true;
                break;
            } else {
                line[i] = buffer[offset];
            }
            i++;
            offset++;
        }
        if (end) {
            break;
        }

        if (line == NULL) break;
        
        if (line[0] == 'v') {
            float numbers[3] = { 0 };
            int i = 0;
            for (int n = 0; n < 3; n++) {
                int j = 0;
                while ((line[i] < '0' || line[i] > '9') && line[i] != '-') i++;
                while (j < 128 - 1 && (line[i] == '.' || line[i] == '-'
                            || (line[i] >= '0' && line[i] <= '9'))) {
                    number_buf[j] = line[i];
                    i++; j++;
                }
                number_buf[j] = 0;

                numbers[n] = atof(number_buf);
            }

            if (line[0] == 'v' && line[1] == ' ') {
                vertices[vertex_i] = numbers[0]; vertex_i++;
                vertices[vertex_i] = numbers[1]; vertex_i++;
                vertices[vertex_i] = numbers[2]; vertex_i++;
                vertex_count++;
            } else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
                texcoords[tex_coord_i] = numbers[0]; tex_coord_i++;
                texcoords[tex_coord_i] = numbers[1]; tex_coord_i++;
                texcoord_count++;
            } else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
                normals[normal_i] = numbers[0]; normal_i++;
                normals[normal_i] = numbers[1]; normal_i++;
                normals[normal_i] = numbers[2]; normal_i++;
                normal_count++;
            }

        } else if (line[0] == 'f') {

            // Find the line length
            int eol = 0;
            while (line[eol] != '\n' && line[eol] != 0) eol++;

            // Find out how many numbers we have
            int number_count = 0;
            bool have_delimiter = false;
            for (int i = 0; i < eol; i++) {
                if (line[i] >= '0' && line[i] <= '9') {
                    if (have_delimiter) {
                        number_count++;
                        have_delimiter = false;
                    }
                } else {
                    have_delimiter = true;
                }
            }

            // TODO we may want to add quad or more than quad support here.
            // For now we assume the form 'f p1/t1/n1 p2/t2/n2 p3/t3/n3'
            int numbers[12];

            int i = 0;
            for (int n = 0; n < number_count; n++) {
                int j = 0;
                while ((line[i] < '0' || line[i] > '9') && line[i] != '-') i++;
                while (j < 128 - 1 && (line[i] == '.' || line[i] == '-'
                            || (line[i] >= '0' && line[i] <= '9'))) {
                    number_buf[j] = line[i];
                    i++; j++;
                }
                number_buf[j] = 0;
                numbers[n] = atoi(number_buf);
            }

            if (dbg_i < 20) {
                dbg_i++;
            }

            for (int n = 0; n < 3; n++) {
                m->vertices[triangle_i * 3 + 0] = vertices[(numbers[n * 3] - 1) * 3 + 0];
                m->vertices[triangle_i * 3 + 1] = vertices[(numbers[n * 3] - 1) * 3 + 1];
                m->vertices[triangle_i * 3 + 2] = vertices[(numbers[n * 3] - 1) * 3 + 2];
                m->texcoords[triangle_i * 2 + 0] = texcoords[(numbers[n * 3] - 1) * 2 + 0];
                m->texcoords[triangle_i * 2 + 1] = texcoords[(numbers[n * 3] - 1) * 2 + 1];
                m->normals[triangle_i * 3 + 1] = normals[(numbers[n * 3] - 1) * 3 + 0];
                m->normals[triangle_i * 3 + 2] = normals[(numbers[n * 3] - 1) * 3 + 1];
                m->normals[triangle_i * 3 + 3] = normals[(numbers[n * 3] - 1) * 3 + 2];
                triangle_i++;
            }
        }
    }

    for (int i = 0; i < model.meshCount; i++) rlLoadMesh(&model.meshes[i], false);
    model.transform = MatrixIdentity();
    model.materialCount = 1;
    model.materials = (Material *)RL_CALLOC(model.materialCount, sizeof(Material));
    model.materials[0] = LoadMaterialDefault();
    if (model.meshMaterial == NULL) model.meshMaterial = (int *)RL_CALLOC(model.meshCount, sizeof(int));

    return model;
}

void write_obj(Model model, const char *filename) {
    Mesh *m = &(model.meshes[0]);
    FILE *objFile = fopen("texcoords.obj", "wt");

    fprintf(objFile, "# Vertex Count:     %i\n", m->vertexCount);
    fprintf(objFile, "# Triangle Count:   %i\n\n", m->triangleCount);
    fprintf(objFile, "g mesh\n");

    for (int i = 0, v = 0; i < m->vertexCount; i++, v += 3) {
        fprintf(objFile, "v %.3f %.3f %.3f\n", m->vertices[v], m->vertices[v + 1], m->vertices[v + 2]);
    }

    for (int i = 0, v = 0; i < m->vertexCount; i++, v += 2) {
        fprintf(objFile, "vt %.3f %.3f\n", m->texcoords[v], m->texcoords[v + 1]);
    }

    for (int i = 0, v = 0; i < m->vertexCount; i++, v += 3) {
        fprintf(objFile, "vn %.3f %.3f %.3f\n", m->normals[v], m->normals[v + 1], m->normals[v + 2]);
    }

    for (int i = 0; i < m->triangleCount * 3; i += 3) {
        fprintf(objFile, "f %i/%i/%i %i/%i/%i %i/%i/%i\n",
                i + 1, i + 1, i + 1, i + 2, i + 2, i + 2, i + 3, i + 3, i + 3
               );
    }

    fprintf(objFile, "\n");

    fclose(objFile);
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Island");

    // Set up the camera
    Camera camera = { 0 };
    camera.position = (Vector3){ 20.0f, 20.0f, 20.0f };
    camera.target = (Vector3){ 0.0f, 8.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.6f, 0.0f };
    camera.fovy = 45.0f;
    camera.type = CAMERA_PERSPECTIVE;

    Ray ray = {0};
    bool mouse_intercepted = false;

    // Set up the texture picker
    int tp_rows = 2;
    int tp_cols = 2;
    Rectangle tp_rect = {5, 5, 100, 100};
    Rectangle tp_selection = {0.0f, 0.0f, 0.5f, 0.5f};

    // Load the model
    Model original_model = load_obj("/home/paul/projects/lidata/z02LGI/meshes/land.obj");
    for (int i = 0; i < original_model.meshes[0].vertexCount; i++) {
        float u = original_model.meshes[0].texcoords[i*2];
        float v = original_model.meshes[0].texcoords[i*2+1];

        if (fabs(original_model.meshes[0].texcoords[i*2] - 1.0f) < 0.01f) {
            original_model.meshes[0].texcoords[i*2] = 0.5f;
        }
        if (fabs(original_model.meshes[0].texcoords[i*2+1] - 1.0f) < 0.01f) {
            original_model.meshes[0].texcoords[i*2+1] = 0.5f;
        }
    }
    float *original_texcoords = malloc(sizeof(float) * original_model.meshes[0].triangleCount * 3 * 2);
    for (int i = 0; i < original_model.meshes[0].triangleCount * 6; i++) {
        original_texcoords[i] = original_model.meshes[0].texcoords[i];
    }

    Model model = load_obj("texcoords.obj");
    Mesh *m = &(model.meshes[0]);
    float *saved_texcoords = malloc(sizeof(float) * m->triangleCount * 3 * 2);
    for (int i = 0; i < m->triangleCount * 6; i++) {
        saved_texcoords[i] = m->texcoords[i];
    }
    rlUpdateMesh(model.meshes[0], 1, m->vertexCount);

    /* Texture2D texture = LoadTexture("/home/paul/project/lidata/z04IFC/textures/text028.png"); */
    Texture2D texture = LoadTexture("out.png");
    SetTextureWrap(texture, WRAP_CLAMP);
    model.materials[0].maps[MAP_DIFFUSE].texture = texture;

    Vector3 modelPos = { 0.0f, 0.0f, 0.0f };
    BoundingBox modelBBox = MeshBoundingBox(model.meshes[0]);
    bool hitMeshBBox = false;
    bool hitTriangle = false;

    // Test triangle
    Vector3 ta = (Vector3){ -25.0, 0.5, 0.0 };
    Vector3 tb = (Vector3){ -4.0, 2.5, 1.0 };
    Vector3 tc = (Vector3){ -8.0, 6.5, 0.0 };

    Vector3 bary = { 0.0f, 0.0f, 0.0f };

    SetCameraMode(camera, CAMERA_FREE);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        UpdateCamera(&camera);

        // Display information about closest hit
        RayHitInfo nearestHit = { 0 };
        char *hitObjectName = "None";
        nearestHit.distance = FLT_MAX;
        nearestHit.hit = false;
        Color cursorColor = WHITE;

        // Get ray and test against ground, triangle, and mesh
        ray = GetMouseRay(GetMousePosition(), camera);

        // Check ray collision aginst ground plane
        RayHitInfo groundHitInfo = GetCollisionRayGround(ray, 0.0f);

        if ((groundHitInfo.hit) && (groundHitInfo.distance < nearestHit.distance)) {
            nearestHit = groundHitInfo;
            cursorColor = GREEN;
            hitObjectName = "Ground";
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), tp_rect)) {
            tp_selection.x = (float)((int)(GetMouseX() - tp_rect.x) / (int)(tp_rect.width / tp_cols)) / (float)tp_cols;
            tp_selection.y = (float)((int)(GetMouseY() - tp_rect.y) / (int)(tp_rect.height / tp_rows)) / (float)tp_rows;
            mouse_intercepted = true;
        }

        hitTriangle = false;
        int i;
        for (i = 0; i < m->triangleCount; i++) {
            ta = (Vector3){
                m->vertices[i * 9 + 0],
                m->vertices[i * 9 + 1],
                m->vertices[i * 9 + 2],
            };
            tb = (Vector3){
                m->vertices[i * 9 + 3],
                m->vertices[i * 9 + 4],
                m->vertices[i * 9 + 5],
            };
            tc = (Vector3){
                m->vertices[i * 9 + 6],
                m->vertices[i * 9 + 7],
                m->vertices[i * 9 + 8],
            };
            RayHitInfo triHitInfo = GetCollisionRayTriangle(ray, ta, tb, tc);
            if ((triHitInfo.hit) && (triHitInfo.distance < nearestHit.distance)) {
                nearestHit = triHitInfo;
                cursorColor = PURPLE;
                hitObjectName = "Triangle";
                bary = Vector3Barycenter(nearestHit.position, ta, tb, tc);
                hitTriangle = true;
                break;
            }
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            if (!mouse_intercepted && hitTriangle) {
                for (int j = 0; j < 3; j++) {
                    m->texcoords[i*6 + j*2 + 0] = original_texcoords[i*6 + j*2 + 0] + tp_selection.x;
                    m->texcoords[i*6 + j*2 + 1] = original_texcoords[i*6 + j*2 + 1] + tp_selection.y;
                }
                rlUpdateMesh(*m, 1, m->triangleCount * 3);
            }
        } else {
            mouse_intercepted = false;
        }

        hitMeshBBox = false;

        if (IsKeyPressed(KEY_E)) {
            write_obj(model, "texcoords.obj");
            printf("Exported.\n");

            model = load_obj("texcoords.obj");
            for (int i = 0; i < m->triangleCount * 6; i++) {
                saved_texcoords[i] = m->texcoords[i];
            }
            m = &(model.meshes[0]);
            model.materials[0].maps[MAP_DIFFUSE].texture = texture;
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        // Draw the model
        // WARNING: If scale is different than 1.0f,
        // not considered by GetCollisionRayModel()
        DrawModel(model, modelPos, 1.0f, WHITE);

        // Draw the test triangle
        DrawLine3D(ta, tb, PURPLE);
        DrawLine3D(tb, tc, PURPLE);
        DrawLine3D(tc, ta, PURPLE);

        // Draw the mesh bbox if we hit it
        if (hitMeshBBox) DrawBoundingBox(modelBBox, LIME);

        // If we hit something, draw the cursor at the hit point
        if (nearestHit.hit) {
            DrawCube(nearestHit.position, 0.3, 0.3, 0.3, cursorColor);
            DrawCubeWires(nearestHit.position, 0.3, 0.3, 0.3, RED);

            Vector3 normalEnd;
            normalEnd.x = nearestHit.position.x + nearestHit.normal.x;
            normalEnd.y = nearestHit.position.y + nearestHit.normal.y;
            normalEnd.z = nearestHit.position.z + nearestHit.normal.z;

            DrawLine3D(nearestHit.position, normalEnd, RED);
        }

        DrawRay(ray, MAROON);

        DrawGrid(10, 10.0f);

        EndMode3D();

        DrawTexturePro(
                texture,
                (Rectangle){0, 0, texture.width, texture.height},
                tp_rect,
                (Vector2){0.0f, 0.0f},
                0.0f,
                WHITE);
        DrawRectangleLines(
                tp_rect.x + tp_selection.x * tp_rect.width,
                tp_rect.y + tp_selection.y * tp_rect.height,
                tp_selection.width * tp_rect.width,
                tp_selection.height * tp_rect.height,
                RED);


        DrawText(TextFormat("Selection: %f, %f", tp_selection.x, tp_selection.y), 10, 400, 10, GRAY);
        DrawText("Use Mouse to Move Camera", 10, 430, 10, GRAY);
        DrawFPS(10, 10);

        EndDrawing();
    }

    /* UnloadModel(model); */
    UnloadTexture(texture);
    CloseWindow();
    return 0;
}
