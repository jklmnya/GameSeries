#
## 1. VAO, VBO, EBO
```
VAO : Vertex Array Object
VBO : Vertex Buffer Object
EBO : Element Buffer Object
```
## 2. OpenGL图形渲染管线
![Image text](Imgs\OpenGL_Graphics_Pipeline.png)
```
原图装配：将顶点着色器的输出的所有顶点作为输入，将所有点装配成指定原图的形状

片段着色器：将每个像色运用Shader着色
```
## 3. 纹理坐标（Texture Coordinate）
### 3.1 每个顶点可以持有一个纹理坐标，通常用来表示从纹理图像的哪一个部分采样
![Image text](Imgs\Texture_Coord.png)
```
1. 纹理坐标范围[0, 1]
2. 2D纹理坐标起始于(0, 0)
```
#
## 4. 纹理过滤
### 4.1 纹理放大
![Image text](Imgs\GL_NEAREST.png)
```
GL_NEAREST : 选择中心点最接近纹理坐标的那个像素
```
![Image text](Imgs\GL_LINEAR.png)
```
GL_LINEAR : 双线性插值，使用附近的像素颜色做插值
```

## 5. Mipmap
### 5.1 远处的物体使用缩小后的纹理
![Image text](Imgs\Mipmap.png)

## 6. 齐次坐标（Homogeneous Coordinates）
### 6.1 vec = (x, y, z, 1), 使用一个变换矩阵就可以完成（缩放，旋转等）与平移的组合变换

## 7. 坐标转化
![Image text](Imgs\Coordinate_Systems.png)
```
Model Mat : 物体在世界空间中如何摆放
View Mat  : 摄像机如何观察物体
Projection Mat : 指定一个坐标范围（在每个维度上从-1000到1000），投影矩阵会将这个范围内的坐标变换成标准化设备坐标范围（-1， 1）。超出坐标范围的物体将会被裁剪

Viewport Transform : 将标准化设备坐标转化为显示窗口坐标（原点(0, 0))
```