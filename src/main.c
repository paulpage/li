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

    // Search through the buffer for 'v', 'vt', 'vn', and 'f' strings at the
    // beginning of lines to see how many of each object there are.
    bool search = true;
    for (int i = 0; i < file_len - 3; i++) {
        if (search) {
            if (buffer[i] == 'v' && buffer[i + 1] == ' ') {
                model.meshes[0].vertexCount++;
            } else if (buffer[i] == 'v' && buffer[i + 1] == 't' && buffer[i + 2] == ' ') {
            } else if (buffer[i] == 'v' && buffer[i + 1] == 'n' && buffer[i + 2] == ' ') {
            } else if (buffer[i] == 'f' && buffer[i + 1] == ' ') {
                model.meshes[0].triangleCount++;
            }
            search = false;
        }

        if (buffer[i] == '\n') {
            search = true;
        }
    }
    model.meshes[0].vertices = (float*)calloc(model.meshes[0].vertexCount * 3, sizeof(float));
    model.meshes[0].texcoords = (float*)calloc(model.meshes[0].vertexCount * 2, sizeof(float));
    model.meshes[0].normals = (float*)calloc(model.meshes[0].vertexCount * 3, sizeof(float));
    model.meshes[0].vboId = (unsigned int *)calloc(/*DEFAULT_MESH_VERTEX_BUFFERS*/ 7, sizeof(unsigned int));
    model.meshes[0].indices = (unsigned short*)calloc(model.meshes[0].triangleCount * 3 * 2, sizeof(unsigned short));

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
                while (j < 128 - 1 && (line[i] == '.' || line[i] == '-' || (line[i] >= '0' && line[i] <= '9'))) {
                    number_buf[j] = line[i];
                    i++; j++;
                }
                number_buf[j] = 0;

                numbers[n] = atof(number_buf);
            }

            if (line[0] == 'v' && line[1] == ' ') {
                model.meshes[0].vertices[vertex_i] = numbers[0]; vertex_i++;
                model.meshes[0].vertices[vertex_i] = numbers[1]; vertex_i++;
                model.meshes[0].vertices[vertex_i] = numbers[2]; vertex_i++;
            } else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
                model.meshes[0].texcoords[tex_coord_i] = numbers[0]; tex_coord_i++;
                model.meshes[0].texcoords[tex_coord_i] = numbers[1]; tex_coord_i++;
                /* model.meshes[0].texcoords[tex_coord_i] = numbers[2]; tex_coord_i++; */
            } else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
                model.meshes[0].normals[normal_i] = numbers[0]; normal_i++;
                model.meshes[0].normals[normal_i] = numbers[1]; normal_i++;
                model.meshes[0].normals[normal_i] = numbers[2]; normal_i++;
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
                while (j < 128 - 1 && (line[i] == '.' || line[i] == '-' || (line[i] >= '0' && line[i] <= '9'))) {
                    number_buf[j] = line[i];
                    i++; j++;
                }
                number_buf[j] = 0;
                numbers[n] = atoi(number_buf);
            }

            if (dbg_i < 20) {
                dbg_i++;
            }

            model.meshes[0].indices[triangle_i] = numbers[0] - 1; triangle_i++;
            model.meshes[0].indices[triangle_i] = numbers[3] - 1; triangle_i++;
            model.meshes[0].indices[triangle_i] = numbers[6] - 1; triangle_i++;
            if (number_count == 12) {
                model.meshes[0].indices[triangle_i] = numbers[6] - 1; triangle_i++;
                model.meshes[0].indices[triangle_i] = numbers[9] - 1; triangle_i++;
                model.meshes[0].indices[triangle_i] = numbers[0] - 1; triangle_i++;
                model.meshes[0].triangleCount++;
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

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Island");

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 20.0f, 20.0f, 20.0f };
    camera.target = (Vector3){ 0.0f, 8.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.6f, 0.0f };
    camera.fovy = 45.0f;
    camera.type = CAMERA_PERSPECTIVE;

    Ray ray = {0};


    /* Model model = load_obj("/home/paul/projects/lidata/z02LGI/meshes/land.obj"); */
    Model model = load_obj("texcoords.obj");
    for (int i = 0; i < model.meshes[0].vertexCount; i++) {
        float u = model.meshes[0].texcoords[i*2];
        float v = model.meshes[0].texcoords[i*2+1];

        if (fabs(model.meshes[0].texcoords[i*2] - 1.0f) < 0.01f) {
            model.meshes[0].texcoords[i*2] = 0.5f;
        }
        if (fabs(model.meshes[0].texcoords[i*2+1] - 1.0f) < 0.01f) {
            model.meshes[0].texcoords[i*2+1] = 0.5f;
        }
    }
    float *original_tex_coords = malloc(sizeof(float) * model.meshes[0].triangleCount * 3 * 2);
    for (int i = 0; i < model.meshes[0].triangleCount * 3; i++) {
        original_tex_coords[i + 0] = model.meshes[0].texcoords[model.meshes[0].indices[i + 0] * 2 + 0];
        original_tex_coords[i + 1] = model.meshes[0].texcoords[model.meshes[0].indices[i + 0] * 2 + 1];
        original_tex_coords[i + 2] = model.meshes[0].texcoords[model.meshes[0].indices[i + 1] * 2 + 0];
        original_tex_coords[i + 3] = model.meshes[0].texcoords[model.meshes[0].indices[i + 1] * 2 + 1];
        original_tex_coords[i + 4] = model.meshes[0].texcoords[model.meshes[0].indices[i + 2] * 2 + 0];
        original_tex_coords[i + 5] = model.meshes[0].texcoords[model.meshes[0].indices[i + 2] * 2 + 1];
    }
    rlUpdateMesh(model.meshes[0], 1, model.meshes[0].vertexCount);

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

    Mesh *m = &(model.meshes[0]);

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

        // Check ray collision against test triangle
        /* RayHitInfo triHitInfo = GetCollisionRayTriangle(ray, ta, tb, tc); */

        hitTriangle = false;
        int i;
        for (i = 0; i < m->triangleCount * 3; i += 3) {
            ta = (Vector3){
                m->vertices[m->indices[i + 0] * 3 + 0],
                m->vertices[m->indices[i + 0] * 3 + 1],
                m->vertices[m->indices[i + 0] * 3 + 2],
            };
            tb = (Vector3){
                m->vertices[m->indices[i + 1] * 3 + 0],
                m->vertices[m->indices[i + 1] * 3 + 1],
                m->vertices[m->indices[i + 1] * 3 + 2],
            };
            tc = (Vector3){
                m->vertices[m->indices[i + 2] * 3 + 0],
                m->vertices[m->indices[i + 2] * 3 + 1],
                m->vertices[m->indices[i + 2] * 3 + 2],
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
        if (hitTriangle && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            m->texcoords[m->indices[i + 0] * 2 + 0] += 0.5;
            m->texcoords[m->indices[i + 0] * 2 + 1] += 0.5;
            m->texcoords[m->indices[i + 1] * 2 + 0] += 0.5;
            m->texcoords[m->indices[i + 1] * 2 + 1] += 0.5;
            m->texcoords[m->indices[i + 2] * 2 + 0] += 0.5;
            m->texcoords[m->indices[i + 2] * 2 + 1] += 0.5;
            /* m->texcoords[m->indices[i + 0] * 2 + 0] = original_tex_coords[(i + 0) * 2 + 0] + 0.5; */
            /* m->texcoords[m->indices[i + 0] * 2 + 1] = original_tex_coords[(i + 0) * 2 + 1] + 0.5; */
            /* m->texcoords[m->indices[i + 1] * 2 + 0] = original_tex_coords[(i + 1) * 2 + 0] + 0.5; */
            /* m->texcoords[m->indices[i + 1] * 2 + 1] = original_tex_coords[(i + 1) * 2 + 1] + 0.5; */
            /* m->texcoords[m->indices[i + 2] * 2 + 0] = original_tex_coords[(i + 2) * 2 + 0] + 0.5; */
            /* m->texcoords[m->indices[i + 2] * 2 + 1] = original_tex_coords[(i + 2) * 2 + 1] + 0.5; */
            rlUpdateMesh(*m, 1, m->triangleCount * 3);
        }
        /* if ((triHitInfo.hit) && (triHitInfo.distance < nearestHit.distance)) { */
        /*     nearestHit = triHitInfo; */
        /*     cursorColor = PURPLE; */
        /*     hitObjectName = "Triangle"; */

        /*     bary = Vector3Barycenter(nearestHit.position, ta, tb, tc); */
        /*     hitTriangle = true; */
        /* } */
        /* else hitTriangle = false; */

        /* RayHitInfo meshHitInfo = {0}; */

        /* // Check ray collision against bounding box first, before trying the full ray-mesh test */
        /* if (CheckCollisionRayBox(ray, modelBBox)) { */
        /*     hitMeshBBox = true; */

        /*     // Check ray collision against model */
        /*     // NOTE: It considers model.transform matrix! */
        /*     meshHitInfo = GetCollisionRayModel(ray, model); */

        /*     if ((meshHitInfo.hit) && (meshHitInfo.distance < nearestHit.distance)) */
        /*     { */
        /*         nearestHit = meshHitInfo; */
        /*         cursorColor = ORANGE; */
        /*         hitObjectName = "Mesh"; */
        /*     } */
        /* } */

        hitMeshBBox = false;

        if (IsKeyPressed(KEY_E)) {

            for (int i = 0; i < 1; i++) {

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
                    fprintf(objFile, "f %u/%u/%u %u/%u/%u %u/%u/%u\n",
                        m->indices[i + 0] + 1, m->indices[i + 0] + 1, m->indices[i + 0] + 1,
                        m->indices[i + 1] + 1, m->indices[i + 1] + 1, m->indices[i + 1] + 1,
                        m->indices[i + 2] + 1, m->indices[i + 2] + 1, m->indices[i + 2] + 1
                    );
                }

                fprintf(objFile, "\n");

                fclose(objFile);
                printf("Exported.\n");

                model = load_obj("texcoords.obj");
                model.materials[0].maps[MAP_DIFFUSE].texture = texture;
            }
        }

        if (IsKeyPressed(KEY_R)) {
            model = load_obj("/home/paul/projects/lidata/z02LGI/meshes/land.obj");
        }
        model.materials[0].maps[MAP_DIFFUSE].texture = texture;

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

                    float shortest1 = FLT_MAX;
                    float shortest2 = FLT_MAX;
                    float shortest3 = FLT_MAX;
                    float shortest4 = FLT_MAX;
                    Vector3 s1 = {0};
                    Vector3 s2 = {0};
                    Vector3 s3 = {0};
                    Vector3 s4 = {0};
                    for (int i = 0; i < m->triangleCount * 3; i += 3) {
                        Vector3 point = (Vector3){
                            m->vertices[m->indices[i + 0] * 3 + 0],
                            m->vertices[m->indices[i + 0] * 3 + 1],
                            m->vertices[m->indices[i + 0] * 3 + 2],
                        };
                        float distance = Vector3Distance(point, nearestHit.position);
                        if (distance < shortest1) {
                            shortest4 = shortest3;
                            shortest3 = shortest2;
                            shortest2 = shortest1;
                            shortest1 = distance;
                            s4 = s3;
                            s3 = s2;
                            s2 = s1;
                            s1 = point;
                        } else if (distance < shortest2) {
                            shortest4 = shortest3;
                            shortest3 = shortest2;
                            shortest2 = distance;
                            s4 = s3;
                            s3 = s2;
                            s2 = point;
                        } else if (distance < shortest3) {
                            shortest4 = shortest3;
                            shortest3 = distance;
                            s4 = s3;
                            s3 = point;
                        } else if (distance < shortest4) {
                            shortest4 = distance;
                            s4 = point;
                        }
                    }

                    DrawSphere(s1, 2, RED);
                    DrawSphere(s2, 2, RED);
                    DrawSphere(s3, 2, RED);
                    DrawSphere(s4, 2, RED);

                    Vector3 normalEnd;
                    normalEnd.x = nearestHit.position.x + nearestHit.normal.x;
                    normalEnd.y = nearestHit.position.y + nearestHit.normal.y;
                    normalEnd.z = nearestHit.position.z + nearestHit.normal.z;

                    DrawLine3D(nearestHit.position, normalEnd, RED);
                }

                DrawRay(ray, MAROON);

                DrawGrid(10, 10.0f);

            EndMode3D();

            // Draw some debug GUI text
            DrawText(TextFormat("Hit Object: %s", hitObjectName), 10, 50, 10, BLACK);

            if (nearestHit.hit) {
                int ypos = 70;

                DrawText(TextFormat("Distance: %3.2f", nearestHit.distance), 10, ypos, 10, BLACK);

                DrawText(TextFormat("Hit Pos: %3.2f %3.2f %3.2f",
                                    nearestHit.position.x,
                                    nearestHit.position.y,
                                    nearestHit.position.z), 10, ypos + 15, 10, BLACK);

                DrawText(TextFormat("Hit Norm: %3.2f %3.2f %3.2f",
                                    nearestHit.normal.x,
                                    nearestHit.normal.y,
                                    nearestHit.normal.z), 10, ypos + 30, 10, BLACK);

                if (hitTriangle) DrawText(TextFormat("Barycenter: %3.2f %3.2f %3.2f",  bary.x, bary.y, bary.z), 10, ypos + 45, 10, BLACK);
            }

            DrawText("Use Mouse to Move Camera", 10, 430, 10, GRAY);

            DrawFPS(10, 10);

        EndDrawing();
    }

    /* UnloadModel(model); */
    UnloadTexture(texture);
    CloseWindow();
    return 0;
}
