<h1 align="center">Echo-Mate</h1>

<br>

## 1. Overview

这是一个基于rv1106，可以陪你聊天，翻译，能跑视觉，能够二次开发，小巧的linux桌面助手和开发板~

<br>

## 2. 功能演示

1. 功能演示



2. Desk-Bot demo 功能表

   <p align="center">
      <img border="1px" width="75%" src="./assets/func_map.png">
   </p>

3. 

<br>

## 3. 开发环境

推荐使用`ubuntu22.04 LTS` ，首先拉下整个项目：

```shell
git clone https://github.com/No-Chicken/Echo-Mate.git
cd Echo-Mate
git submodule update --init --recursive
```
如果要递归更新所有子模块，请执行：

```shell
git submodule update --remote --merge --recursive
```

SDK开发环境与系统配置详见SDK文件夹中的[README.md](./SDK/README.md).

Demo如何烧录使用详见Demo文件夹的[README.md](./Demo/README.md).

<br>

## 4. 仓库目录说明

```
Echo-Mate/
├── Demo/                  # Echo开发板的demo
│   ├── DeskBot_demo/      # AI桌面机器人
│   └── xxx_demo/          # xxx对应的独立子demo
├── SDK/                   # SDK文件夹
│   ├── rv1106-sdk/        # 基于luckfox的SDK
│   └── README             # SDK和开发板使用相关说明
```