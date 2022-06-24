# vitis-gpio-led

| License | Versioning |
| ------- | ---------- |
| [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) | [![semantic-release](https://img.shields.io/badge/%20%20%F0%9F%93%A6%F0%9F%9A%80-semantic--release-e10079.svg)](https://github.com/semantic-release/semantic-release) |

GPIO programming with C++.


## Preprequisites

* Host: GNU/Linux only
* Container: Podman `3.4.7` and above. Make sure to configure for rootless.
* Target board: `digilentinc.com:arty-z7-20:part0:1.1`.
* Vivado 2022.1 and above installed via Flatpak.
* [petalinux-builder v1.2.0](https://github.com/extra2000/petalinux-builder).


## Getting Started

Clone this project and `cd` into the project root:
```
git clone https://github.com/extra2000/vitis-gpio-led.git
cd vitis-gpio-led
```


## Creating Vivado Project

From the project root directory, execute the following command:
```
cd vivado/run/
flatpak run com.github.corna.Vivado -mode batch -source ../script/create_prj.tcl -notrace -tclargs --project_name gpio-led
```

Then, load the Vivado project:
```
flatpak run com.github.corna.Vivado -mode gui ./gpio-led/gpio-led.xpr
```

Generate XSA file required by Vitis and PetaLinux.


## Build PetaLinux

Fix permission issues:
```
chcon -R -v -t container_file_t ./petalinux ./vivado/run/gpio-led
podman unshare chown -R 1000:1000 ./petalinux
```

Create project:
```
podman run -it --rm -v ${PWD}/petalinux:${PWD}/petalinux:rw --workdir ${PWD}/petalinux --security-opt label=type:petalinux_builder.process localhost/extra2000/petalinux-builder:latest
petalinux-create --type project --template zynq --name arty-z7-20
chmod -R og+rw ./arty-z7-20/.petalinux/
exit
```

Initialize project:
```
podman run -it --rm -v ${PWD}/vivado:${PWD}/vivado:ro -v ${PWD}/petalinux:${PWD}/petalinux:rw --workdir ${PWD}/petalinux/arty-z7-20 --security-opt label=type:petalinux_builder.process localhost/extra2000/petalinux-builder:latest
petalinux-config --get-hw-description ../../vivado/run/gpio-led/
```

Choose `EXT4` file system:
* `Image Packaging Configuration` > `Root filesystem type` > `EXT4`.

Execute the following command:
```
petalinux-config -c kernel
```

Enable the following options (usually enabled by default):
* `Device Drivers` > `GPIO Support` > `/sys/class/gpio/... (sysfs interface)`
* `Device Drivers` > `GPIO Support` > `Memory mapped GPIO drivers` > `Xilinx Zynq GPIO support`

Execute the following command:
```
petalinux-config -c rootfs
```

Then, set the following configurations:
* Add all packages in `Filesystem Packages` > `libs` > `boost`
* Add all packages in `Filesystem Packages` > `libs` > `libgpiod`
* Add `Filesystem Packages` > `misc` > `gcc-runtime` > `libstdc++`
* Add `Filesystem Packages` > `misc` > `packagegroup-core-tools-profile` > `packagegroup-core-tools-profile`
* Add `Filesystem Packages` > `misc` > `packagegroup-core-tools-profile` > `packagegroup-core-tools-profile-dbg`
* Set `Image Features` > `Init-manager` > `systemd`

Build project:
```
petalinux-build
petalinux-build --sdk
petalinux-package --sysroot
petalinux-package --boot --fsbl ./images/linux/zynq_fsbl.elf --fpga ./images/linux/system.bit --u-boot
```

Exit from the container:
```
exit
```

Restore ownership:
```
podman unshare chown -R 0:0 ./petalinux/
```

Create SD card image.


## Creating Vitis Project

`cd` into ./workspace:
```
cd workspace
```

Generate platform:
```
flatpak run --command=xsct com.github.corna.Vivado ../vitis/platform/arty-z7-20.tcl
```

Launch Vitis:
```
flatpak run --command=vitis com.github.corna.Vivado -workspace .
```

Then, `Import Projects` > `Eclipse workspace or zip file`:
* Select root directory: `/path/to/vitis-gpio-led`.
* Projects:
    * `gpio_led_platform`
    * `gpio_led_system`
    * `led`
    * `logger`
    * `mygpio`
* Options:
    * Do not copy projects into workspace.

Execute `led` binary using the following arguments:
```
--verbose --chip gpiochip0 --line 0 --state on
```
