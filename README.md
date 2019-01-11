[![star](https://gitee.com/xhzhou/CloudViewer/badge/star.svg?theme=dark)](https://gitee.com/xhzhou/CloudViewer/stargazers)

# CloudViewer
该项目基于https://github.com/nightn/CloudViewer 。如果CloudViewer对你有帮助，还请给原作者star。

该项目只是修改相关配置文件，仅为自我学习之用。

## 环境

1. vs2015 
2. pcl1.8.0 
3. qt5.8 
4. VTK 7.0

## 配置文件

该项目为了方便，使用配置文件进行环境配置，见`QT.pros `,`PCL.props`

### QT.pros

修改`QTRoot`路径

![图1](http://ws2.sinaimg.cn/large/6e529308gy1ftu1nkdx6nj214y0mwn67.jpg)

### PCL.props

修改`PCL_ROOT`和`VTK_ROOT`路径及对应`lib`库及`bin`文件路径

修改对应版本号

![图2](http://wx1.sinaimg.cn/large/6e529308gy1ftu312wsfkj21450qaguw.jpg)

### VTK

该项目回用到VTK，可以自行编译或者使用我编译好的`VTK-7.0`版本，注意修改图2中的相关路径

[VTK7.0-google drive](https://drive.google.com/open?id=1DStneyGnaUUj4kPJVCGOA985yimOcX1o)

[VTK7.0-百度网盘](https://pan.baidu.com/s/1yjFOP6Ako-MMpnrDX597eg)  密码: 68th

## 编译运行即可

![图3](http://ws4.sinaimg.cn/large/6e529308gy1ftu35x4i13j21hc0teaey.jpg)