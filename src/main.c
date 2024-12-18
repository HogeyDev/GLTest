#include "glad/glad.h"
#include "linalg.h"
#include "shader.h"
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <string.h>
#include <stdio.h>

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

    const char *vertex_shader = read_file("./src/shaders/vertex.glsl");
    const char *fragment_shader = read_file("./src/shaders/fragment.glsl");
    Shader shader = Shader_create_program(vertex_shader, fragment_shader);

    const GLfloat vertices[] = {
        -0.5f, -0.5f, -1.0f,
        0.5f, -0.5f, -1.0f,
        0.5f, 0.5f, -1.0f,
        -0.5f, 0.5f, -1.0f,
    };
    const GLuint indices[] = {
        0, 1, 3,
        1, 2, 3,
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Uint64 last_frame_time = SDL_GetPerformanceCounter();
    unsigned int frame_counter = 0;

    Matrix4 uTransform;
    Matrix4 uProjection;
    Matrix4 uView;

    Matrix4_identity(uTransform);
    Matrix4_perspective(uProjection, 80.0f * 3.1415f / 180.0f, (float)window_width / window_height, 0.1f, 1000.0f);
    Matrix4_identity(uView);

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
                        case SDLK_f:
                            fullscreen = !fullscreen;
                            SDL_SetWindowFullscreen(window, (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
                            SDL_DisplayMode display_mode;
                            SDL_GetWindowDisplayMode(window, &display_mode);
                            window_width = display_mode.w;
                            window_height = display_mode.h;
                            break;
                    }
                    break;
                case SDL_QUIT:
                    destroy_window(&window, &gl_context);
                    exit(0);
                    break;
            }
        
        }
        // glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE); // not extremely necessary, just remember that it will remove all triangles facing away from the camera based on winding order.
        // glEnable(GL_MULTISAMPLE); // MSAA

        glViewport(0, 0, window_width, window_height);
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        uTransform[3][0] += 0.01f;
        uTransform[3][1] += 0.01f;
        Shader_set_uniform_mat4(shader, "uTransform", uTransform);
        Shader_set_uniform_mat4(shader, "uProjection", uProjection);
        Shader_set_uniform_mat4(shader, "uView", uView);

        glUseProgram(shader.program);

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
