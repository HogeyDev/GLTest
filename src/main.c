#include "glad/glad.h"
#include "linalg.h"
#include "shader.h"
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_stdinc.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static unsigned int window_width = 1280;
static unsigned int window_height = 720;
static const char *window_name = "Cool shaders idk";

void destroy_window(SDL_Window **window, SDL_GLContext *gl_context) {
    SDL_GL_DeleteContext(*gl_context);
    SDL_DestroyWindow(*window);
    SDL_Quit();
}

void print_opengl_debug_info(void) {
    printf("VENDOR:\t\t%s\n", glGetString(GL_VENDOR));
    printf("RENDERER:\t%s\n", glGetString(GL_RENDERER));
    printf("VERSION:\t%s\n", glGetString(GL_VERSION));
    printf("SHADER\t\tLANGUAGE: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void initialize_rendering(SDL_Window **window, SDL_GLContext *gl_context) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16); // 16xMSAA
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8); // 8xMSAA
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // 4xMSAA

    SDL_SetRelativeMouseMode(SDL_TRUE);

    *window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_OPENGL);
    *gl_context = SDL_GL_CreateContext(*window);
    if (gl_context == 0) {
        fprintf(stderr, "Could not create SDL_GLContext");
    }

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        fprintf(stderr, "Could not initialize Glad");
    }

    print_opengl_debug_info();
}

char *read_file(const char *file_path) {
    FILE *fp = fopen(file_path, "r");
    if (!fp) {
        fprintf(stderr, "Could not open file: %s\n", file_path);
        exit(1);
    }

    size_t capacity = 1024;
    size_t length = 0;
    char *contents = malloc(capacity);
    if (!contents) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    char buf[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buf, 1, sizeof(buf), fp)) > 0) {
        if (length + bytes_read >= capacity) {
            capacity *= 2;
            contents = realloc(contents, capacity);
            if (!contents) {
                fprintf(stderr, "Memory reallocation failed\n");
                exit(1);
            }
        }
        memcpy(contents + length, buf, bytes_read);
        length += bytes_read;
    }

    contents[length] = '\0';

    fclose(fp);
    return contents;
}

int main(void) {
    SDL_Window *window;
    SDL_GLContext gl_context;
    initialize_rendering(&window, &gl_context);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("./res/wall.jpg", &width, &height, &nrChannels, 0);

    const char *vertex_shader = read_file("./src/shaders/vertex.glsl");
    const char *fragment_shader = read_file("./src/shaders/fragment.glsl");
    Shader shader = Shader_create_program(vertex_shader, fragment_shader);

    const GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f, -1.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 1.0f,

        -0.5f, -0.5f, 0.5f, -1.0f, -1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, -1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 1.0f,
    };
    const GLuint indices[] = {
        0, 1, 3,
        1, 2, 3,

        4, 7, 5,
        5, 7, 6,

        0, 3, 4,
        4, 3, 7,

        1, 5, 2,
        5, 6, 2,

        3, 2, 7,
        2, 6, 7,

        0, 4, 1,
        1, 4, 5,
    };
    GLfloat vertices_with_normals[8 * 8] = { 0 };
    for (unsigned int i = 0; i < sizeof(indices) / sizeof(indices[0]); i += 3) {
        GLuint i0 = indices[i];
        GLuint i1 = indices[i + 1];
        GLuint i2 = indices[i + 2];

        Vector3 p0 = { vertices[i0 * 5], vertices[i0 * 5 + 1], vertices[i0 * 5 + 2] };
        Vector3 p1 = { vertices[i1 * 5], vertices[i1 * 5 + 1], vertices[i1 * 5 + 2] };
        Vector3 p2 = { vertices[i2 * 5], vertices[i2 * 5 + 1], vertices[i2 * 5 + 2] };

        Vector3 e0 = { p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2] };
        Vector3 e1 = { p2[0] - p0[0], p2[1] - p0[1], p2[2] - p0[2] };

        Vector3 normal;
        Vector3_cross(normal, e0, e1);
        Vector3_normalize(normal);

        for (int j = 0; j < 3; j++) {
            GLuint idx = indices[i + j];
            vertices_with_normals[idx * 8] = vertices[idx * 5];
            vertices_with_normals[idx * 8 + 1] = vertices[idx * 5 + 1];
            vertices_with_normals[idx * 8 + 2] = vertices[idx * 5 + 2];
            vertices_with_normals[idx * 8 + 3] = vertices[idx * 5 + 3];
            vertices_with_normals[idx * 8 + 4] = vertices[idx * 5 + 4];
            vertices_with_normals[idx * 8 + 5] = normal[0];
            vertices_with_normals[idx * 8 + 6] = normal[1];
            vertices_with_normals[idx * 8 + 7] = normal[2];
        }
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_with_normals), vertices_with_normals, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8, (void *)(5 * sizeof(GLfloat)));

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    Uint64 last_frame_time = SDL_GetPerformanceCounter();
    unsigned int frame_counter = 0;

    Matrix4 uTransform;
    Matrix4 uProjection;
    Matrix4 uView;

    Matrix4_identity(uTransform);
    Matrix4_perspective(uProjection, 80.0f * 3.1415f / 180.0f, (float)window_width / window_height, 0.01f, 1000.0f);
    Matrix4_identity(uView);

    int shift = 0, space = 0;
    int left = 0, right = 0;
    int down = 0, up = 0;
    Vector3 camera_position = { 0 };
    camera_position[2] = -2;
    float camera_pitch = 0, camera_yaw = 0;
    int fullscreen = 0;
    SDL_Event e;
    while (1) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_q:
                            destroy_window(&window, &gl_context);
                            exit(0);
                            break;
                        case SDLK_F11:
                            fullscreen = !fullscreen;
                            SDL_SetWindowFullscreen(window, (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
                            SDL_DisplayMode display_mode;
                            SDL_GetWindowDisplayMode(window, &display_mode);
                            window_width = display_mode.w;
                            window_height = display_mode.h;
                            glViewport(0, 0, window_width, window_height);
                            break;
                        case SDLK_a: left = 1; break;
                        case SDLK_d: right = 1; break;
                        case SDLK_w: up = 1; break;
                        case SDLK_s: down = 1; break;
                        case SDLK_SPACE: space = 1; break;
                        case SDLK_LSHIFT: shift = 1; break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (e.key.keysym.sym) {
                        case SDLK_a: left = 0; break;
                        case SDLK_d: right = 0; break;
                        case SDLK_w: up = 0; break;
                        case SDLK_s: down = 0; break;
                        case SDLK_SPACE: space = 0; break;
                        case SDLK_LSHIFT: shift = 0; break;
                    }
                    break;
                case SDL_QUIT:
                    destroy_window(&window, &gl_context);
                    exit(0);
                    break;
            }
        
        }
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE); // not extremely necessary, just remember that it will remove all triangles facing away from the camera based on winding order.
        // glEnable(GL_MULTISAMPLE); // MSAA

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        float pitch_rad = camera_pitch * 3.1415f / 180.0f;
        float yaw_rad = camera_yaw * 3.1415f / 180.0f;
        Vector3 camera_direction;
        camera_direction[0] = -sinf(yaw_rad) * cosf(pitch_rad);
        camera_direction[1] = sinf(pitch_rad);
        camera_direction[2] = cosf(yaw_rad) * cosf(pitch_rad);
        Vector3_normalize(camera_direction);
        // printf("X: %.2f\nY: %.2f\nZ: %.2f\n\n", camera_direction[0], camera_direction[1], camera_direction[2]);

        Vector3 world_up = { 0.0f, 1.0f, 0.0f };
        Vector3 camera_right;
        Vector3_cross(camera_right, world_up, camera_direction);

        Vector3 movement = { 0 };
        movement[0] += camera_direction[0] * (float)(up - down);
        movement[2] += camera_direction[2] * (float)(up - down);

        movement[0] += camera_right[0] * (float)(right - left);
        movement[2] += camera_right[2] * (float)(right - left);

        movement[1] += (float)(space - shift);
        Vector3_normalize(movement);

        float camera_speed = 0.1f;
        Vector3_scale(movement, movement, camera_speed);
        Vector3_add(camera_position, camera_position, movement);

        int mouse_x, mouse_y;
        SDL_GetRelativeMouseState(&mouse_x, &mouse_y);
        SDL_WarpMouseInWindow(window, 0, 0);
        camera_yaw -= mouse_x * 0.2f;
        camera_pitch -= mouse_y * 0.2f;
        if (camera_yaw < 0) {
            camera_yaw = 360.0f;
        } else if (camera_yaw > 360.0f) {
            camera_yaw = 0.0f;
        }
        if (camera_pitch < -90.0f) {
            camera_pitch = -90.0f;
        } else if (camera_pitch > 90.0f) {
            camera_pitch = 90.0f;
        }

        Matrix4_identity(uTransform);
        Matrix4_rotate_x(uTransform, camera_pitch * 3.1415f / 180.0f);
        Matrix4_rotate_y(uTransform, camera_yaw * 3.1415f / 180.0f);

        Shader_set_uniform_vec3(shader, "uCameraPosition", camera_position);
        Shader_set_uniform_mat4(shader, "uTransform", uTransform);
        Shader_set_uniform_mat4(shader, "uProjection", uProjection);
        Shader_set_uniform_mat4(shader, "uView", uView);

        glUseProgram(shader.program);

        glBindTexture(GL_TEXTURE_2D, texture);

        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);

        if (++frame_counter % 60 == 0) {
            Uint64 current_frame_time = SDL_GetPerformanceCounter();
            float framerate = 60.0f * (float)SDL_GetPerformanceFrequency() / (current_frame_time - last_frame_time);
            last_frame_time = current_frame_time;
            printf("FPS: %.0f\n", framerate);
        }
    }
    
    return 0;
}
