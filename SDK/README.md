# Echo Mate - SDK

</br>

## :ledger: 1. 获取SDK

你可以使用luckfox的原始仓库的SDK，但是需要改一些东西，例如：`.dts`, `.mk`,  `build.sh`, `insmod_wifi.sh`, `kernal config`, `buildroot config`等，luckfox的获取如下：

```shell
git clone https://github.com/LuckfoxTECH/luckfox-pico.git
```

当然你也可以直接使用本仓库改好的SDK，如下：

```shell
git clone https://github.com/No-Chicken/Echo-Mate.git
cd ./SDK/luckfox-pico-sdk
```

</br>

## 📥2. 安装依赖

```shell
sudo apt-get install repo git ssh make gcc gcc-multilib g++-multilib module-assistant expect g++ gawk texinfo libssl-dev bison flex fakeroot cmake unzip gperf autoconf device-tree-compiler libncurses5-dev pkg-config
```

</br>

## :black_nib: 3. 更改luckfox原始SDK

### 注：使用本仓库改过的SDK可跳过以下操作：

1. 首先需要修改设备树，因为需要使用到wifi以方便用户ssh或者连wifi运行demo，修改`<SDK路径>/sysdrv/source/kernel/arch/arm/boot/dts/rv1106g-luckfox-pico-pro-max.dts`

   ```dts
   /***********mmc interface for wifi map to sdmmc1 set for sdio mode**********/
   &sdio {
   	max-frequency = <50000000>; // 最大运行频率不超过 150Mhz; SDIO2.0 卡最大 50M，SDIO3.0 最大支持 150M
   	bus-width = <4>;             // 4线数据模式
   	cap-sd-highspeed;            // 此配置同 SD 卡功能，作为 SDIO 外设，也有区分是否为 highspeed 的 SDIO 外设。
   	cap-sdio-irq;                // 此配置标识该 SDIO 外设(通常是 Wifi)是否支持 sdio 中断
   	keep-power-in-suspend;       // 此配置表示是否支持睡眠不断电，请默认加入该选项。Wifi 一般都有深度唤醒的要求。
   	non-removable;               // 此项表示该插槽为不可移动设备且此项为 SDIO 设备必须添加项。
   	sd-uhs-sdr50;         
   	no-1-8-v;
   	supports-sdio;         // 标识此插槽为 SDIO 功能，为必须添加项。否则无法初始化 SDIO 外设。
   	pinctrl-names = "default";
   	pinctrl-0 = <&sdmmc1m1_clk &sdmmc1m1_cmd &sdmmc1m1_bus4>; // rv1106-pinctrl.dtsi
   	status = "okay";
   };
   ```

2. 板载配置需要使能WIFI，在`<SDK路径>/project/cfg/BoardConfig_IPC/BoardConfig-SD_CARD-Buildroot-RV1106_Luckfox_Pico_Pro_Max-IPC.mk`加入如下语句

   ```sh
   # enable external wifi module
   export RK_ENABLE_WIFI=y
   ```

3. 在`<SDK路径>/build.sh`中，build_app函数中的构建和导出meta配置改为如下：

   ```sh
   function build_app() {
   	# ......
   	# 省略前面
   	echo "============Start building app============"
   	echo "TARGET_APP_CONFIG=$RK_APP_DEFCONFIG $RK_APP_DEFCONFIG_FRAGMENT $RK_APP_TYPE"
   	echo "========================================="
   
   	# build_meta --export # export meta header files
   	build_meta --export --media_dir $RK_PROJECT_PATH_MEDIA # for rtl8723bs
   	test -d ${SDK_APP_DIR} && make -C ${SDK_APP_DIR}
   
   	finish_build
   }
   ```

4. 修改`<SDK路径>/sysdrv/drv_ko/wifi/insmod_wifi.sh`，这个是会自动导入到开发板的文件系统中的，用于安装wifi的mod，在里面加入rtl8723bs的安装：

   ```sh
   # rtl8723bs
   cat /sys/bus/sdio/devices/*/uevent | grep "024C:B723"
   if [ $? -eq 0 ];then
   	insmod  cfg80211.ko
   	insmod  r8723bs.ko
   fi
   ```

5. 在`<SDK路径>/`下加入`external`文件夹放置rtl8723的bin文件，详见本仓库的加入的东西，直接复制即可；

6. 进入`<SDK路径>/sysdrv/source/kernel`，修改`kernel config`，需要使能rtl8723的drive

   ```shell
   make ARCH=arm menuconfig
   ```

7. <p align="center">
   	<img border="1px" width="60%" src="./assets/kernel config-rtl8723.jpg">
   </p>

   

   然后保存

   ```shell
   make ARCH=arm savedefconfig
   ```

8. 修改`buildroot config`，加入你需要的包，例如`iftop`，`wpa_supplicant`等同样也是设置再保存

   ```shell
   make menuconfig
   make savedefconfig
   ```

</br>

## 🔨4. 编译

最好详细查看luckfox的README文档。

1. 首先需要在SDK文件夹选择板级配置，这里选择`[7]custom`，会弹出所有的`.mk`文件，选择echo mate的配置即可，当然你也可以选择直接用改过的原配置文件

   ```shell
   ./build.sh lunch
   ```

2. 一键自动编译

   ```shell
   ./build.sh 
   ```

3. `<SDK目录>/build.sh`全部可用选项：

   ```shell
   Usage: build.sh [OPTIONS]
   Available options:
   lunch              -Select Board Configure
   env                -build env
   meta               -build meta (optional)
   uboot              -build uboot
   kernel             -build kernel
   rootfs             -build rootfs
   driver             -build kernel's drivers
   sysdrv             -build uboot, kernel, rootfs
   media              -build rockchip media libraries
   app                -build app
   recovery           -build recovery
   tool               -build tool
   updateimg          -build update image
   unpackimg          -unpack update image
   factory            -build factory image
   all                -build uboot, kernel, rootfs, recovery image
   allsave            -build all & firmware & save
   
   clean              -clean all
   clean uboot        -clean uboot
   clean kernel       -clean kernel
   clean driver       -clean driver
   clean rootfs       -clean rootfs
   clean sysdrv       -clean uboot/kernel/rootfs
   clean media        -clean rockchip media libraries
   clean app          -clean app
   clean recovery     -clean recovery
   
   firmware           -pack all the image we need to boot up system
   ota                -pack update_ota.tar
   save               -save images, patches, commands used to debug
   check              -check the environment of building
   info               -see the current board building information
   
   buildrootconfig    -config buildroot and save defconfig"
   kernelconfig       -config kernel and save defconfig"
   ```

</br>

## 📥5. 烧录

1. 首先下载并打开瑞芯微的SocToolKit，进入，选择RV1106

<p align="center">
      	<img border="1px" width="75%" src="./assets/SocToolKit-select.jpg">
</p>



2. 把output中的镜像文件如下，烧录到空白的SD卡。SD卡格式化可以使用`SD Card Formatter`。

<p align="center">
      	<img border="1px" width="75%" src="./assets/烧录SD.jpg">
</p>


</br>


