---
layout: page
title: Linux Cheatsheet
---
Here is a Cheatsheet of various linux tips and tricks I accumulated through the time.<br/>
It is mostly a tool for myself, but figures it might benefits other people.

# Tips and tricks

## Echo byte in file
To write hex<br/>
`echo -n -e '\xbc\x00' > failures`<br/>
To display hex<br/>
`xxd failures`<br/>
<hr/>

## Search for pattern inside a file
`grep -rnw '/path/to/search' -e 'patern'`<br/>
<hr/>

## Remount as rw
`mount -o remount rw /`<br/>
<hr/>

## Stream ethernet data
From<br/>
`pv --rate-limit 100M </dev/urandom | nc 192.168.1.42 7777`<br/>
To<br/>
`nc -l 7777 > nc.txt`<br/>
<hr/>

## Create a new serial console
add `ttyUSB0::respawn:/sbin/getty -L  ttyUSB0 115200 vt100` in /etc/inittab<br/>
Another way is to bind a console on the fly with : `setsid /sbin/getty -L  ttyUSB2 115200 vt100`<br/>
<hr/>

## View disk usage (RW)
`cat /proc/diskstats`<br/>
Ex. output:<br/>
179       0 mmcblk0 255 326 19542 767 25 21 92 111 0 640 710 0 0 0 0<br/>
field 1 (255) = reads<br/>
field 7 (92) = writes<br/>

To monitor what is written<br/>
`echo 1 > /proc/sys/vm/block_dump`

To see and example of output<br/>
`echo hello > world.txt`<br/>
`dmesg`<br/>

To see all, reboot and<br/>
`dmesg | grep dirtied | grep "on vda" | sort`
<hr/>

## Flash ftdi eeprom
`sudo apt install ftdi-eeprom`<br/>
to read eeprom, create a file such as ftdi_eeprom.conf with<br/>
```
##################################################
filename="eeprom.copy"  # Filename to save eemrpon config
eeprom_type=3           # number three is for type R chips
flash_raw=true          # raw copy of unmodified image
##################################################
```
<br/>
Find the vendor and product code with `lsusb`<br/>
Then run `sudo ftdi_eeprom --device i:0x0403:0x6011 --read-eeprom ftdi_eeprom.conf`<br/>
<hr/>

## Edit a flash rootfs
This [guide](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18842473/Build+and+Modify+a+Rootfs) covers how to extract, mount compress and use mkimage to edit the rootfs<br/>
<hr/>

## list difference between two folder
`diff -qr --no-dereference /path/to/folder1 /path/to/folder2`<br/>
<hr/>

## Show what device a folder is on
`df /path/to/folder`<br/>
Or to get what device<br/>
`findmnt /path`<br/>
<hr/>

## Mount a folder in RAM
`mount -t tmpfs -o size=100M tmpfs /path/to/folder`<br/>
<hr/>

## Create a bind point of a path
This allow for acces to old bind point of a folder (Ex.: if tmpfs is mounted over a path)<br/>
`mount -o bind /source /destination`<br/>
<hr/>

## Analyse systemd boot sequence
`systemd-analyze plot > output.svg`<br/>
<hr/>

## Update time from web
`timedatectl set-ntp on`<br/>
`timedatectl set-ntp off`<br/>
<hr/>

## rsyslog severity
Severity<br/>
Num.Code 	Severity 	Description<br/>
0 			emerg 		system is unusable<br/>
1 			alert 		action must be taken immediately<br/>
2 			crit 		critical conditions<br/>
3 			error 		error conditions<br/>
4 			warning 	warning conditions<br/>
5 			notice 		normal but significant condition<br/>
6 			info 		informational messages<br/>
7 			debug 		debug-level messages<br/>
<hr/>

## link down and up
sudo ip link set dev eth1 down<br/>
sudo ip link set dev eth1 up<br/>
<hr/>

## Give a fixed interface name
create a rule such as<br/>
`/etc/systemd/network/10-my_eth.link`<br/>
with the config such as<br/>
```conf
[Match]
MACAddress=00:a0:de:63:7a:e6

[Link]
Name=my_eth
```
<br/>

To fix parameters, then edit /etc/dhcpcd.conf such as<br/>
```conf
interface my_eth
static ip_address=192.168.0.10/24
#static ip6_address=fd51:42f8:caae:d92e::ff/64
static routers=192.168.0.1
#static domain_name_servers=192.168.0.1 8.8.8.8 fd51:42f8:caae:d92e::1

```
<br/>
To set a priority, add in dhcpcd.conf (lower in first, 200 default, +100 if wifi)<br/>
```
interface my_eth
metric 400
```
<br/>
<hr/>

## get usb device info
`/bin/udevadm info --name=/dev/ttyUSB0 | grep "ID_USB_INTERFACE_NUM=00" `<br/>
`/bin/udevadm info --name=/dev/ttyUSB0 | grep "ID_SERIAL_SHORT=FT2NP4Z6"`<br/>
`/bin/udevadm info --name=/dev/ttyUSB0 | grep "DEVNAME"`<br/>
<hr/>

## How to prepare rootfs for read-only
https://wiki.debian.org/ReadonlyRoot<br/>
<hr/>

## Extract device tree
Run the raspberry pi without uboot so it can pass the complete devicetree<br/>
and extract it with:<br/>
`sudo dtc -I fs -O dtb /sys/firmware/devicetree/base -o device_tree.dtb`<br/>

If the source is needed:<br/>
`sudo dtc -I fs -O dts /sys/firmware/devicetree/base -o device_tree.dts`<br/>
<hr/>


## Install node version with package manager
Dowload  the package<br/>
`tar -xvf node-v12.16.1-linux-x64.tar.xz -C ~/nodejs --strip-components 1`<br/>
add to bashrc: (REPLACE <yourhome> with your actual username)<br/>
``` bash
if ! [[ $PATH =~ '/home/<yourhome>/nodejs/bin' ]]; then
    export PATH="$PATH:/home/<yourhome>/nodejs/bin"
fi
```

or just<br/>
`PATH="{PATH:+${PATH}:}/opt/nodejs/bin"`<br/>
<hr/>

## Bridge two network interface
How to enable port 80 from one interface to the other<br/>
```bash
sudo su
echo 1 > /proc/sys/net/ipv4/ip_forward
iptables -t nat -A PREROUTING -p tcp --dport 80 -j DNAT --to-destination 192.168.0.20:80
iptables -A FORWARD -i network_eth -o hydro_eth -p tcp --dport 80 -j ACCEPT
iptables -t nat -A POSTROUTING -o hydro_eth -j MASQUERADE
iptables-save | tee /etc/iptables.up.rules
```
<hr/>

## Connect to a VPN
`sudo apt install pptp-linux`<br/>
`sudo systemctl enable --now pppd-dns.service`<br/>

Create /etc/ppp/peers/my_vpn and add:<br/>
```bash
pty "/usr/sbin/pptp <HOST> --nolaunchpppd"
name <USERNAME>
password <PASSWORD>
remotename PPTP
require-mppe-128
require-mschap-v2
refuse-eap
refuse-pap
refuse-chap
refuse-mschap
noauth
debug
persist
maxfail 0
timout
defaultroute
replacedefaultroute
usepeerdns

```
Execute with `sudo pon my_vpn`<br/>

Debug with `sudo pon my_vpn debug dump logfd 2 nodetach`<br/>
Does not work if ufw is enabled!<br/>

Route a remote address as local with:<br/>
`sudo ip route add <IP_DESIRED>/32 dev ppp0`<br/>
<hr/>

## Mounting a socket as a serial port + bash terminal access through it
Mount locally the terminal with<br/>
`socat  pty,link=/tmp/virt,raw  tcp:<host>:<port>`<br/>
Then attach a screen session to it<br/>
`screen -A /tmp/virt`<br/>

If you want to listen to a local socket instead if a remote socket<br/>
`socat -d -d pty,link=/dev/virt,raw,echo=0 tcp-listen:<port>`<br/>

`-d -d` options add debug.<br/>
<hr/>

## Ethernet through a serial port
Ethernet capable computer:<br/>
`sudo pppd -detach lock proxyarp 192.192.1.1:192.192.1.2 /dev/ttyUSB0 2400`<br/>

Serial computer:<br/>
`sudo pppd -detach lock defaultroute 192.192.1.2:192.192.1.1 /dev/ttyUSB0 2400`<br/>
<hr/>

## Git branch and merge
```bash
git checkout -b <branch_name>
git commit -am "new_stuff"
git push --set-upstream origin <branch_name>
git checkout master
git pull
git merge <branch_name>
git push
git branch -d <branch_name>
```
<hr/>

## Mount telnet port on windows as com port
Install NetBurner Virtual Comm Port<br/>
Use it to mount an address:port as com port<br/>
<hr/>


## Force synchronise timedate ctl
`/lib/systemd/systemd-timesyncd`<br/>
<hr/>

## Create ramfs
Create the ramfs<br/>
`update-initramfs -c -k $(uname -r) -b ./`<br/>

Convert it for use in U-Boot with<br/>
`mkimage -A arm -O linux -T ramdisk -C gzip -n 'Ramdisk Image' -d <ramdisk.img> initrd7.img`<br/>
<hr/>
