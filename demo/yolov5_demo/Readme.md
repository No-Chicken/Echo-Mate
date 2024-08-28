# Yolov5 物体识别例程

## 文件结构

```bash
yolov5_demo
├── cpp --------------------------------------例程源码
├── demo -------------------------------------编译生成后的文件
└── model ------------------------------------例程使用的模型源码
```

## 编译

+ 设置环境变量

```bash
export SDK_PATH=< Sdk 地址>
```

**注意**：使用绝对地址。

+ 执行

```bash
cd cpp/build
cmake ..
make -j
make install
```

        编译完成后在 `cpp` 文件夹下会生成 `rv1106_yolov5_demo` 文件夹，移动至开发板运行。

## 运行

+ 开发板进入工程文件，配置执行权限

```bash
cd rv1106_yolov5_demo
chmod a+x ./rknn_yolov5_demo
```

+ 执行

```bash
./rknn_yolov5_demo <yolov5模型> 
```

+ 示例

```bash
./rknn_yolov5_demo ./model/yolov5.rknn
```

## 效果

        摄像头获取图像，经过模型推理和 opencv-mobile 图像处理后在 Pico-LCD-1.3 上显示，会框住摄像头捕获到的物体并标注识别种类和置信度。
