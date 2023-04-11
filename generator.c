/*Andrew Waugaman
 *CSC 352
 *This file has a main function that creates an stl file called output.stl
 *that contains a very basic 3d object made of a fractal, a sphere, a cube,
 *and a pyramid. It then destroys the scene to prevent memory leaks.
 */

#include "3d.h"

int main(){
	Scene3D* scene = Scene3D_create();
        Coordinate3D origin = (Coordinate3D) {0, 0, 0};
        Scene3D_add_fractal(scene, origin, 20, 3);
	Scene3D_add_sphere(scene, origin, 15, 20);
	Coordinate3D cubeOrigin = (Coordinate3D) {0, 0, 20};
	Scene3D_add_cuboid(scene, cubeOrigin, 5, 5, 5);
	Coordinate3D pyramidOrigin = (Coordinate3D) {0, 0, 22.5};
	Scene3D_add_pyramid(scene, pyramidOrigin, 5, 5, "up");
        Scene3D_write_stl_binary(scene, "output.stl");
        Scene3D_destroy(scene);
}