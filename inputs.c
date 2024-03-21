void get_key(int q, double v[3])
{

    double del = M_PI / 10;

    if (q == 65362)
    {
        camera.eye[0] += v[0]; // make camera.eye go in direction of the vector
        camera.eye[1] += v[1];
        camera.eye[2] += v[2];
    }
    else if (q == 65363)
    {
        camera.x_z_angle += del;
        v[0] = sin(camera.x_z_angle); // rotate the vector from original direction to new angle
        v[1] = 0;
        v[2] = cos(camera.x_z_angle);
    }
    else if (q == 65364)
    {
        camera.eye[0] -= v[0];
        camera.eye[1] -= v[1];
        camera.eye[2] -= v[2];
    }
    else if (q == 65361)
    {
        camera.x_z_angle -= del;
        v[0] = sin(camera.x_z_angle);
        v[1] = 0;
        v[2] = cos(camera.x_z_angle);
    }
}
