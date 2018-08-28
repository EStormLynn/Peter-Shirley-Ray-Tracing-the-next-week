# Peter Shirley-Ray Tracing the next week(2016)

原著：Peter Shirley


[英文原著地址](https://pan.baidu.com/s/1b5CvAdElCcXAO2R4lNFgkA)  密码: urji

[Github地址](https://github.com/EStormLynn/Peter-Shirley-Ray-Tracing-in-one-weenkend)



## 目录：
- [ ] Chapter1:Motion Blur
- [ ] Chapter2:Bounding Volume Hierarchies
- [ ] Chapter3:Solid Textures
- [ ] Chapter4:Perlin Noise
- [ ] Chapter5:Image Texxture Mapping
- [ ] Chapter6:Rectangles and Lights
- [ ] Chapter7:Instances
- [ ] Chapter8:Volumes
- [ ] Chapter9:A Scene Test All New Features

## Chapter1:Output an image
使用ppm渲染到图片

```c++
#include <iostream>

using namespace std;


int main()
{
    int nx =200;
    int ny=100;
    cout<<"P3\n"<<nx<<" "<<ny<<"\n255\n";
    for(int j=ny-1;j>=0;j--)
    {
        for(int i=0;i<nx;i++)
        {
            float r=float(i)/float(nx);
            float g=float(j)/float(ny);
            float b=0.2;

            int ir=int(255.99*r);
            int ig=int(255.99*g);
            int ib=int(255.99*b);
            cout<<ir<<" "ig<<" "<<ib<<"\n";
        }
    }
}
```
说明：
* 像素从左往右打印
* 从上向下打印
* 这个例子中RGB计算出来在[0,1]之间，输出之前映射到一个高范围空间
* 红+绿=黄
* 打印的内容保存成.ppm格式即可预览

<div align=center><img src="http://oo8jzybo8.bkt.clouddn.com/Chapter01_Output%20an%20image.png" width="400" height="200" alt=""/></div>
