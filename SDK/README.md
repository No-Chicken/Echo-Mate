# EchoMate - SDK

## :ledger: 获取SDK

你可以使用luckfox的原始仓库的SDK，但是需要改一些东西，例如：`.dts`, `.mk`,  `build.sh`, `insmod_wifi.sh`, `kernal config`, `buildroot config`等，luckfox的获取如下：

```shell
git clone https://github.com/LuckfoxTECH/luckfox-pico.git
```

当然你也可以直接使用本仓库改好的SDK，如下：

```shell
git clone https://github.com/No-Chicken/Echo-Mate.git
cd ./SDK/luckfox-pico-sdk
```

## :black_nib: 更改luckfox原始SDK

### 使用本仓库改过的SDK可跳过以下操作：

1. 首先需要修改设备树，因为需要使用到wifi以方便用户ssh或者连wifi运行demo，修改`<SDK路径>/sysdrv/source/kernel/arch/arm/boot/dts/rv1103g-luckfox-pico-plus.dts`

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

2. 板载配置需要使能WIFI，在`<SDK路径>/project/cfg/BoardConfig_IPC/BoardConfig-SD_CARD-Buildroot-RV1103_Luckfox_Pico_Plus-IPC.mk`加入如下语句

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

6. 修改`kernel config`，需要使能rtl8723的drive

7. <p align="center">
   	<img border="1px" width="60%" src="./images/kernel config-rtl8723.jpg">
   </p>

8. 修改`buildroot config`，加入你需要的包，例如`iftop`，`wpa_supplicant`等

## :black_nib: 编译

