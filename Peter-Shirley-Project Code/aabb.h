//
// Created by SeeKHit on 2018/9/5.
//

#ifndef PETER_SHIRLEY_PROJECT_CODE_AABB_H
#define PETER_SHIRLEY_PROJECT_CODE_AABB_H

#include "vec3.h"
#include "ray.h"

inline float ffmin(float a, float b)
{
    return a<b?a:b;
}

inline float ffmax(float a, float b)
{
    return a>b?a:b;
}

class aabb
{
public:
    aabb(){}
    aabb(const vec3 a,const vec3 &b)
    {
        _min = a;_max = b;
    }

    vec3 min()const{ return  _min};
    vec3 max()const{ return  _max};

    bool hit(const ray& r,float tmin,float tmax)const
    {
        for(int a =0;a<3;a++)
        {
            float invD = 1.0f/r.direction()[a];
            float t0 = (min()[a] - r.direction()[a]) * invD;
            float t1 = (max()[a] - r.direction()[a]) * invD;

            if(invD<0.0f)
                std::swap(t0,t1);
            tmin = t0>tmin?t0:tmin;
            tmax = t1<tmax?t1:tmax;
            if(tmax <= tmin)
                return false;
        }
        return true;
    }
    vec3 _min;
    vec3 _max;
};

aabb surrounding_box(aabb box0, aabb box1) {
    vec3 small( fmin(box0.min().x(), box1.min().x()),
                fmin(box0.min().y(), box1.min().y()),
                fmin(box0.min().z(), box1.min().z()));
    vec3 big  ( fmax(box0.max().x(), box1.max().x()),
                fmax(box0.max().y(), box1.max().y()),
                fmax(box0.max().z(), box1.max().z()));
    return aabb(small,big);
}

#endif //PETER_SHIRLEY_PROJECT_CODE_AABB_H
