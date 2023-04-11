/*Andrew Waugaman
 *CSC 352
 *This file has the implementation for the functions defined in 3d.h. They
 *allow you to create or destroy a scene, add cuboids, fractals, spheres,
 *or pyramids, or write the data to an stl file as ascii text or binary.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include "3d.h"

#define DEGTORAD PI/180

// This variable keeps track of the current tail of the scene. 
Triangle3DNode* tail = NULL;

Scene3D* Scene3D_create(){
        Scene3D* scene = malloc(sizeof(Scene3D));
        scene->count = 0;
        scene->root = NULL;
        return scene;
}

void Scene3D_destroy(Scene3D* scene){
        if(scene->root != NULL){
                Triangle3DNode* node = scene->root;
                Triangle3DNode* next = node->next;
                while(next != NULL){
                        free(node);
                        node = next;
                        next = next->next;
                }
                free(node);
        }
        free(scene);
}

void Scene3D_write_stl_text(Scene3D* scene, char* file_name){
        FILE* file = fopen(file_name, "w");
        Triangle3DNode* node = scene->root;
        fprintf(file, "solid scene\n");
        while(node != NULL){
                fprintf(file, "  facet normal 0.0 0.0 0.0\n    outer loop\n");
                fprintf(file, "      vertex %.5f %.5f %.5f\n", node->triangle.a.x, node->triangle.a.y, node->triangle.a.z);
                fprintf(file, "      vertex %.5f %.5f %.5f\n", node->triangle.b.x, node->triangle.b.y, node->triangle.b.z);
                fprintf(file, "      vertex %.5f %.5f %.5f\n", node->triangle.c.x, node->triangle.c.y, node->triangle.c.z);
                fprintf(file, "    endloop\n  endfacet\n");
                node = node->next;
        }
        fprintf(file, "endsolid scene\n");
        fclose(file);
}

void Scene3D_write_stl_binary(Scene3D* scene, char* file_name){
        FILE* file = fopen(file_name, "wb");
        uint16_t zeroi = 0;
        for (int i=0; i<40; i++){
                fwrite(&zeroi, sizeof(zeroi), 1, file);
        }
        uint32_t count = scene->count;
        fwrite(&count, sizeof(count), 1, file);
        float zerof = 0.0;
        Triangle3DNode* cur = scene->root;
        while(cur != NULL){                
		for (int j=0; j<3; j++){
                        fwrite(&zerof, sizeof(zerof), 1, file);
                }
                float ax = (float) cur->triangle.a.x;
                float ay = (float) cur->triangle.a.y;
                float az = (float) cur->triangle.a.z;
                float bx = (float) cur->triangle.b.x;
                float by = (float) cur->triangle.b.y;
                float bz = (float) cur->triangle.b.z;
                float cx = (float) cur->triangle.c.x;
                float cy = (float) cur->triangle.c.y;
                float cz = (float) cur->triangle.c.z;
		float coords[] = {ax, ay, az, bx, by, bz, cx, cy, cz};
                fwrite(coords, sizeof(coords), 1, file);
                fwrite(&zeroi, sizeof(zeroi), 1, file);
		cur = cur->next;
        }
	fclose(file);
}

/*Parameter scene: The scene to add a triangle to.
 *Parameter triangle: The triangle to add to the scene.
 *This method recurses to the end node of the current scene, creates a new Triangle3DNode
 *containing the triangle, and increases the count of the scene.
 */
void Scene3D_add_triangle(Scene3D* scene, Triangle3D triangle){
        if (scene->root == NULL){
                Triangle3DNode* node = malloc(sizeof(Triangle3DNode));
                scene->root = node;
                node->triangle = triangle;
                node->next = NULL;
		tail = node;
        } else {
		Triangle3DNode* newNode = malloc(sizeof(Triangle3DNode));
                tail->next = newNode;
                newNode->triangle = triangle;
		newNode->next = NULL;
		tail = newNode;
        }
        scene->count++;
}

void Scene3D_add_quadrilateral(Scene3D* scene, Coordinate3D a, Coordinate3D b, Coordinate3D c, Coordinate3D d){
	Triangle3D triangle_1 = (Triangle3D) {a, b, c};
        Triangle3D triangle_2 = (Triangle3D) {b, c, d};
        Triangle3D triangle_3 = (Triangle3D) {a, c, d};
        Triangle3D triangle_4 = (Triangle3D) {a, b, d};
        Scene3D_add_triangle(scene, triangle_1);
        Scene3D_add_triangle(scene, triangle_2);
        Scene3D_add_triangle(scene, triangle_3);
        Scene3D_add_triangle(scene, triangle_4);
}

/*Parameter scene: The scene to add the sides of a pyramid to.
 *Parameter c1: The first corner coordinate used.
 *Parameter c2: The second corner coordinate used.
 *Parameter c3: The third corner coordinate used.
 *Parameter c4: The fourth corner coordinate used.
 *Parameter top: The coordinate representing the top of the pyramid.
 *This method creates 4 triangles representing the 4 sides of a pyramid using the 4 corners
 *and the top passed as parameters and then adds them all to the scene.
 */
void Scene3D_add_pyramid_sides(Scene3D* scene, Coordinate3D c1, Coordinate3D c2, Coordinate3D c3, Coordinate3D c4, Coordinate3D top){
        Triangle3D triangle_1 = (Triangle3D) {top, c1, c2};
        Triangle3D triangle_2 = (Triangle3D) {top, c1, c3};
        Triangle3D triangle_3 = (Triangle3D) {top, c2, c4};
        Triangle3D triangle_4 = (Triangle3D) {top, c3, c4};
        Scene3D_add_triangle(scene, triangle_1);
        Scene3D_add_triangle(scene, triangle_2);
        Scene3D_add_triangle(scene, triangle_3);
        Scene3D_add_triangle(scene, triangle_4);
}

void Scene3D_add_pyramid(Scene3D* scene, Coordinate3D origin, double width, double height, char* orientation){
        if (strcmp(orientation, "left") == 0 || strcmp(orientation, "right") == 0){
                Coordinate3D c1 = (Coordinate3D) {origin.x, (origin.y-(width/2)), (origin.z-(width/2))};
                Coordinate3D c2 = (Coordinate3D) {origin.x, (origin.y+(width/2)), (origin.z-(width/2))};
                Coordinate3D c3 = (Coordinate3D) {origin.x, (origin.y-(width/2)), (origin.z+(width/2))};
                Coordinate3D c4 = (Coordinate3D) {origin.x, (origin.y+(width/2)), (origin.z+(width/2))};
                Scene3D_add_quadrilateral(scene, c1, c2, c3, c4);
                if (strcmp(orientation, "left") == 0){
                        Coordinate3D top = (Coordinate3D) {(origin.x-height), origin.y, origin.z};
                        Scene3D_add_pyramid_sides(scene, c1, c2, c3, c4, top);
                } else {
                        Coordinate3D top = (Coordinate3D) {(origin.x+height), origin.y, origin.z};
                        Scene3D_add_pyramid_sides(scene, c1, c2, c3, c4, top);
                }
        } else if (strcmp(orientation, "forward") == 0 || strcmp(orientation, "backward") == 0){
                Coordinate3D c1 = (Coordinate3D) {(origin.x-(width/2)), origin.y, (origin.z-(width/2))};
                Coordinate3D c2 = (Coordinate3D) {(origin.x+(width/2)), origin.y, (origin.z-(width/2))};
                Coordinate3D c3 = (Coordinate3D) {(origin.x-(width/2)), origin.y, (origin.z+(width/2))};
                Coordinate3D c4 = (Coordinate3D) {(origin.x+(width/2)), origin.y, (origin.z+(width/2))};
                Scene3D_add_quadrilateral(scene, c1, c2, c3, c4);
                if (strcmp(orientation, "backward") == 0){
                        Coordinate3D top = (Coordinate3D) {origin.x, (origin.y-height), origin.z};
                        Scene3D_add_pyramid_sides(scene, c1, c2, c3, c4, top);
                } else {
                        Coordinate3D top = (Coordinate3D) {origin.x, (origin.y+height), origin.z};
                        Scene3D_add_pyramid_sides(scene, c1, c2, c3, c4, top);
                }
        } else if (strcmp(orientation, "up") == 0 || strcmp(orientation, "down") == 0){
                Coordinate3D c1 = (Coordinate3D) {(origin.x-(width/2)), (origin.y-(width/2)), origin.z};
                Coordinate3D c2 = (Coordinate3D) {(origin.x+(width/2)), (origin.y-(width/2)), origin.z};
                Coordinate3D c3 = (Coordinate3D) {(origin.x-(width/2)), (origin.y+(width/2)), origin.z};
                Coordinate3D c4 = (Coordinate3D) {(origin.x+(width/2)), (origin.y+(width/2)), origin.z};
                Scene3D_add_quadrilateral(scene, c1, c2, c3, c4);
                if (strcmp(orientation, "down") == 0){
                        Coordinate3D top = (Coordinate3D) {origin.x, origin.y, (origin.z-height)};
                        Scene3D_add_pyramid_sides(scene, c1, c2, c3, c4, top);
                } else {
                        Coordinate3D top = (Coordinate3D) {origin.x, origin.y, (origin.z+height)};
                        Scene3D_add_pyramid_sides(scene, c1, c2, c3, c4, top);
                }
        }
}

void Scene3D_add_cuboid(Scene3D* scene, Coordinate3D origin, double width, double height, double depth){
        //Variable format: Left/Right+Back/Forward+Down/Up
        Coordinate3D LBD = (Coordinate3D) {(origin.x-(width/2)), (origin.y-(height/2)), (origin.z-(depth/2))};
        Coordinate3D RBD = (Coordinate3D) {(origin.x+(width/2)), (origin.y-(height/2)), (origin.z-(depth/2))};
        Coordinate3D LFD = (Coordinate3D) {(origin.x-(width/2)), (origin.y+(height/2)), (origin.z-(depth/2))};
        Coordinate3D RFD = (Coordinate3D) {(origin.x+(width/2)), (origin.y+(height/2)), (origin.z-(depth/2))};
        Coordinate3D LBU = (Coordinate3D) {(origin.x-(width/2)), (origin.y-(height/2)), (origin.z+(depth/2))};
        Coordinate3D RBU = (Coordinate3D) {(origin.x+(width/2)), (origin.y-(height/2)), (origin.z+(depth/2))};
        Coordinate3D LFU = (Coordinate3D) {(origin.x-(width/2)), (origin.y+(height/2)), (origin.z+(depth/2))};
        Coordinate3D RFU = (Coordinate3D) {(origin.x+(width/2)), (origin.y+(height/2)), (origin.z+(depth/2))};
        Scene3D_add_quadrilateral(scene, LFD, LBD, LFU, LBU);
        Scene3D_add_quadrilateral(scene, RFD, RBD, RFU, RBU);
        Scene3D_add_quadrilateral(scene, LFD, RFD, LFU, RFU);
        Scene3D_add_quadrilateral(scene, LBD, RBD, LBU, RBU);
        Scene3D_add_quadrilateral(scene, LFD, RFD, LBD, RBD);
        Scene3D_add_quadrilateral(scene, LFU, RFU, LBU, RBU);
}

/*Parameter scene: The scene to add the fractal part to.
 *Parameter origin: The origin of the part of the fractal being added.
 *Parameter size: The side length of the previous level of the fractal.
 *Parameter levels: The number of remaining levels to add.
 *This function adds 6 cubes around one of the cubes made on the last level of the fractal, with one cube
 *on each side. It then checks if more levels need to be added and calls itself for each of the new cubes
 *if they do.
 */
void Scene3D_add_fractal_helper(Scene3D* scene, Coordinate3D origin, double size, int levels){
        double newSize = size/2;
        Coordinate3D o1 = (Coordinate3D) {(origin.x-newSize), origin.y, origin.z};
        Coordinate3D o2 = (Coordinate3D) {(origin.x+newSize), origin.y, origin.z};
        Coordinate3D o3 = (Coordinate3D) {origin.x, (origin.y-newSize), origin.z};
        Coordinate3D o4 = (Coordinate3D) {origin.x, (origin.y+newSize), origin.z};
        Coordinate3D o5 = (Coordinate3D) {origin.x, origin.y, (origin.z-newSize)};
        Coordinate3D o6 = (Coordinate3D) {origin.x, origin.y, (origin.z+newSize)};
        Scene3D_add_cuboid(scene, o1, newSize, newSize, newSize);
        Scene3D_add_cuboid(scene, o2, newSize, newSize, newSize);
        Scene3D_add_cuboid(scene, o3, newSize, newSize, newSize);
        Scene3D_add_cuboid(scene, o4, newSize, newSize, newSize);
        Scene3D_add_cuboid(scene, o5, newSize, newSize, newSize);
        Scene3D_add_cuboid(scene, o6, newSize, newSize, newSize);
        if (levels > 1){
                Scene3D_add_fractal_helper(scene, o1, newSize, levels-1);
                Scene3D_add_fractal_helper(scene, o2, newSize, levels-1);
                Scene3D_add_fractal_helper(scene, o3, newSize, levels-1);
                Scene3D_add_fractal_helper(scene, o4, newSize, levels-1);
                Scene3D_add_fractal_helper(scene, o5, newSize, levels-1);
                Scene3D_add_fractal_helper(scene, o6, newSize, levels-1);
        }
}

void Scene3D_add_fractal(Scene3D* scene, Coordinate3D origin, double size, int levels){
        Scene3D_add_cuboid(scene, origin, size, size, size);
        if (levels > 1){
                Scene3D_add_fractal_helper(scene, origin, size, levels-1);
        }
}

/*Parameter spherical: A spherical Coordinate3D to be converted to cartesian.
 *Parameter origin: The origin of the sphere.
 *Returns: A cartesian Coordinate3D made by converting the spherical coordinate.
 *This function takes a spherical Coordinate3D and makes a cartesian coordinate by calculating x, y, and z
 *using the radius, theta, and phi of the spherical coordinate and the location of the origin. It rounds
 *to 4 decimal places.
 */
Coordinate3D convert_spherical_to_cartesian(Coordinate3D spherical, Coordinate3D origin){
        double x = (roundf(spherical.x*cos(spherical.y*DEGTORAD)*sin(spherical.z*DEGTORAD))*10000)/10000;
        double y = (roundf(spherical.x*sin(spherical.y*DEGTORAD)*sin(spherical.z*DEGTORAD))*10000)/10000;
        double z = (roundf(spherical.x*cos(spherical.z*DEGTORAD))*10000)/10000;
	return (Coordinate3D) {(x+origin.x), (y+origin.y), (z+origin.z)};
}

void Scene3D_add_sphere(Scene3D* scene, Coordinate3D origin, double radius, double increment){
        for (double phi=increment; phi<=180; phi+=increment){
                for (double theta=0; theta<360; theta+=increment){
                        //s1 = spherical coordinate 1, c1 = cartesian coordinate 1
                        Coordinate3D s1 = (Coordinate3D) {radius, theta, phi};
                        Coordinate3D s2 = (Coordinate3D) {radius, theta, phi-increment};
                        Coordinate3D s3 = (Coordinate3D) {radius, theta-increment, phi};
                        Coordinate3D s4 = (Coordinate3D) {radius, theta-increment, phi-increment};
                        Coordinate3D c1 = convert_spherical_to_cartesian(s1, origin);
                        Coordinate3D c2 = convert_spherical_to_cartesian(s2, origin);
                        Coordinate3D c3 = convert_spherical_to_cartesian(s3, origin);
                        Coordinate3D c4 = convert_spherical_to_cartesian(s4, origin);
                        Scene3D_add_quadrilateral(scene, c1, c2, c3, c4);
                }
        }
}