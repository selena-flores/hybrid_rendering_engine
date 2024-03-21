#include "FPToolkit.c"
#include "M3d_matrix_tools.c"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "graphics_structs.c"
#include "light_model.c"
#include "ray_tracer.c"
#include "graphics.c"
#include "tree.c"
#include "sphere.c"
#include "ray_traced_sphere.c"
#include "cylinder.c"
#include "plane.c"
#include "torus.c"
#include "inputs.c"

void initialize_objects()
{
    double pos[3] = {0, 0, 15};
    double rot[3] = {0, 0, 0};
    double scl[3] = {2, 2, 2};
    double rgb[3] = {0, 1, 1};

    // torus
    
    scene.objects[scene.num_objects] = *create_object(rot, scl, pos, rgb);
    scene.objects[scene.num_objects].render = render_torus;
    scene.objects[scene.num_objects].r_inner = 1;
    scene.objects[scene.num_objects].r_outer = .25;
    scene.num_objects++;

    // rgb[0] = 1;
    // rgb[1] = .1;
    // rgb[2] = 1;
    // scl[0] = .1;
    // scl[1] = .1;
    // scl[2] = .1;

    // // tree
    // int num_branches = 50;
    // scene.objects[scene.num_objects] = *create_object(rot, scl, pos, rgb);
    // scene.objects[scene.num_objects].render = render_tree;
    // scene.objects[scene.num_objects].head = init_tree(num_branches);
    // scene.num_objects++;

    // // cylinder
    // pos[0] = 0;
    // pos[1] = 0;
    // pos[2] = 1;
    // scl[0] = .2;
    // scl[1] = .2;
    // scl[2] = .2;
    // rgb[0] = 1;
    // rgb[1] = .4;
    // rgb[2] = 1;
    // scene.objects[scene.num_objects] = *create_object(rot, scl, pos, rgb);
    // scene.objects[scene.num_objects].render = render_cylinder;
    // scene.num_objects++;

    // pos[0] = 0;
    // pos[1] = 0;
    // pos[2] = 1;
    // rot[0] = -45;
    // rot[1] = 0;
    // rot[2] = 0;
    // scl[0] = .25;
    // scl[1] = .25;
    // scl[2] = .25;
    // rgb[0] = .1;
    // rgb[1] = .3;
    // rgb[2] = 1;
    // scene.objects[scene.num_objects] = *create_object(rot, scl, pos, rgb);
    // scene.objects[scene.num_objects].render = render_plane;
    // scene.num_objects++;

    pos[0] = 3;
    rgb[0] = .1;
    rgb[1] = 0;
    rgb[2] = 1;
    scene.objects[scene.num_objects] = *create_object(rot, scl, pos, rgb);
    scene.objects[scene.num_objects].ref[0] = .2;
    scene.objects[scene.num_objects].ref[1] = .2;
    scene.objects[scene.num_objects].ref[2] = .2;
    scene.objects[scene.num_objects].get_partial = get_partial_sphere;
    scene.objects[scene.num_objects].ray_render = render_ray_traced_sphere;
    scene.num_objects++;

    pos[0] = -3;
    rgb[0] = .1;
    rgb[1] = 0;
    rgb[2] = 1;
    scene.objects[scene.num_objects] = *create_object(rot, scl, pos, rgb);
    scene.objects[scene.num_objects].ref[0] = .5;
    scene.objects[scene.num_objects].ref[1] = .5;
    scene.objects[scene.num_objects].ref[2] = .5;
    scene.objects[scene.num_objects].get_partial = get_partial_sphere;
    scene.objects[scene.num_objects].ray_render = render_ray_traced_sphere;
    scene.num_objects++;
}
void set_lights()
{
    scene.lights[0] = *create_light(camera.eye[0], camera.eye[1], camera.eye[2]);
}
void update_scene()
{
    scene.objects[0].rot[0] = 10 * screen.i_time;
    scene.objects[0].rot[1] = 10 * screen.i_time;

    set_obj_mat(&scene.objects[0]);

    scene.lights[0].x = camera.eye[0];
    scene.lights[0].y = camera.eye[1];
    scene.lights[0].z = camera.eye[2];
}
void render_scene()
{
    render_using_ray_tracer();
    render_using_zbuffer();
}

int main()
{

    init_screen(600, 600);
    G_init_graphics(screen.width, screen.height);

    double camera_lens_angle = 45;
    double eye[3] = {0, 0, 0};
    set_camera_constants(screen.width, screen.height, camera_lens_angle, eye);
    double v[3] = {0, 0, 1};
    position_camera(v);

    initialize_objects();
    set_lights();
    screen.i_time = 0;
    int q = '0';
    while (q != 'q')
    {
        refresh_screen();

        render_scene();
        G_display_image();

        q = G_no_wait_key();
        get_key(q, v);
        position_camera(v);
        update_scene();
        screen.i_time += M_PI / 10;
        usleep(100 / 3);
    }
}