struct object
{
    double rot[3];
    double scl[3];
    double pos[3];
    double rgb[3];
    double ref[3];
    double r_inner;
    double r_outer;
    struct treenode *head;
    double RST[4][4];
    double RST_inverse[4][4];
    int type;
    void (*get_partial)(double res[3], double point[3]);
    double (*ray_render)(double start[], double end[], double delta[]);
    void (*render)(struct object obj);
};

struct camera
{
    double eye[3];
    double up[3];
    double coi[3];
    double x_z_angle;
    double v[4][4];
    double vi[4][4];
    double zbuf[1000][1000];
    double H;
    double a;
    double b;
    double c;
};

#define MAX_LIGHTS 20

struct light{
    double x;
    double y;
    double z;
};

struct camera camera;

struct screen
{
    int width;
    int height;
    double i_time;
};

struct screen screen;

struct scene
{
    struct object objects[100];
    struct camera camera;
    struct light lights[MAX_LIGHTS];
    int num_objects;
};

struct scene scene;

int out_of_frame_objects[100];
