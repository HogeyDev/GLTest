#ifndef LINALG_H
#define LINALG_H

#include "glad/glad.h"
#include <stdio.h>
#include <math.h>

typedef GLfloat Vector3[3];
typedef GLfloat Vector4[4];
typedef Vector4 Matrix4[4];

void Matrix4_print(Matrix4 m) {
    for (unsigned int y = 0; y < 4; y++) {
        for (unsigned int x = 0; x < 4; x++) {
            printf("%.2f ", m[x][y]);
        }
        putchar('\n');
    }
}

void Matrix4_identity(Matrix4 m) {
    for (unsigned int x = 0; x < 4; x++) {
        for (unsigned int y = 0; y < 4; y++) {
            m[x][y] = x == y ? 1.0f : 0.0f;
        }
    }
}

void Matrix4_perspective(Matrix4 p, float fovy, float aspect, float zNear, float zFar) {
    Matrix4_identity(p);
    const float tanHalfFovy = tanf(fovy / 2.0f);

    p[0][0] = 1.0f / (aspect * tanHalfFovy);
    p[1][1] = 1.0f / (tanHalfFovy);
    p[2][2] = zFar / (zFar - zNear);
    p[2][3] = 1.0f;
    p[3][2] = -(zFar * zNear) / (zFar - zNear);
}

void Matrix4_multiply(Matrix4 m, Matrix4 a, Matrix4 b) {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            m[x][y] = 
                a[x][0] * b[0][y] +
                a[x][1] * b[1][y] +
                a[x][2] * b[2][y] +
                a[x][3] * b[3][y];
        }
    }
}

void Matrix4_rotate_x(Matrix4 m, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);

    Matrix4 rot;
    Matrix4_identity(rot);
    rot[1][1] = c;
    rot[2][1] = -s;
    rot[1][2] = s;
    rot[2][2] = c;

    Matrix4 result;
    Matrix4_multiply(result, rot, m);

    for (unsigned int y = 0; y < 4; y++) {
        for (unsigned int x = 0; x < 4; x++) {
            m[x][y] = result[x][y];
        }
    }
}

void Matrix4_rotate_y(Matrix4 m, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);

    Matrix4 rot;
    Matrix4_identity(rot);
    rot[0][0] = c;
    rot[2][0] = s;
    rot[0][2] = -s;
    rot[2][2] = c;

    Matrix4 result;
    Matrix4_multiply(result, rot, m);

    for (unsigned int y = 0; y < 4; y++) {
        for (unsigned int x = 0; x < 4; x++) {
            m[x][y] = result[x][y];
        }
    }
}

void Matrix4_rotate_z(Matrix4 m, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);

    Matrix4 rot;
    Matrix4_identity(rot);
    rot[0][0] = c;
    rot[1][0] = -s;
    rot[0][1] = s;
    rot[1][1] = c;

    Matrix4 result;
    Matrix4_multiply(result, rot, m);

    for (unsigned int y = 0; y < 4; y++) {
        for (unsigned int x = 0; x < 4; x++) {
            m[x][y] = result[x][y];
        }
    }
}

float Vector3_dot(Vector3 a, Vector3 b) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

void Vector3_cross(Vector3 v, Vector3 a, Vector3 b) {
    v[0] = a[1]*b[2] - a[2]*b[1];
    v[1] = a[2]*b[0] - a[0]*b[2];
    v[2] = a[0]*b[1] - a[1]*b[0];
}

void Vector3_normalize(Vector3 v) {
    float dot = Vector3_dot(v, v);
    if (dot == 0.0f) return;
    float len = sqrtf(dot);
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}

void Vector3_scale(Vector3 v, Vector3 a, float b) {
    a[0] *= b;
    a[1] *= b;
    a[2] *= b;
}

void Vector3_add(Vector3 v, Vector3 a, Vector3 b) {
    v[0] = a[0] + b[0];
    v[1] = a[1] + b[1];
    v[2] = a[2] + b[2];
}

#endif
