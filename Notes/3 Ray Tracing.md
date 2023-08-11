# GAMES 101：现代计算机图形学入门

## Ray Tracing部分

课程网站：https://sites.cs.ucsb.edu/~lingqi/teaching/games101.html

本课程将全面而系统地介绍现代计算机图形学的四大组成部分：

1. 光栅化成像
2. 几何表示
3. **光的传播理论**
4. 动画与模拟。

每个方面都会从基础原理出发讲解到实际应用，并介绍前沿的理论研究。通过本课程，你可以学习到计算机图形学背后的数学和物理知识，并锻炼实际的编程能力。

## Lecture 13 Ray Tracing 1

###### Whitted- Style Ray Tracing

光栅化不能很好的解决全局（global）效果：

- （软）阴影
- 光的多次反射：Glossy Reflection、Indirect illumination

光栅化很快，但是质量相对较低，用作实时渲染；光线追踪更准确，但是速度非常慢，用作offline渲染。

### Basic Ray-Tracing Algorithm

**Light Rays**：

1. 光线沿直线传播。
2. 光线之间不会发生碰撞，不会相互影响。
3. 光线从光源出发，经过不断反射后进入相机（光路可逆，相机也可以追踪到光源）。

**光线投射**（Ray Casting）：

1. Generating Eye Rays：从相机出发，对屏幕某一像素点发射一条光线，找到和场景物体最近的交点，即第一折射点。
2. Shading Pixels（Local Only）：通过入射方向、光源方向、法线方向计算着色，写入像素。

**递归光线投射（Whitted- Style）**：

把所有折射、反射点的着色值都写入此像素中。

![11](./image/11.png)

### Ray-Surface Intersection

**光线**（Ray）：一个起点（origin）和一个方向向量（direction）。

**光线方程**（Ray Equation）：沿光线上的任意一点 $\mathbf{r}(t) = \mathbf{o} + t\mathbf{d},\quad t\in[0,+\infty)$

#### 光线和球体求交

![12](./image/12.png)

#### 光线和隐式物体（Implicit Surface）求交

从上面方法推广：

![13](./image/13.png)

#### 光线和显式物体（Triangle Mesh）求交

对每个三角形面：

1. 考虑空间中光线和三角形求交：
   1. 判断光线和平面求交。
   2. 判断交点是否在三角形内。
2. 取最近的hit点（t最小）

**平面方程**（Plane Equation）：一个法向量（N）和平面内任意一点（p'）。对平面上任意一点p， $(\mathbf{p} - \mathbf{p}') \cdot \mathbf{N} = 0$

##### 光线和平面求交

![14](./image/14.png)

##### Moller Trubore Algorithm

使用重心坐标，快速进行光线和三角形求交。即**光线方程==重心坐标**：

![15](./image/15.png)

要求 $t, b_1, b_2, 1-b_1-b_2 \geq 0$ 。

#### Accelerating Ray-Surface Intersection 加速结构（AABB）

##### Bounding Volumes 包围盒

对于复杂物体，使用简单图形将其包裹。

如果一个光线碰不到包围盒，那也碰不到里面的物体。

##### Ray-Intersection With Box

长方体（box）是三对平行面的交集。

通常使用**轴对齐包围盒**（Axis-Aligned Bounding Box，AABB）。

##### 光线和AABB求交

###### 2D

2D矩形为两对平行平面的交集，可以分别求两对平行平面的$t_{min},t_{max}$ ，对两对t值求交集：

![16](./image/16.png)

###### 3D

**与上述求交集相似，核心思路是：**

- 只有光线全部进入三对平行面形成的区域，光线才进入包围盒。
- 只要光线离开某一个区域，光线就离开了包围盒。

**所以求交步骤为：**

1. 计算每个对面的$t_{min},t_{max}$。
2. $t_{enter}=max\{t_{min}\}, t_{exit} = min\{t_{max}\}$。
3. 如果$t_{enter} < t_{exit}$，则光线与包围盒可能有交点，但是需要判断以下情况：
   - 如果$t_{exit} < 0$，则光线离开时间为负数，一定没有交点
   - 如果$t_{exit} >= 0, t_{enter} < 0$，则光线在盒子内，一定有交点。
4. 所以当且仅当$t_{enter} < t_{exit}, t_{exit} >= 0$ 时，光线和包围盒有交点。

**使用轴对齐的原因是，可以将光线和平面求交的方程简化：**

![17](./image/17.png)

## Lecture 14 Ray Tracing 2

###### Acceleration & Radiometry
