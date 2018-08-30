# Peter Shirley-Ray Tracing the next week(2016)

原著：Peter Shirley


[英文原著地址](https://pan.baidu.com/s/1b5CvAdElCcXAO2R4lNFgkA)  密码: urji


## 目录：
- [x] Chapter1:Motion Blur
- [ ] Chapter2:Bounding Volume Hierarchies
- [ ] Chapter3:Solid Textures
- [ ] Chapter4:Perlin Noise
- [ ] Chapter5:Image Texxture Mapping
- [ ] Chapter6:Rectangles and Lights
- [ ] Chapter7:Instances
- [ ] Chapter8:Volumes
- [ ] Chapter9:A Scene Test All New Features

## Chapter1:Motion Blur
运动模糊。当你在进行ray tracing的时候，模糊反射和散焦模糊的过程中，每个像素你需要采样多个点，来决定最终像素的颜色，这种效果在现实世界中是另外一种实现方法，现实世界中，相机通过控制快门的开和关，记录下快门开闭时间内，物体运动的轨迹，通过这样的方法实现模糊的效果。

运动模糊的意思是，现实世界中，相机快门开启的时间间隔内，相机活着物体发生了位移，画面最后呈现出来的像素，是移动过程中像素的平均值。我们可以通过随机一条光线持续的时间，最后计算出像素平均的颜色，这也是光线追踪使用了很多随机性的方法，最后的画面接近真实世界的原因。

这种方法的基础是当快门开机的时间段内，随机时间点生成光线，修改之前的ray类，添加一个光线存在时间的变量。


```c++
class ray
{
public:
    ray(){}
    ray(const vec3& a, const vec3 & b){ A =a; B = b;}
    vec3 origin() const  { return  A;}
    vec3 direction() const { return  B;}
    vec3 point_at_parameter(float t) const { return A+t*B;}

    float time() const{ return  _time};
    vec3 A;
    vec3 B;
    float _time;

};
```

接下来就是控制camera在时间t1和t2之间，随机时间点生成光线，给camera类添加个float 的时间变量，记录光线产生的时间。

```C++

class camera
{
    vec3 origin;
    vec3 u,v,w;
    vec3 horizontal;
    vec3 vertical;
    vec3 lower_left_corner;
    float len_radius;
    float time0,time1;

public :
    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist,
    float t0,float t1)
    {
        time0 = t0;
        time1 = t1;
        len_radius = aperture/2;
        float theta = vfov*M_PI/180;
        float half_height = tan(theta/2);
        float half_width = aspect * half_height;
        origin = lookfrom;

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w,u);

        lower_left_corner = origin - half_width*focus_dist*u - half_height*focus_dist*v - focus_dist*w;
        horizontal = 2*half_width*focus_dist*u;
        vertical = 2*half_height*focus_dist*v;
    }

    ray get_ray(float s,float t)
    {
        vec3 rd = len_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() +v*rd.y();
        float time = time0 + drand48()*(time1 - time0);
        return ray(origin + offset,lower_left_corner+s*horizontal + t*vertical - origin - offset,time);
    }

    vec3 random_in_unit_disk()
    {
        vec3 p;
        do{
            p = 2.0*vec3(drand48(),drand48(),0)-vec3(1,1,0);
        }while (dot(p,p)>=1.0);
        return p;
    }

};
```

关于如何移动物体，接下来创建一个移动球体的类，里面存储某个球在时间点t0从位置center0，移动到时间点t1的位置center1.这个时间间隔不用和相机快门的时间长短相同。

```C++
class moving_sphere:public  hitable
{
public:
    moving_sphere(){}
    moving_sphere(vec3 cen0,vec3 cen1,float t0,float t1,float r,material *m):
            center0(cen0),center1(cen1),time0(t0),time1(t1),radius(r),mat_ptr(m) {};

    virtual  bool hit(const ray&r,float tmin, float tmax, hit_record& rec) const;

    vec3 center(float time) const;
    vec3 center0,center1;
    float time0,time1;
    float radius;
    material *mat_ptr;
};

// 当前时间点，球心的位置
vec3 moving_sphere::center(float time) const {
    return center0 + ((time-time0)/(time1-time0))*(center1-center0);
}
```

重写moving_sphere的hit函数，修改之前的center为一个时间相关的位置
```C++
bool moving_sphere::hit(const ray& r,float t_min,float t_max,hit_record & rec )const
{
    // 修改之前的center为一个时间相关的位置
    vec3 oc = r.origin() - center(r.time());
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant))/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}
```

最后修改上本书最后绘制的场景，小球在time=0的时候，在原来的位置，time=1的时候，移动到center+vec3(0,0.5*drand48(),0)位置，在此pre期间，光圈一直开启。

```C++

hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable *[n + 1];
    list[0] = new sphere(vec3(0, -700, 0), 700, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = drand48();
            vec3 center(a + 0.9 * drand48(), 0.2, b + 0.9 * drand48());
            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {  // diffuse
                    // 运动模糊的小球
                    list[i++] = new moving_sphere(center, center + vec3(0, 0.5 * drand48(), 0), 0.0, 1.0, 0.2,
                                                  new lambertian(vec3(drand48() * drand48(), drand48() * drand48(),
                                                                      drand48() * drand48())));
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
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(1, 1, 1), 0.0));

    return new hitable_list(list, i);
}
```

最后渲染出来达到的效果如下：


<div align=center><img src="http://oo8jzybo8.bkt.clouddn.com/Screen%20Shot%202018-08-31%20at%201.50.45%20AM.png" width="400" height="250" alt=""/></div>
