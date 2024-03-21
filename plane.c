
double plane_x(double u, double v)
{
    return u;
}
double plane_y(double u, double v)
{
    return 0;
}
double plane_z(double u, double v)
{
    return v;
}

void render_plane(struct object obj)
{

    double P[3], Q[3], R[3], S[3];
    double RES = 50;
    double delta = 1 / 200.0;
    double ulo = -1, uhi = 1;
    double vlo = -1, vhi = 1;
    for (double u = ulo; u < uhi; u += delta)
    {
        for (double v = vlo; v < vhi; v += delta)
        {

            /*

            this renderer uses adaptive delta modification
            depending on the relative stretch factor of each
            u,v cells which are being stretched into objects
            space and then screen space

            */

            P[0] = plane_x(u, v);
            P[1] = plane_y(u, v);
            P[2] = plane_z(u, v);

            Q[0] = plane_x(u + 1 / RES, v);
            Q[1] = plane_y(u + 1 / RES, v);
            Q[2] = plane_z(u + 1 / RES, v);
            R[0] = plane_x(u, v + 1 / RES);
            R[1] = plane_y(u, v + 1 / RES);
            R[2] = plane_z(u, v + 1 / RES);

            M3d_mat_mult_pt(P, obj.RST, P); // transformed by T matrix
            M3d_mat_mult_pt(Q, obj.RST, Q);
            M3d_mat_mult_pt(R, obj.RST, R);
            M3d_mat_mult_pt(P, camera.v, P); // pass through view matrix
            M3d_mat_mult_pt(Q, camera.v, Q); // pass through view matrix
            M3d_mat_mult_pt(R, camera.v, R); // pass through view matrix

            double a[3]; // TODO: FIND PQ
            a[0] = Q[0] - P[0];
            a[1] = Q[1] - P[1];
            a[2] = Q[2] - P[2];

            double b[3]; // TODO: FIND PR
            b[0] = R[0] - P[0];
            b[1] = R[1] - P[1];
            b[2] = R[2] - P[2];

            double N[3];
            M3d_x_product(N, a, b);

            zbuffer_render(P, N, obj);
        }
    }
}