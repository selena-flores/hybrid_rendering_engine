struct treenode
{
    double x;
    double y;
    double z;
    struct treenode *left;
    struct treenode *right;
};

struct treenode *insert(struct treenode *node, double x, double y, double z)
{

    if (node == NULL)
    {
        node = malloc(sizeof(struct treenode));
        node->x = x;
        node->y = y;
        node->z = z;
        node->left = node->right = NULL;
    }
    else
    {
        if (drand48() < .6)
        {
            if (x < node->x)
            {
                node->right = insert(node->right, x, y, z);
            }
            else
            {
                node->left = insert(node->left, x, y, z);
            }
        }
        else
        {
            if (drand48() < .5)
            {
                node->right = insert(node->right, x, y, z);
            }
            else
            {
                node->left = insert(node->left, x, y, z);
            }
        }
    }

    return node;
}

void norm(double N[3])
{

    double mag = sqrt(N[0] * N[0] + N[1] * N[1] + N[2] * N[2]);

    N[0] /= mag;
    N[1] /= mag;
    N[2] /= mag;
}

void G_circ3(double P[3], double Q[3], double T[3], struct object obj)
{ // plot line in 3 space

    double xyz[3], N[3];
    double radius = .01;
    for (double theta = 0; theta < 2 * M_PI; theta += 1 / 5.0)
    {

        xyz[0] = T[0] + radius * (cos(theta) * P[0] + sin(theta) * Q[0]); // line is a parametric equation of
        xyz[1] = T[1] + radius * (cos(theta) * P[1] + sin(theta) * Q[1]); // line is a parametric equation of
        xyz[2] = T[2] + radius * (cos(theta) * P[2] + sin(theta) * Q[2]); // line is a parametric equation of

        // point on circle - center point of circle
        N[0] = T[0] - xyz[0];
        N[1] = T[1] - xyz[1];
        N[2] = T[2] - xyz[2];

        // to avoid nan's
        N[0] *= 1e10;
        N[1] *= 1e10;
        N[2] *= 1e10;

        // put point and normal in eye space
        M3d_mat_mult_pt(xyz, obj.RST, xyz);
        M3d_mat_mult_pt(xyz, camera.v, xyz);
        M3d_mat_mult_pt(N, obj.RST, N);
        M3d_mat_mult_pt(N, camera.v, N);

        zbuffer_render(xyz, N, obj);
    }
}

void G_line3(double x1, double y1, double z1,
             double x2, double y2, double z2,
             struct object obj)
{ // plot line in 3 space

    double P[3], Q[3], R[3], S[3], W[3], N[3];

    for (double t = 0; t < 1; t += 1.0 / 200)
    {
        P[0] = x1 + (x2 - x1) * t; // line is a parametric equation of
        P[1] = y1 + (y2 - y1) * t; //  one variable
        P[2] = z1 + (z2 - z1) * t;

        N[0] = P[0] - x1; // partial t
        N[1] = P[1] - y1;
        N[2] = P[2] - z1;

        W[0] = P[0] + drand48(); // create an aux vector in the same plane
        W[1] = P[1] + drand48();
        W[2] = P[2] + drand48();

        M3d_x_product(Q, W, N); // W x N = perp to W and N
        M3d_x_product(S, Q, N); // Q x N = perp to Q and N
        // Thus, Q perp N and S perp Q and N forms basis
        // relative to partial t

        norm(Q); // normalized
        norm(S);
        // nor orthonomral basis

        G_circ3(Q, S, P, obj); // pass starting point and orthobasis to 3D cyl plotter
    }
}

void dfs_render_tree(struct treenode *node, double x, double y, double z,
                     struct object obj)
{
    if (node != NULL)
    {
        G_line3(x, y, z, node->x, node->y, node->z, obj);
        x = node->x;
        y = node->y;
        z = node->z;
        dfs_render_tree(node->left, x, y, z, obj);
        dfs_render_tree(node->right, x, y, z, obj);
    }
}

struct treenode *init_tree(int branches)
{

    double x;
    double y;
    double z;
    int i = 0;
    struct treenode *node = NULL;
    while (i < branches)
    {
        x = y * (drand48() - .5);
        z = y * (drand48() - .5);

        if (drand48() < .6)
            y += .1;
        else
            y -= .07;

        node = insert(node, x, y, z);
        i++;
    }

    return node;
}

void render_tree(struct object obj)
{
    G_line3(obj.head->x, obj.head->y - .5, obj.head->z,
            obj.head->x, obj.head->y, obj.head->z, obj);
    dfs_render_tree(obj.head, obj.head->x, obj.head->y, obj.head->z, obj);
}