# 2.4.1 Ustack


#### 准备dpdk环境
```
# cd /home/king/share/dpdk/dpdk-stable-19.08.2
# export RTE_SDK=/home/king/share/dpdk/dpdk-stable-19.08.2/
# export RTE_TARGET=x86_64-native-linux-gcc
# ./usertools/dpdk-setup.sh
从43到49都需要执行。备注，网卡 ifconfig eth0 down
```
![Alt text](https://disk.0voice.com/p/mD)

如果出现以下现象，需要增加内存
![Alt text](https://disk.0voice.com/p/mE)

成功
![Alt text](https://disk.0voice.com/p/mv)

![success](https://disk.0voice.com/p/mZ)

更好源地址与目的地址
![code](https://disk.0voice.com/p/Ok)


length: 7424, content: Hello Udp Server King
7424十进制，16进制 1D00。
ntohs()以后，001D, 29. 