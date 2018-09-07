//
// Created by SeeKHit on 2018/8/10.
//

#ifndef PETER_SHIRLEY_PROJECT_CODE_HITABLE_H
#define PETER_SHIRLEY_PROJECT_CODE_HITABLE_H

#include "ray.h"
#include "aabb.h"

class material;

struct hit_record
{
    float t;
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


#endif //PETER_SHIRLEY_PROJECT_CODE_HITABLE_H
