#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    int rounded_x = (int) roundf(x);
    int rounded_y = (int) roundf(y);
    return get_pixel(im, rounded_x, rounded_y, c);
    
}

image nn_resize(image im, int w, int h)
{
    // TODO fix edges in the image
    image resized_img = make_image(w, h, im.c);

    float old_start_coeff = -0.5;
    float new_start_coeff = -0.5;

    float old_end_coeff_w = im.w - 0.5;
    float new_end_coeff_w = w - 0.5;

    float old_end_coeff_h = im.h - 0.5;
    float new_end_coeff_h = h - 0.5;

    float x_1 = (new_start_coeff - old_end_coeff_w) / (old_start_coeff - new_end_coeff_w);
    float y_1 = new_start_coeff - (x_1 * old_start_coeff);

    float x_2 = (new_start_coeff - old_end_coeff_h) / (old_start_coeff - new_end_coeff_h);
    float y_2 = new_start_coeff - (x_2 * old_start_coeff);

    for (int j = 0; j < resized_img.h; j++){
        for (int k = 0; k < resized_img.w; k++){
            for (int c = 0; c < resized_img.c; c++){
                float new_x = x_1 * k + y_1;
                float new_y = x_2 * j + y_2;

                float interpolated_value = nn_interpolate(im, new_x, new_y, c);
                set_pixel(resized_img, k, j, c, interpolated_value);
            }
        

        }
    }
    return resized_img;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // TODO
    return 0;
}

image bilinear_resize(image im, int w, int h)
{
    // TODO
    return make_image(1,1,1);
}