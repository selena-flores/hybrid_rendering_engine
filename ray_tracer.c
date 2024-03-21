
#define DEPTH 2

double calc_mag(double N[3])
{
    return sqrt(N[0] * N[0] + N[1] * N[1] + N[2] * N[2]);
}

void compute_norm(struct object obj, double rxyz[3], int j_min, double N[3])
{

    double partial[3];

    obj.get_partial(partial, rxyz);

    N[0] = partial[0] * obj.RST_inverse[0][0] + partial[1] * obj.RST_inverse[1][0] + partial[2] * obj.RST_inverse[2][0];
    N[1] = partial[0] * obj.RST_inverse[0][1] + partial[1] * obj.RST_inverse[1][1] + partial[2] * obj.RST_inverse[2][1];
    N[2] = partial[0] * obj.RST_inverse[0][2] + partial[1] * obj.RST_inverse[1][2] + partial[2] * obj.RST_inverse[2][2];

    double mag = calc_mag(N);
    N[0] /= mag;
    N[1] /= mag;
    N[2] /= mag;
}

void compute_reflection(double res[3], double src[3],
                        double tip[3], double N[3])
{

    double V[3];
    double mag, vDotN;

    V[0] = src[0] - tip[0];
    V[1] = src[1] - tip[1];
    V[2] = src[2] - tip[2];

    // mag = calc_mag(V);
    // V[0] /= mag;
    // V[1] /= mag;
    // V[2] /= mag;

    vDotN = (V[0] * N[0] + V[1] * N[1] + V[2] * N[2]);

    res[0] = 2 * vDotN * N[0] - V[0];
    res[1] = 2 * vDotN * N[1] - V[1];
    res[2] = 2 * vDotN * N[2] - V[2];
}

int find_closest_object(double r_xyz[], double ray_source[], double ray_tip[])
{
    int j, j_min = -1;
    double temp_ray_source[3], temp_ray_tip[3];
    double min_t = 1e20, tval = 1e20;

    for (j = 0; j < scene.num_objects; j++)
    {
        if (scene.objects[j].ray_render == NULL)
        {
            continue;
        }

        temp_ray_source[0] = ray_source[0];
        temp_ray_source[1] = ray_source[1];
        temp_ray_source[2] = ray_source[2];

        temp_ray_tip[0] = ray_tip[0];
        temp_ray_tip[1] = ray_tip[1];
        temp_ray_tip[2] = ray_tip[2];

        M3d_mat_mult_pt(temp_ray_source, scene.objects[j].RST_inverse, temp_ray_source);
        M3d_mat_mult_pt(temp_ray_tip, scene.objects[j].RST_inverse, temp_ray_tip);

        double delta[3] = {temp_ray_tip[0] - temp_ray_source[0],
                           temp_ray_tip[1] - temp_ray_source[1],
                           temp_ray_tip[2] - temp_ray_source[2]};

        tval = scene.objects[j].ray_render(temp_ray_source, temp_ray_tip, delta);

        if (tval < min_t)
        {
            min_t = tval;
            j_min = j;

            // point in object space
            r_xyz[0] = temp_ray_source[0] + min_t * (temp_ray_tip[0] - temp_ray_source[0]);
            r_xyz[1] = temp_ray_source[1] + min_t * (temp_ray_tip[1] - temp_ray_source[1]);
            r_xyz[2] = temp_ray_source[2] + min_t * (temp_ray_tip[2] - temp_ray_source[2]);
        }

    } // end for j

    return j_min;
}

double ray_tracer(double ray_source[], double ray_tip[], double argb[], int d)
{

    double r_xyz[3];
    int j_min = find_closest_object(r_xyz, ray_source, ray_tip);
    if (j_min == -1)
    {
        // no intersections
        argb[0] = 0;
        argb[1] = 0;
        argb[2] = 0;
        return 1e20;
    }
    else{
        double N[3];
        double res[3];
        // r_xyz needs object space at this point in code for compute_norm
        compute_norm(scene.objects[j_min], r_xyz, j_min, N); // N is the eyespace unit norm
        // get_rgb(r_xyz, domain[j_min][0], domain[j_min][1], // r_xyz in object space
        //         range[j_min][0], range[j_min][1],
        //         rangeoid[j_min][0], rangeoid[j_min][1],
        //         type[j_min], color[j_min]); // color pixel based on u,v position

        // need point in eyespace :
        M3d_mat_mult_pt(r_xyz, scene.objects[j_min].RST, r_xyz);
        double rgb[3];
        Light_Model(scene.objects[j_min].rgb, ray_source, r_xyz, N, rgb, 1);

        compute_reflection(res, ray_source, ray_tip, N); // in world space
        double nSrc[3], nTip[3];

        nSrc[0] = r_xyz[0] + 0.001 * res[0];
        nSrc[1] = r_xyz[1] + 0.001 * res[1];
        nSrc[2] = r_xyz[2] + 0.001 * res[2];
        nTip[0] = r_xyz[0] + res[0];
        nTip[1] = r_xyz[1] + res[1];
        nTip[2] = r_xyz[2] + res[2];
        // if (shadow_ray(nSrc, light_in_eye_space) == 1)
        // {
        //     double f = AMBIENT / (AMBIENT + MAX_DIFFUSE);
        //     rgb[0] = f * scene.objects[j_min].rgb[0];
        //     rgb[1] = f * scene.objects[j_min].rgb[1];
        //     rgb[2] = f * scene.objects[j_min].rgb[2];
        // } // do shadow ray trace

        if (d == DEPTH)
        { // base case
            argb[0] = rgb[0] * (1 - scene.objects[j_min].ref[0]);
            argb[1] = rgb[1] * (1 - scene.objects[j_min].ref[1]);
            argb[2] = rgb[2] * (1 - scene.objects[j_min].ref[2]);
        }
        else if (d < DEPTH)
        { // recurse case
            double aargb[3];
            ray_tracer(nSrc, nTip, aargb, ++d);
            argb[0] = rgb[0] * (1 - scene.objects[j_min].ref[0]) + aargb[0] * scene.objects[j_min].ref[0];
            argb[1] = rgb[1] * (1 - scene.objects[j_min].ref[1]) + aargb[1] * scene.objects[j_min].ref[1];
            argb[2] = rgb[2] * (1 - scene.objects[j_min].ref[2]) + aargb[2] * scene.objects[j_min].ref[2];
        }
        return r_xyz[2];
    }

}
