#ifndef LINALG_H
#define LINALG_H

#include "glad/glad.h"
#include <stdio.h>
#include <math.h>

typedef GLfloat Vector3[3];
typedef GLfloat Vector4[4];
typedef Vector4 Matrix4[4];

void Matrix4_print(Matrix4 m) {
    for (unsigned int x = 0; x < 4; x++) {
        for (unsigned int y = 0; y < 4; y++) {
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

#endif
