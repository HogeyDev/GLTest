#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"
#include "linalg.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    GLuint program;
} Shader;

GLuint Shader_compile_shader(GLuint type, const char *source) {
    GLuint shader_object;

    switch (type) {
        case GL_VERTEX_SHADER: shader_object = glCreateShader(GL_VERTEX_SHADER); break;
        case GL_FRAGMENT_SHADER: shader_object = glCreateShader(GL_FRAGMENT_SHADER); break;
        default:
            fprintf(stderr, "Unknown shader type: %d", type);
            exit(1);
    }

    glShaderSource(shader_object, 1, &source, (void *)0);
    glCompileShader(shader_object);

    GLint success;
    glGetShaderiv(shader_object, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader_object, 512, (void *)0, log);
        fprintf(stderr, "Shader compilation error: %s\n", log);
    }

    return shader_object;
}

Shader Shader_create_program(const char *vertex, const char *fragment) {
    Shader shader;

    GLuint program_object = glCreateProgram();

    GLuint vertex_shader = Shader_compile_shader(GL_VERTEX_SHADER, vertex);
    GLuint fragment_shader = Shader_compile_shader(GL_FRAGMENT_SHADER, fragment);

    glAttachShader(program_object, vertex_shader);
    glAttachShader(program_object, fragment_shader);
    glLinkProgram(program_object);

    GLint success;
    glGetProgramiv(program_object, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program_object, 512, (void *)0, log);
        fprintf(stderr, "Shader compilation error: %s\n", log);
    }

    shader.program = program_object;

    return shader;
}

void Shader_set_uniform_mat4(Shader shader, const char *name, Matrix4 value) {
    glUseProgram(shader.program);
    GLuint location = glGetUniformLocation(shader.program, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, (const GLfloat *)value);
    glUseProgram(0);
}

#endif
