---
layout: page
title: Linux Cheatsheet
---

{simpleLineBreaks: true}

Here is a Cheatsheet of various linux tips and tricks I accumulated through the time.

It is mostly a tool for myself, but figures it might benefits other people.

# Tips and tricks

## Echo byte in file
To write hex
`echo -n -e '\xbc\x00' > failures`
To display hex
`xxd failures`
<hr/>

## Search for pattern inside a file
`grep -rnw '/path/to/search' -e 'patern'`
<hr/>

## Remount as rw
`mount -o remount rw /`
<hr/>

## Stream ethernet data
From
`pv --rate-limit 100M </dev/urandom | nc 192.168.1.42 7777`
To
`nc -l 7777 > nc.txt`
<hr/>

## Create a new serial console
add `ttyUSB0::respawn:/sbin/getty -L  ttyUSB0 115200 vt100` in /etc/inittab

Another way is to bind a console on the fly with : `setsid /sbin/getty -L  ttyUSB2 115200 vt100`
<hr/>

## View disk usage (RW)
`cat /proc/diskstats`
Ex. output:
179       0 mmcblk0 255 326 19542 767 25 21 92 111 0 640 710 0 0 0 0
field 1 (255) = reads
field 7 (92) = writes

To monitor what is written
`echo 1 > /proc/sys/vm/block_dump`

To see and example of output
`echo hello > world.txt`
`dmesg`

To see all, reboot and
`dmesg | grep dirtied | grep "on vda" | sort`
<hr/>

## Flash ftdi eeprom
`sudo apt install ftdi-eeprom`
to read eeprom, create a file such as ftdi_eeprom.conf with
``
##################################################
filename="eeprom.copy"  # Filename to save eemrpon config
eeprom_type=3           # number three is for type R chips
flash_raw=true          # raw copy of unmodified image
##################################################
``
Find the vendor and product code with `lsusb`
Then run `sudo ftdi_eeprom --device i:0x0403:0x6011 --read-eeprom ftdi_eeprom.conf`
<hr/>

## Edit a flash rootfs
This [guide](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18842473/Build+and+Modify+a+Rootfs) covers how to extract, mount compress and use mkimage to edit the rootfs
<hr/>

## list difference between two folder
`diff -qr --no-dereference /path/to/folder1 /path/to/folder2`
<hr/>

## Show what device a folder is on
`df /path/to/folder`
Or to get what device
`findmnt /path`
<hr/>

## Mount a folder in RAM
`mount -t tmpfs -o size=100M tmpfs /path/to/folder`
<hr/>

## Create a bind point of a path
This allow for acces to old bind point of a folder (Ex.: if tmpfs is mounted over a path)
`mount -o bind /source /destination`
<hr/>

## Analyse systemd boot sequence
`systemd-analyze plot > output.svg`
<hr/>

## Update time from web
`timedatectl set-ntp on`
`timedatectl set-ntp off`
<hr/>

## rsyslog severity
Severity
Num.Code 	Severity 	Description
0 			emerg 		system is unusable
1 			alert 		action must be taken immediately
2 			crit 		critical conditions
3 			error 		error conditions
4 			warning 	warning conditions
5 			notice 		normal but significant condition
6 			info 		informational messages
7 			debug 		debug-level messages
<hr/>

## link down and up
sudo ip link set dev eth1 down
sudo ip link set dev eth1 up
<hr/>

## Give a fixed interface name
create a rule such as
`/etc/systemd/network/10-my_eth.link`
with the config such as
```conf
[Match]
MACAddress=00:a0:de:63:7a:e6

[Link]
Name=my_eth
```

To fix parameters, then edit /etc/dhcpcd.conf such as
```conf
interface my_eth
static ip_address=192.168.0.10/24
#static ip6_address=fd51:42f8:caae:d92e::ff/64
static routers=192.168.0.1
#static domain_name_servers=192.168.0.1 8.8.8.8 fd51:42f8:caae:d92e::1

```
To set a priority, add in dhcpcd.conf (lower in first, 200 default, +100 if wifi)
```
interface my_eth
metric 400
```
<hr/>

## get usb device info
`/bin/udevadm info --name=/dev/ttyUSB0 | grep "ID_USB_INTERFACE_NUM=00" `
`/bin/udevadm info --name=/dev/ttyUSB0 | grep "ID_SERIAL_SHORT=FT2NP4Z6"`
`/bin/udevadm info --name=/dev/ttyUSB0 | grep "DEVNAME"`
<hr/>

## How to prepare rootfs for read-only
https://wiki.debian.org/ReadonlyRoot
<hr/>

## Extract device tree
Run the raspberry pi without uboot so it can pass the complete devicetree
and extract it with:
`sudo dtc -I fs -O dtb /sys/firmware/devicetree/base -o device_tree.dtb`

If the source is needed:
`sudo dtc -I fs -O dts /sys/firmware/devicetree/base -o device_tree.dts`
<hr/>


## Install node version with package manager
Dowload  the package
`tar -xvf node-v12.16.1-linux-x64.tar.xz -C ~/nodejs --strip-components 1`
add to bashrc: (REPLACE <yourhome> with your actual username)
``` bash
if ! [[ $PATH =~ '/home/<yourhome>/nodejs/bin' ]]; then
    export PATH="$PATH:/home/<yourhome>/nodejs/bin"
fi
```

or just
`PATH="{PATH:+${PATH}:}/opt/nodejs/bin"`
<hr/>

## Bridge two network interface
How to enable port 80 from one interface to the other
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
`sudo apt install pptp-linux`
`sudo systemctl enable --now pppd-dns.service`

Create /etc/ppp/peers/my_vpn and add:
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
Execute with `sudo pon my_vpn`

Debug with `sudo pon my_vpn debug dump logfd 2 nodetach`
Does not work if ufw is enabled!

Route a remote address as local with:
`sudo ip route add <IP_DESIRED>/32 dev ppp0`
<hr/>

## Mounting a socket as a serial port + bash terminal access through it
Mount locally the terminal with
`socat  pty,link=/tmp/virt,raw  tcp:<host>:<port>`
Then attach a screen session to it
`screen -A /tmp/virt`

If you want to listen to a local socket instead if a remote socket
`socat -d -d pty,link=/dev/virt,raw,echo=0 tcp-listen:<port>`

`-d -d` options add debug.
<hr/>

## Ethernet through a serial port
Ethernet capable computer:
`sudo pppd -detach lock proxyarp 192.192.1.1:192.192.1.2 /dev/ttyUSB0 2400`

Serial computer:
`sudo pppd -detach lock defaultroute 192.192.1.2:192.192.1.1 /dev/ttyUSB0 2400`
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
Install NetBurner Virtual Comm Port
Use it to mount an address:port as com port
<hr/>


## Force synchronise timedate ctl
`/lib/systemd/systemd-timesyncd`
<hr/>

## Create ramfs
Create the ramfs
`update-initramfs -c -k $(uname -r) -b ./`

Convert it for use in U-Boot with
`mkimage -A arm -O linux -T ramdisk -C gzip -n 'Ramdisk Image' -d <ramdisk.img> initrd7.img`
<hr/>
