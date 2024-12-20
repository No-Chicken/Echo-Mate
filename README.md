<h1 align="center">Echo-Mate</h1>

<br>

## 1. Overview

这是一个基于rv1106，可以陪你聊天，翻译，能跑视觉，能够二次开发，小巧的linux桌面助手和开发板~

<br>

## 2. 开发环境

推荐使用`ubuntu22.04 LTS` ，首先拉下整个项目：

```shell
git clone https://github.com/No-Chicken/Echo-Mate.git
cd Echo-Mate
git submodule update --init --recursive
```
如果要更新子模块，请执行：

```shell
git submodule update --remote --merge --recursive
```

SDK开发环境与系统配置详见SDK文件夹中的[README.md](./SDK/README.md).

<br>

## 3. 功能演示

todo

<br>

## 4. 开发板使用

注：登录账号和密码，改过的SDK都设置为了`root`，如果需要改密码，除了常规的在buildroot deconfig里面更改，还需要在 <rv1106-sdk>/sysdrv/tools/board/buildroot/shadow_defconfig修改你的密码计算哈希值，再编译。


### 4.1 如何使用WIFI：

1. 开启wifi

   ```
   ifconfig wlan0 up
   ```

2. 进入wpa conf，`vi /etc/wpa_supplicant.conf`，配置wifi名和密码。

   ```bash
   ctrl_interface=/var/run/wpa_supplicant
   ap_scan=1
   update_config=1
   
   network={
           ssid="wifi-name"
           psk="12345678"
           key_mgmt=WPA-PSK
   }
   ```

3. 创建一个socket文件

   ```bash
   mkdir -p /var/run/wpa_supplicant
   ```

4. 然后使用`wpa_supplicant -B -c /etc/wpa_supplicant.conf -i wlan0`连接wifi，然后需要等待一会，会输出以下内容：

   ```bash
   [root@root ]# wpa_supplicant -B -c /etc/wpa_supplicant.conf -i wlan0
   
   Successfully initialized wpa_supplicant
   rfkill: Cannot open RFKILL control device
   [  670.124975] RTL8723BS: rtw_set_802_11_connect(wlan0)  fw_state = 0x00000008
   [  678.988193] RTL8723BS: rtw_set_802_11_connect(wlan0)  fw_state = 0x00000008
   [  688.127631] RTL8723BS: rtw_set_802_11_connect(wlan0)  fw_state = 0x00000008
   [  697.804890] RTL8723BS: rtw_set_802_11_connect(wlan0)  fw_state = 0x00000008
   [  698.446240] RTL8723BS: start auth
   [  698.466241] RTL8723BS: auth success, start assoc
   [  698.521065] RTL8723BS: rtw_cfg80211_indicate_connect(wlan0) BSS not found !!
   [  698.521119] RTL8723BS: assoc success
   [  698.598174] RTL8723BS: send eapol packet
   [  698.643221] RTL8723BS: send eapol packet
   [  698.644951] RTL8723BS: set pairwise key camid:4, addr:9e:a4:d3:f5:da:8d, kid:0, type:AES
   [  698.647953] RTL8723BS: set group key camid:5, addr:9e:a4:d3:f5:da:8d, kid:1, type:AES
   ```

5. 配置IP

   ```bash
   udhcpc -i wlan0
   ```

6. 然后你就可以ping一下baidu等网站测下网络了

7. 如果想要切换`WiFi`，需要重启 `wpa_supplicant` 服务，需要运行

   ```bash
   killall -9 wpa_supplicant 
   ```

<br>

### 4.2 如何传输文件：

1. 使用SSH
   ```bash
   # 传输文件
   scp ./send_file.txt root@172.32.0.93:/root
   # 传输文件夹
   scp -r ./send_files root@172.32.0.93:/root
   ```

2. 其他
   samba, ADB这些详见网上教程
   
<br>

### 4.3 如何测试屏幕：

1. 调节背光
   ```bash
   echo 49 > /sys/class/backlight/backlight/brightness
   ```

2. 测试花屏和清屏
   ```bash
   cat /dev/urandom > /dev/fb0
   cat /dev/zero > /dev/fb0
   ```
   