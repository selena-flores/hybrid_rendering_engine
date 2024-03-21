
double torus_x(double u, double v, double r_inner, double r_outer)
{
    return (r_inner + r_outer*cos(v))*cos(u);
}
double torus_y(double u, double v, double r_inner, double r_outer)
{
    return r_outer*sin(v);
}
double torus_z(double u, double v, double r_inner, double r_outer)
{
    return (r_inner + r_outer*cos(v))*sin(u);
}

void render_torus(struct object obj)
{

    double P[3], Q[3], R[3], S[3];
    double RES = 10;
    double delta = 1 / 100.0;
    double ulo = 0, uhi = 2*M_PI;
    double vlo = 0, vhi = 2*M_PI;
    for (double u = ulo; u < uhi; u += delta)
    {
        for (double v = vlo; v < vhi; v += delta)
        {
            P[0] = torus_x(u, v, obj.r_inner, obj.r_outer);
            P[1] = torus_y(u, v, obj.r_inner, obj.r_outer);
            P[2] = torus_z(u, v, obj.r_inner, obj.r_outer);

            Q[0] = torus_x(u + 1 / RES, v, obj.r_inner, obj.r_outer);
            Q[1] = torus_y(u + 1 / RES, v, obj.r_inner, obj.r_outer);
            Q[2] = torus_z(u + 1 / RES, v, obj.r_inner, obj.r_outer);
            R[0] = torus_x(u, v + 1 / RES, obj.r_inner, obj.r_outer);
            R[1] = torus_y(u, v + 1 / RES, obj.r_inner, obj.r_outer);
            R[2] = torus_z(u, v + 1 / RES, obj.r_inner, obj.r_outer);

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