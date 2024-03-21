int point_is_closer(double point[], double xyz[], int render)
{
    if (render == 0)
    {
        if (xyz[2] > 0 &&
            xyz[1] / xyz[2] < camera.H &&
            xyz[0] / xyz[2] < camera.H &&
            xyz[1] / xyz[2] > -camera.H &&
            xyz[0] / xyz[2] > -camera.H)
        {
            point[0] = camera.a * (xyz[0] / xyz[2]) + camera.b; // screen transformation
            point[1] = camera.a * (xyz[1] / xyz[2]) + camera.c;
            // check if the point is allowed into zbuffer
            if (point[0] < screen.width &&
                point[1] < screen.height &&
                point[0] >= 0 && point[1] >= 0 &&
                xyz[2] < camera.zbuf[(int)(point[0])][(int)(point[1])])
            {
                return 1;
            }
        }
    }
    else if (render == 1)
    {
        if (xyz[0] > 0 && xyz[2] < camera.zbuf[(int)(point[0])][(int)(point[1])])
        {
            return 1;
        }
    }
    return 0;
}

void zbuffer_render(double xyz[3], double N[3], struct object obj)
{
    double argb[3], point[2];
    if (point_is_closer(point, xyz, 0) == 1)
    {
        camera.zbuf[(int)(point[0])][(int)(point[1])] = xyz[2];
        M3d_mat_mult_pt(camera.eye, camera.v, camera.eye);
        Light_Model(obj.rgb, camera.eye, xyz, N, argb, 0);
        M3d_mat_mult_pt(camera.eye, camera.vi, camera.eye);
        G_rgb(argb[0], argb[1], argb[2]);
        G_point(point[0], point[1]);
    }
}

void render_using_zbuffer()
{
    for (int i = 0; i < scene.num_objects; i++)
    {
        if (scene.objects[i].render != NULL)
            scene.objects[i].render(scene.objects[i]);
    }
}

void render_using_ray_tracer()
{
    double rgb[3] = {0, 0, 0};
    double eye_tip[3];
    double xyz[3], point[2];
    // int Tn, Ttypelist[100];
    // double Tvlist[100];
    // double M[4][4], Mi[4][4];
    // Tn = 0;
    // Ttypelist[Tn] = RY;
    // Tvlist[Tn] = atan2(camera.coi[0] - camera.eye[0], camera.coi[2] - camera.eye[2]);
    // Tn++;
    // Ttypelist[Tn] = TX;
    // Tvlist[Tn] = camera.eye[0];
    // Tn++;
    // Ttypelist[Tn] = TY;
    // Tvlist[Tn] = camera.eye[1];
    // Tn++;
    // Ttypelist[Tn] = TZ;
    // Tvlist[Tn] = camera.eye[2];
    // Tn++;
    // M3d_make_movement_sequence_matrix(M, Mi, Tn,
    //                                   Ttypelist, Tvlist); // move object
    for (double k = 0; k < screen.width; k += 1)
    {
        for (double l = 0; l < screen.height; l += 1)
        {
            // sin(atan2(v[0]/v[2]));
            // cos(atan2(v[0]/v[2]));
            eye_tip[0] = (k - camera.b) / camera.a; // place the eye_tip relative to eye in eye space
            eye_tip[1] = (l - camera.c) / camera.a;
            eye_tip[2] = 1;

            M3d_mat_mult_pt(eye_tip, camera.vi, eye_tip);

            xyz[2] = ray_tracer(camera.eye, eye_tip, rgb, 0);

            point[0] = k;
            point[1] = l;
            if (point_is_closer(point, xyz, 1) == 1)
            {
                camera.zbuf[(int)(point[0])][(int)(point[1])] = xyz[2];
                G_rgb(rgb[0], rgb[1], rgb[2]);
                G_point(k, l);
            }
        }
    }
}

void position_camera(double v[3])
{
    camera.coi[0] = camera.eye[0] + v[0];
    camera.coi[1] = camera.eye[1] + v[1];
    camera.coi[2] = camera.eye[2] + v[2];

    camera.up[0] = camera.eye[0] + 0;
    camera.up[1] = camera.eye[1] + 1;
    camera.up[2] = camera.eye[2] + 0;

    M3d_view(camera.v, camera.vi, camera.eye, camera.coi, camera.up); // create view matrix
}

void set_camera_constants(int width, int height, double Half_angle_degrees, double eye[3])
{
    double Half_window_size = 0.5 * width;
    camera.H = tan(Half_angle_degrees * M_PI / 180);
    camera.a = Half_window_size / camera.H;
    camera.b = 0.5 * width;
    camera.c = 0.5 * height;
    camera.eye[0] = eye[0];
    camera.eye[1] = eye[1];
    camera.eye[2] = eye[2];
}

void set_rgb(struct object *obj, double rgb[3])
{
    for (int i = 0; i < 3; i++)
        obj->rgb[i] = rgb[i];
}

void set_world_rst(struct object *obj, double rot[3], double scl[3], double pos[3])
{

    for (int i = 0; i < 3; i++)
    {
        obj->rot[i] = rot[i];
        obj->scl[i] = scl[i];
        obj->pos[i] = pos[i];
    }
}

void set_obj_mat(struct object *obj)
{
    int Tn, Ttypelist[100];
    double Tvlist[100];
    Tn = 0;
    Ttypelist[Tn] = SX;
    Tvlist[Tn] = obj->scl[0];
    Tn++;
    Ttypelist[Tn] = SY;
    Tvlist[Tn] = obj->scl[1];
    Tn++;
    Ttypelist[Tn] = SZ;
    Tvlist[Tn] = obj->scl[2];
    Tn++;
    Ttypelist[Tn] = RX;
    Tvlist[Tn] = obj->rot[0];
    Tn++;
    Ttypelist[Tn] = RY;
    Tvlist[Tn] = obj->rot[1];
    Tn++;
    Ttypelist[Tn] = RZ;
    Tvlist[Tn] = obj->rot[2];
    Tn++;
    Ttypelist[Tn] = TX;
    Tvlist[Tn] = obj->pos[0];
    Tn++;
    Ttypelist[Tn] = TY;
    Tvlist[Tn] = obj->pos[1];
    Tn++;
    Ttypelist[Tn] = TZ;
    Tvlist[Tn] = obj->pos[2];
    Tn++;

    M3d_make_movement_sequence_matrix(obj->RST, obj->RST_inverse, Tn,
                                      Ttypelist, Tvlist); // move object
}

struct object *create_object(double rot[3], double scl[3], double pos[3], double rgb[3])
{
    struct object *obj = malloc(sizeof(struct object));
    set_world_rst(obj, rot, scl, pos);
    set_obj_mat(obj);
    set_rgb(obj, rgb);
    return obj;
}

void set_light_pos(struct light *light, double x, double y, double z)
{
    light->x = x;
    light->y = y;
    light->z = z;
}

struct light *create_light(double x, double y, double z)
{
    struct light *light = malloc(sizeof(struct light));
    set_light_pos(light, x, y, z);
    return light;
}

void init_screen(int width, int height)
{
    screen.width = width;
    screen.height = height;
}

void refresh_zbuf()
{
    for (int i = 0; i < screen.width; i++)
    {
        for (int j = 0; j < screen.height; j++)
        {
            camera.zbuf[i][j] = 1e10;
        }
    }
}

void refresh_screen()
{
    G_rgb(0, 0, 0);
    G_clear();
    refresh_zbuf();
}
