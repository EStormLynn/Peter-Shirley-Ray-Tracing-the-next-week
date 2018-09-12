//
// Created by SeeKHit on 2018/9/13.
//

#ifndef PETER_SHIRLEY_PROJECT_CODE_TEXTURE_H
#define PETER_SHIRLEY_PROJECT_CODE_TEXTURE_H

#include "vec3.h"

class texture {
public:
    virtual vec3 value(float u, float v, const vec3 &p) const = 0;
};

class constant_texture : public texture {
public:
    constant_texture() {}

    constant_texture(vec3 c) : color(c) {}

    virtual vec3 value(float u, float v, const vec3 &p) const {
        return color;
    }

    vec3 color;
};

// 棋盘纹理
class checker_texture:public texture
{
public:
    checker_texture(){}
    checker_texture(texture *t0,texture *t1):even(t0),odd(t1){}
    virtual vec3 value (float u,float v, const vec3 &p)const {
        float sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
        if(sines<0)
            return odd->value(u,v,p);
        else
            return  even->value(u,v,p);
    }

    texture *odd;
    texture *even;
};

#endif //PETER_SHIRLEY_PROJECT_CODE_TEXTURE_H
