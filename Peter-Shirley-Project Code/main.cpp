#include<fstream>
#include<iostream>
#include "sphere.h"
#include "box.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "random"
#include "material.h"
#include "surface_texture.h"
// 须要先声明宏，不然stb_image 会报错找不到图片格式
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include "aarect.h"
#include "constant_medium.h"
#include "bvh.h"


#define random(a, b) (rand()%(b-a+1)+a)

using namespace std;

vec3 color(const ray &r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        // 散射后的光线
        ray scattered;
        // 衰减
        vec3 attenuation;
        // 记录自发光的颜色
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            // 递归 衰减
            return emitted + attenuation*color(scattered, world, depth+1);
        } else {
            return emitted;
        }
    } else {
//        vec3 unit_direction = unit_vector(r.direction());
//        float t = 0.5 * (unit_direction.y() + 1.0);
//        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
        return vec3(0,0,0);
    }
}


hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable *[n + 1];

    // 棋盘纹理
    texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
                                           new constant_texture(vec3(0.9, 0.9, 0.9)));

    list[0] = new sphere(vec3(0, -700, 0), 700, new lambertian(checker));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = drand48();
            vec3 center(a + 0.9 * drand48(), 0.2, b + 0.9 * drand48());
            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {  // diffuse
                    // 运动模糊的小球
//                    list[i++] = new moving_sphere(center, center + vec3(0, 0.5 * drand48(), 0), 0.0, 1.0, 0.2,
//                         new lambertian(new constant_texture(
//                    vec3(drand48() * drand48(), drand48() * drand48(),drand48() * drand48()))));
                } else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(center, 0.2,
                                           new metal(vec3(0.5 * (1 + drand48()), 0.5 * (1 + drand48()),
                                                          0.5 * (1 + drand48())), 0.5 * drand48()));
                } else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(2.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(1, 1, 1), 0.0));

    return new hitable_list(list, i);
}

hitable *earth() {
    hitable **list = new hitable*[2];
    material *light = new diffuse_light( new constant_texture(vec3(7, 7, 7)) );
    list[0] = new xz_rect(63, 483, 55, 482, 554, light);

    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("picture.png", &nx, &ny, &nn, 0);
    material *mat = new lambertian(new image_texture(tex_data, nx, ny));
    list[1] = new sphere(vec3(360, 250, 150), 100, mat);
    return new hitable_list(list,2);
}

hitable *two_spheres() {
    material *light = new diffuse_light( new constant_texture(vec3(15, 15, 15)) );
    texture *checker = new checker_texture( new constant_texture(vec3(0.2,0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
    material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    int n = 50;
    hitable **list = new hitable*[n+1];
    list[0] =  new sphere(vec3(0,-10, 0), 10,red);
//    list[1] =  new flip_normals(new yz_rect(0, 555, 0, 555, 0, red));
    list[1] =   (new yz_rect(0, 555, 0, 555, 0, red));

    return new hitable_list(list,2);
}

hitable *two_perlin_spheres() {
    texture *pertext = new noise_texture();
    hitable **list = new hitable *[2];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
//    list[1] = new sphere(vec3(0,2,0),2,new lambertian(pertext));
    list[1] = earth();
    return new hitable_list(list, 2);
}

// 带rect和光源的场景
hitable *simple_light() {
    texture *pertext = new noise_texture(4);
    texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
                                           new constant_texture(vec3(0.9, 0.9, 0.9)));
    hitable **list = new hitable *[4];
    int i = 0;
    list[i++] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
    list[i++] = new sphere(vec3(0, -700, 0), 700, new lambertian(checker));
    list[i++] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
    list[i++] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
    return new hitable_list(list, i);
}

hitable *test() {
    texture *pertext = new noise_texture(4);
    texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
                                           new constant_texture(vec3(0.9, 0.9, 0.9)));
    hitable **list = new hitable *[4];
    list[0] = new sphere(vec3(0, -700, 0), 700, new lambertian(checker));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
    list[2] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(11, 11, 11))));
//    list[3] = new xy_rect(3,5,1,3,-2,new diffuse_light(new constant_texture(vec3(4,4,4))));
    return new hitable_list(list, 3);
}

// cornell_box经典场景
hitable *cornell_box() {
    hitable **list = new hitable*[8];
    int i = 0;
    material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
    material *light = new diffuse_light( new constant_texture(vec3(15, 15, 15)) );
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
//    list[i++] = new box(vec3(130,0,65),vec3(295,165,230),white);
//    list[i++] = new box(vec3(265,0,295),vec3(430,330,460),white);
    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130,0,65));
    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15), vec3(265,0,295));
    return new hitable_list(list,i);
}

// cornel_box volume
hitable *cornell_smoke()
{
    hitable **list = new hitable*[8];
    int i = 0;
    material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
    material *light = new diffuse_light( new constant_texture(vec3(4, 4, 4)) );
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    hitable *b1 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130,0,65));
    hitable *b2 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15), vec3(265,0,295));
    list[i++] = new constant_medium(b1, 0.01, new constant_texture(vec3(1.0, 1.0, 1.0)));
    list[i++] = new constant_medium(b2, 0.01, new constant_texture(vec3(0.0, 0.0, 0.0)));
    return new hitable_list(list,i);
}

hitable *final() {
    int nb = 10;
    hitable **list = new hitable*[3000];
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material *ground = new lambertian( new constant_texture(vec3(0.48, 0.83, 0.53)) );
    int b = 0;
    int l = 0;
    for (int i = 0; i < nb; i++) {
        for (int j = 0; j < nb; j++) {
            float w = 100;
            float x0 = i*w;
            float z0 = j*w;
            float y0 = 0;
            float x1 = x0 + w;
            float y1 = 100*(drand48()+0.01);
            float z1 = z0 + w;
            cout << "("<<x0<<","<<y0<<","<<z0<<") ("<<x1<<","<<y1<<","<<z1<<")"<<endl;
            list[l++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
        }
    }
    material *light = new diffuse_light( new constant_texture(vec3(7, 7, 7)) );
    list[l++] = new xz_rect(123, 423, 147, 412, 554, light);
    vec3 center(400, 400, 200);
    list[l++] = new moving_sphere(center, center+vec3(30, 0, 0), 0, 1, 50,
                                  new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));
    list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
    list[l++] = new sphere(vec3(0, 150, 145), 50, new metal(vec3(0.8, 0.8, 0.9), 10.0));
    hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
    list[l++] = boundary;
    list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));
    boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
    list[l++] = new constant_medium(boundary, 0.0001, new constant_texture(vec3(1.0, 1.0, 1.0)));
    texture *pertext = new noise_texture(0.1);
    list[l++] =  new sphere(vec3(220,280, 300), 80, new lambertian( pertext ));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        list[l++] = new sphere(vec3(165*drand48()-100, 165*drand48()+270, 165*drand48()+395),10 , white);
    }
    cout<< "len(l) = " << l << endl;
    return new hitable_list(list,l);
}

inline vec3 de_nan(const vec3 &c)
{
    vec3 t = c;
    if(!(t[0]==t[0]))
        t[0]=0;
    if(!(t[1]==t[1]))
        t[1]=0;
    if(!(t[2]==t[2]))
        t[2]=0;
    return t;
}

int main() {

    string str = "";

    int nx = 1000;
    int ny = 1000;
    // 采样数量ns
    int ns = 100;
/*-----------------------------------------*/
//     cornell box view
    vec3 lookfrom(228, 278, -800);
    vec3 lookat(278, 278, 0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;
    camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

/*-----------------------------------------*/
//    普通场景camera参数
//    vec3 lookfrom(13, 2, 3);            // simple_light cam参数
//    vec3 lookat(0, 0, 0);
//    float dist_to_focus = 10.0;
//    float aperture = 0.1;
//    camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

/*-----------------------------------------*/
//    final场景camera参数
//    vec3 lookfrom(478, 278, -600);
//    vec3 lookat(278, 278, 0);
//    float dist_to_focus = 10.0;
//    float aperture = 0.0;
//    float vfov = 40.0;
//    camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);


/*-----------------------------------------*/
//    hitable *world = random_scene();
//    hitable *world = random_scene();
//    hitable *world = two_spheres();
//    hitable *world = two_perlin_spheres();
//    hitable *world = earth();
//    hitable *world = simple_light();
//    hitable *world = test();
//    hitable *world = cornell_box();
//    hitable *world = cornell_balls();
//    hitable *world = cornell_smoke();
//    hitable *world = cornell_final();
    hitable *world = final();

    random_device rd;

    ofstream OutFile("Test.ppm"); //利用构造函数创建txt文本，并且打开该文本

    OutFile << "P3\n" << nx << " " << ny << "\n255\n";

    for (int j = ny - 1; j >= 0; j--) {
        str = "";
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);

            for (int s = 0; s < ns; s++) {
                float u = float(i+drand48())/ float(nx);
                float v = float(j+drand48())/ float(ny);

                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                vec3 temp = color(r, world, 0);
                temp = de_nan(temp);
                col += temp;
            }
            // color 取均值
            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);

            // r,g,b值可能超过255，当超过255的时候，ppm默认为%255
            ir = ir>255?255:ir;
            ig = ig>255?255:ig;
            ib = ib>255?255:ib;

            string s = to_string(ir) + " " + to_string(ig) + " " + to_string(ib) + "\n";
            str += s;
        }
        OutFile << str;
        cout<< "lllll: " << j <<"/1000"<<endl;
    }

    OutFile.close();            //关闭Test.txt文件

}
