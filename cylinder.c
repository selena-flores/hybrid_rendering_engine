
double cylX(double u, double v)
{
  return cos(u);
}
double cylY(double u, double v)
{
  return v;
}
double cylZ(double u, double v)
{
  return sin(u);
}

void render_cylinder(struct object obj)
{

  double P[3], Q[3], R[3], S[3];
  double RES = 50;
  double delta = 1 / 100.0;
  double ulo = -M_PI, uhi = M_PI;
  double vlo = -M_PI / 2, vhi = M_PI / 2;
  for (double u = ulo; u < uhi; u += delta)
  {
    for (double v = vlo; v < vhi; v += delta)
    {

      P[0] = cylX(u, v);
      P[1] = cylY(u, v);
      P[2] = cylZ(u, v);

      Q[0] = cylX(u + 1 / RES, v);
      Q[1] = cylY(u + 1 / RES, v);
      Q[2] = cylZ(u + 1 / RES, v);
      R[0] = cylX(u, v + 1 / RES);
      R[1] = cylY(u, v + 1 / RES);
      R[2] = cylZ(u, v + 1 / RES);

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