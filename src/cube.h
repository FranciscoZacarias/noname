#ifndef CUBE_H
#define CUBE_H

typedef struct Cube {
	Mat4f32 transform;
	Vec3f32 color;
} Cube;

function Cube cube_create(Vec3f32 position, Vec3f32 color);
function Cube cube_translate(Cube cube, Vec3f32 translation);
function Cube cube_rotate(Cube cube, Vec3f32 axis, f32 radians);

#endif // CUBE_H