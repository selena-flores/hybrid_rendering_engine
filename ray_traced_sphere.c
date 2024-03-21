
int quad(double A, double B, double C, double t[2])
{

  int sols = -1;
  double det;

  t[0] = -1;
  t[1] = -1;
  det = B * B - 4 * A * C;
  if (det > 0)
  {
    sols = 2;
    t[0] = ((-B + sqrt(det)) / (2 * A));
    t[1] = ((-B - sqrt(det)) / (2 * A));
  }
  else if (det < 0)
  {
    sols = 0;
  }
  else
  {
    sols = 1;
    t[0] = ((-B) / (2 * A));
  }

  return sols;
}

void get_partial_sphere(double partial[], double xyz[3]){
    partial[0] = 2 * xyz[0];
    partial[1] = 2 * xyz[1];
    partial[2] = 2 * xyz[2];
}

double render_ray_traced_sphere(double start[], double end[], double delta[])
{

  double T[2];
  double A = delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2];
  double B = 2 * start[0] * delta[0] + 2 * start[1] * delta[1] + 2 * start[2] * delta[2];
  double C = start[0] * start[0] + start[1] * start[1] + start[2] * start[2] - 1;
  int sols = quad(A, B, C, T);

  double tval = 1e50;
  for (int i = 0; i < sols; i++)
  {
    if (T[i] > 0 && T[i] < tval)
    {
      tval = T[i];
    }
  }
  return tval;
}