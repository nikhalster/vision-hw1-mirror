#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853
#define EXP 2.718

void l1_normalize(image im)
{
    for (int j = 0; j < im.h; j++){
        for (int k = 0; k < im.w; k++){
            // im.data[(k + (im.w * j))] = 1.0 / (im.h * im.w);
                set_pixel(im,k,j,0,get_pixel(im,k,j,0)/(1.0*im.w*im.h));

        }
    }
}

image make_box_filter(int w)
{
    image im = make_image(w,w,1);

    for (int j = 0; j < im.w; j++){
        for (int k = 0; k < im.h; k++){
            for (int l = 0; l < im.c; l++){
                set_pixel(im, j, k, l, 1.0);
            }
        }
    }

    l1_normalize(im);
    return im;
}

image convolve_image(image im, image filter, int preserve)
{
    //If filter and im have the same number of channels then it's just a normal convolution. We sum over spatial and channel dimensions and produce a 1 channel image. UNLESS:
    //If preserve is set to 1 we should produce an image with the same number of channels as the input. This is useful if, for example, we want to run a box filter over an RGB image and get out an RGB image. This means each channel in the image will be filtered by the corresponding channel in the filter. UNLESS:
    //If the filter only has one channel but im has multiple channels we want to apply the filter to each of those channels. Then we either sum between channels or not depending on if preserve is set.
    //Also, filter better have either the same number of channels as im or have 1 channel. I check this with an assert.
    assert(im.c == filter.c || filter.c == 1);
    if(filter.c==1 && preserve == 1){
        image result_image = make_image(im.w, im.h, 3); 
        for (int j = 0; j < im.w; j++){
            for (int k = 0; k < im.h; k++){
                for (int l = 0; l < im.c; l++){
                    float convolved_value = 0.0;
                
                    for (int m = 0; m < filter.w; m++){
                        for (int n = 0; n < filter.h; n++){
                        
                            float filter_value = get_pixel(filter, m, n, 0);
                            convolved_value += (get_pixel(im, j - filter.w/2 + m, k - filter.h/2 + n, l) * filter_value);
                        }
                    }
                    set_pixel(result_image, j, k, l, convolved_value);   
                }
            }
        }
        return result_image;
    }    

    if(preserve == 0){
        image result_image = make_image(im.w, im.h, 1); 
        for (int j = 0; j < result_image.w; j++){
            for (int k = 0; k < result_image.h; k++){
                float convolved_value = 0.0;
                for (int l = 0; l < im.c; l++){
                    
                
                    for (int m = 0; m < filter.w; m++){
                        for (int n = 0; n < filter.h; n++){
                        
                            float filter_value = get_pixel(filter, m, n, 0);
                            convolved_value = convolved_value + (get_pixel(im, j - filter.w/2 + m, k - filter.h/2 + n, l) * filter_value);
                        }
                    }
                    
                }
            set_pixel(result_image, j, k, 0, convolved_value);   

            }
        }
    return result_image;    
    }    
    return make_image(1,1,1);

}

image make_highpass_filter()
{
    
    image im = make_box_filter(3);
    set_pixel(im, 0, 0, 0, 0);
    set_pixel(im, 1, 0, 0, -1);
    set_pixel(im, 2, 0, 0, 0);
    set_pixel(im, 0, 1, 0, -1);
    set_pixel(im, 1, 1, 0, 4);
    set_pixel(im, 2, 1, 0, -1);
    set_pixel(im, 0, 2, 0, 0);
    set_pixel(im, 1, 2, 0, -1);
    set_pixel(im, 2, 2, 0, 0);
    return im;
}

image make_sharpen_filter()
{
    
    image im = make_box_filter(3);
    set_pixel(im, 0, 0, 0, 0);
    set_pixel(im, 1, 0, 0, -1);
    set_pixel(im, 2, 0, 0, 0);
    set_pixel(im, 0, 1, 0, -1);
    set_pixel(im, 1, 1, 0, 5);
    set_pixel(im, 2, 1, 0, -1);
    set_pixel(im, 0, 2, 0, 0);
    set_pixel(im, 1, 2, 0, -1);
    set_pixel(im, 2, 2, 0, 0);
    return im;
}

image make_emboss_filter()
{
    image im = make_box_filter(3);
    set_pixel(im, 0, 0, 0, -2);
    set_pixel(im, 1, 0, 0, -1);
    set_pixel(im, 2, 0, 0, 0);
    set_pixel(im, 0, 1, 0, -1);
    set_pixel(im, 1, 1, 0, 1);
    set_pixel(im, 2, 1, 0, 1);
    set_pixel(im, 0, 2, 0, 0);
    set_pixel(im, 1, 2, 0, 1);
    set_pixel(im, 2, 2, 0, 2);
    return im;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

image make_gaussian_filter(float sigma)
{
    int filter_size = sigma * 6;
    if(filter_size % 2 == 0){
        filter_size += 1;
    }
    image filter = make_box_filter(filter_size);
    
    for (int j = 0; j < filter.w; j++){
        for (int k = 0; k < filter.h; k++){
            float gx = j - 3*sigma;
            float gy = k - 3*sigma; 
            float gaussian_value = (1.0/(TWOPI*powf(sigma,2))) * powf(EXP, -1.0*(powf(gx,2)+powf(gy,2))/(2*powf(sigma,2)));

            set_pixel(filter, j, k, 0, gaussian_value);
        }

    }
    return filter;


}

image add_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);

    image result_image = make_image(a.w, a.h, a.c); 
    
    for (int j = 0; j < a.w; j++){
        for (int k = 0; k < a.h; k++){
            for (int l = 0; l < a.c; l++){
                float result_value = get_pixel(a, j, k, l) + get_pixel(b, j, k, l);
                set_pixel(result_image, j, k, l, result_value);
            }
        }
    }
    return result_image;
}

image sub_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);

    image result_image = make_image(a.w, a.h, a.c); 
    
    for (int j = 0; j < a.w; j++){
        for (int k = 0; k < a.h; k++){
            for (int l = 0; l < a.c; l++){
                float result_value = get_pixel(a, j, k, l) - get_pixel(b, j, k, l);
                set_pixel(result_image, j, k, l, result_value);
            }
        }
    }
    return result_image;
    
}

image make_gx_filter()
{
    image im = make_box_filter(3);
    set_pixel(im, 0, 0, 0, -1);
    set_pixel(im, 1, 0, 0, 0);
    set_pixel(im, 2, 0, 0, 1);
    set_pixel(im, 0, 1, 0, -2);
    set_pixel(im, 1, 1, 0, 0);
    set_pixel(im, 2, 1, 0, 2);
    set_pixel(im, 0, 2, 0, -1);
    set_pixel(im, 1, 2, 0, 0);
    set_pixel(im, 2, 2, 0, 1);
    return im;
}

image make_gy_filter()
{
    image im = make_box_filter(3);
    set_pixel(im, 0, 0, 0, -1);
    set_pixel(im, 1, 0, 0, -2);
    set_pixel(im, 2, 0, 0, -1);
    set_pixel(im, 0, 1, 0, 0);
    set_pixel(im, 1, 1, 0, 0);
    set_pixel(im, 2, 1, 0, 0);
    set_pixel(im, 0, 2, 0, 1);
    set_pixel(im, 1, 2, 0, 2);
    set_pixel(im, 2, 2, 0, 1);
    return im;
}

void feature_normalize(image im)
{
    float value;
    float min = INFINITY;
    float max = INFINITY * -1;
    for (int j = 0; j < im.w; j++){
        for (int k = 0; k < im.h; k++){
            for (int l = 0; l < im.c; l++){
                value =  get_pixel(im, j, k, l);
                min = (value < min) ? value : min;
                max = (value > max) ? value : max;
            }
        }
    }
    float range = max - min;
    float new_value;
    for (int j = 0; j < im.w; j++){
        for (int k = 0; k < im.h; k++){
            for (int l = 0; l < im.c; l++){
                if(range == 0){
                    set_pixel(im, j, k, l, 0.0);
                }
                else{
                    new_value = (get_pixel(im, j, k, l) - min) / range;
                    set_pixel(im, j, k, l, new_value);
                }
            }
        }
    }
    
}

image *sobel_image(image im)
{
    image gx = make_gx_filter();
    image gy = make_gy_filter();
    image gx_filter_image = convolve_image(im, gx, 0);
    image gy_filter_image = convolve_image(im, gy, 0);

    image * sobel_image =  calloc(2, sizeof(image));
    sobel_image[0] = make_image(im.w, im.h, 1);
    sobel_image[1] = make_image(im.w, im.h, 1);

    for (int j = 0; j < im.w; j++){
        for (int k = 0; k < im.h; k++){
            float g = sqrtf(powf(get_pixel(gx_filter_image, j, k, 0), 2) + powf(get_pixel(gy_filter_image, j, k, 0), 2));
            set_pixel(sobel_image[0], j, k, 0, g);
            float theta = atan2f(get_pixel(gy_filter_image, j, k, 0), get_pixel(gx_filter_image, j, k, 0));
            set_pixel(sobel_image[1], j, k, 0, theta);
        }
    }
    return sobel_image;
}

image colorize_sobel(image im)
{
    // TODO
    return make_image(1,1,1);
}
