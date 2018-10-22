//
// Created by SeeKHit on 2018/8/10.
//

#ifndef PETER_SHIRLEY_PROJECT_CODE_HITABLE_H
#define PETER_SHIRLEY_PROJECT_CODE_HITABLE_H

#include "ray.h"
#include "aabb.h"

class material;

void get_sphere_uv(const vec3& p, float& u, float& v) {
    float phi = atan2(p.z(), p.x());
    float theta = asin(p.y());
    u = 1-(phi + M_PI) / (2*M_PI);
    v = (theta + M_PI/2) / M_PI;
}

struct hit_record
{
    float t;
    float u;
    float v;
    vec3 p;
    vec3 normal;
    material *mat_ptr;
};

class hitable
{
public:
    virtual bool hit(const ray& r,float t_min,float t_max,hit_record & rec)const =0;
    virtual bool bounding_box(float t0,float t1,aabb & box)const =0;
};

// 翻转法向量
class flip_normals : public hitable {
public:
    flip_normals(hitable *p) : ptr(p) {}
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        if (ptr->hit(r, t_min, t_max, rec)) {
            rec.normal = -rec.normal;
            return true;
        }
        else
            return false;
    }
    virtual bool bounding_box(float t0, float t1, aabb& box) const {
        return ptr->bounding_box(t0, t1, box);
    }
    hitable *ptr;
};

#endif //PETER_SHIRLEY_PROJECT_CODE_HITABLE_H
