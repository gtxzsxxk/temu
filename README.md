# TEMU (Tailored Emulator)

本项目的英文全称为`Tailored Emulator`，其中的字母`T`绝对与清华大学没有任何关系。本命名为致敬`QEMU`用。

## Description

该项目是一个基于`C`语言编写的`RISC-V`模拟器，支持`rv32ima_zicsr_zicnt_sstc`架构和`sv32`内存分页结构。模拟器实现了**指令级别**的模拟，即使用`C`执行其描述的内存操作与运算等。本模拟器虚拟了SoC的常见体系结构，支持运行主线`Linux`。我们模拟的SoC，在本文档中，都会基于`OpenSBI`+`U-Boot`的方式来进行内核启动前的工作。

## Getting Started

### Dependencies

* `OpenSBI`, `U-Boot`(Optional), `Linux Kernel`的二进制文件
* `cmake`工具链
* `mkimage` (Optional)
* `riscv-gnu-toolchain`(Optional，如果你希望自己编译`RISC-V`平台的目标二进制文件，我这里使用`buildroot`制作的`riscv32-buildroot-linux-gnu-`作为我的交叉编译工具链)
* 本模拟器目前只支持在Linux下完成编译

### Compiling

#### 编译TEMU模拟器

```
mkdir build && cd build
cmake .. -DCMAKE_C_COMPILER=gcc
make
```

#### 编译`OpenSBI`固件

这里需要用到本项目移植过`llep`实验平台的`OpenSBI`来编译固件。我们使用`fw_jump`型固件来完成从`OpenSBI`到`U-Boot`的跳转。

```
make PLATFORM=llep CROSS_COMPILE=riscv32-buildroot-linux-gnu- -j64
```

编译通过后，我们需要的`fw_jump.bin`会出现在`build/platform/llep/firmware`中。

#### 编译`U-Boot`

这里也需要用到本项目移植过`llep`实验平台的`U-Boot`。

```
make xxxdefconfig # TODO
make CROSS_COMPILE=riscv32-buildroot-linux-gnu- -j64
```

此时，`U-Boot`根目录内的`u-boot.bin`即为`u-boot`本体的二进制文件。`u-boot.dtb`就是我们设备树的二进制文件。我们整个模拟系统都会以这个
`u-boot.dtb`作为我们的设备树。

#### 编译`Kernel`

这里还需要用到本项目移植过`llep`实验平台的`Kernel`。需要注意的是，**我们将根文件系统与内核捆绑在一起**，所以需要保证你有已经制作好的`cpio`格式的文件系统。我们的文件系统将作为`ramfs`挂载在内存上。

```
make xxxdefconfig # TODO
make ARCH=riscv CROSS_COMPILE=riscv32-buildroot-linux-gnu- -j64
```

`arch/riscv/boot/Image`就是我们内核的无压缩的镜像。这个镜像包含了内核从虚拟地址`0xc0000000`开始一直到结束的内存的所有内容。`0xc0000000`是我们
进入内核的入口的虚拟地址，`TEMU`将从起始地址（此时内核没有开启虚拟内存，这个地址通常为`0x80000000`，等页表生效后这个地址就被映射到了`0xc0000000`）开始取第一条内核的指令并继续运行。但是`U-Boot`并不能直接运行这个纯二进制文件，因为`U-Boot`不知道这是什么类型的操作系统，不知道应该怎么传参或者运行它。所以我们需要使用`mkimage`工具将`arch/riscv/boot/Image`打包为`U-Boot`能识别的`uImage`文件，其实就是给`arch/riscv/boot/Image`加了一个`0x40`大小的头。

```
mkimage -A riscv -O linux -T kernel -C none -a 0x80000000 -e 0x80000000 -d arch/riscv/boot/Image uImage
```

由于我们没有实现`rvc`压缩指令，所以我们的内核二进制文件会比较大。我们需要压缩`uImage`到`uImage.gz`。这个`uImage.gz`不像`arm`架构的`zImage`，可以实现自解压，因此我们在进入`U-Boot`后还需要手动解压`uImage.gz`到指定的地址（即`0x80000000`）。

```
gzip -c uImage > uImage.gz
```

### Executing program (Using U-Boot Example)

* 确保你拥有`RISC-V`指令集的二进制文件，本项目目前仅支持装载`bin`，即`objcopy`的输出，暂时不支持直接加载ELF，且加载ELF对于系统级别的模拟意义不大
* 这些二进制文件可以在这里下载：[TEMU Booting Binaries](https://cloud.tsinghua.edu.cn/f/8638fe2edb6d4762a7c1/?dl=1)，可以直接使用`wget`下载到`linux`中。
* 这里给出使用`TEMU`加载`OpenSBI`，`U-Boot`，`Kernel`二进制文件的使用例。

#### Usage (32MiB Version)
```
Usage: temu [-ram/-rom/-addr 0x80000000] [-printreg] -exec=program.bin [-with=addr#file.bin]

Example:
--addr=0x81fa0000
--exec=fw_jump.bin
--with=0x80000000#u-boot.bin
--with=0x81ffd800#u-boot.dtb
--with=0x813a0000#uImage.gz
```

使用以下命令启动`TEMU`：

```
./temu --addr=0x81fa0000 --exec=fw_jump.bin \
--with=0x80000000#u-boot.bin \
--with=0x81ffd800#u-boot.dtb \
--with=0x813a0000#uImage.gz
```

如果成功启动，`TEMU`首先会打印`OpenSBI`的启动信息，接着打印`U-Boot`的信息。

```
OpenSBI v1.4
   ____                    _____ ____ _____
  / __ \                  / ____|  _ \_   _|
 | |  | |_ __   ___ _ __ | (___ | |_) || |
 | |  | | '_ \ / _ \ '_ \ \___ \|  _ < | |
 | |__| | |_) |  __/ | | |____) | |_) || |_
  \____/| .__/ \___|_| |_|_____/|____/_____|
        | |
        |_|

Platform Name             : Low-speed Linux Experimental Platform
Platform Features         : medeleg
Platform HART Count       : 1
Platform IPI Device       : ---
Platform Timer Device     : --- @ 0Hz
Platform Console Device   : uart8250
Platform HSM Device       : ---
Platform PMU Device       : ---
Platform Reboot Device    : ---
Platform Shutdown Device  : ---
Platform Suspend Device   : ---
Platform CPPC Device      : ---
Firmware Base             : 0x81fa0000
Firmware Size             : 178 KB
Firmware RW Offset        : 0x20000
Firmware RW Size          : 50 KB
Firmware Heap Offset      : 0x24000
Firmware Heap Size        : 34 KB (total), 2 KB (reserved), 8 KB (used), 23 KB (free)
Firmware Scratch Size     : 4096 B (total), 160 B (used), 3936 B (free)
Runtime SBI Version       : 2.0

Domain0 Name              : root
Domain0 Boot HART         : 0
Domain0 HARTs             : 0*
Domain0 Region00          : 0x12500000-0x12500fff M: (I,R,W) S/U: (R,W)
Domain0 Region01          : 0x81fc0000-0x81fcffff M: (R,W) S/U: ()
Domain0 Region02          : 0x81fa0000-0x81fbffff M: (R,X) S/U: ()
Domain0 Region03          : 0x00000000-0xffffffff M: () S/U: (R,W,X)
Domain0 Next Address      : 0x80000000
Domain0 Next Arg1         : 0x81ffd800
Domain0 Next Mode         : S-mode
Domain0 SysReset          : yes
Domain0 SysSuspend        : yes

Boot HART ID              : 0
Boot HART Domain          : root
Boot HART Priv Version    : v1.12
Boot HART Base ISA        : rv32ia
Boot HART ISA Extensions  : sstc,zicntr
Boot HART PMP Count       : 0
Boot HART PMP Granularity : 0 bits
Boot HART PMP Address Bits: 0
Boot HART MHPM Info       : 0 (0x00000000)
Boot HART Debug Triggers  : 0 triggers
Boot HART MIDELEG         : 0x00000222
Boot HART MEDELEG         : 0x0000b109


U-Boot 2024.04-rc2-g4e147dba9e-dirty (Mar 07 2024 - 23:15:04 +0800)Low-speed Linux Experimental Platform

DRAM:  31.6 MiB
Core:  11 devices, 8 uclasses, devicetree: separate
Loading Environment from nowhere... OK
In:    uart@12500000
Out:   uart@12500000
Err:   uart@12500000
Net:   No ethernet found.
llep@temu => 
```

在`U-Boot Command-Line Interface`下输入以下命令：

```
unzip 0x813a0000 0x80000000 # 解压uImage.gz到0x80000000
setenv bootargs earlycon=sbi console=ttyS0,115200 root=/dev/ram0 # 设置启动参数
bootm 0x80000000 - 0x81ffd800 # 从0x80000000启动，传递0x81ffd800（设备树地址），作为参数
```

接下来就会进入内核，输出如下：

```
llep@temu => unzip 0x813a0000 0x80000000
Uncompressed size: 6436780 = 0x6237AC
llep@temu => setenv bootargs earlycon=sbi console=ttyS0,115200 root=/dev/ram0
llep@temu => bootm 0x80000000 - 0x81ffd800
## Booting kernel from Legacy Image at 80000000 ...
   Image Name:   
   Created:      2024-03-13   6:26:24 UTC
   Image Type:   RISC-V Linux Kernel Image (uncompressed)
   Data Size:    6436716 Bytes = 6.1 MiB
   Load Address: 80000000
   Entry Point:  80000000
   Verifying Checksum ... OK
## Flattened Device Tree blob at 81ffd800
   Booting using the fdt blob at 0x81ffd800
Working FDT set to 81ffd800
   Loading Kernel Image to 80000000
   Using Device Tree in place at 81ffd800, end 82001de3
Working FDT set to 81ffd800

Starting kernel ...

[    0.000000][    T0] Linux version 6.8.0-rc4-00006-gd90188ef7a29-dirty (root@hyz-wsl) (riscv32-buildroot-linux-gnu-gcc.br_real (Buildroot -ge725bb3-dirty) 12.3.0, GNU ld (GNU Binutils) 2.40) #172 Wed Mar 13 14:26:22 CST 2024
[    0.000000][    T0] SBI specification v2.0 detected
[    0.000000][    T0] SBI implementation ID=0x1 Version=0x10004
[    0.000000][    T0] SBI TIME extension detected
[    0.000000][    T0] SBI IPI extension detected
[    0.000000][    T0] SBI RFENCE extension detected
[    0.000000][    T0] SBI DBCN extension detected
[    0.000000][    T0] earlycon: sbi0 at I/O port 0x0 (options '')
[    0.000000][    T0] printk: legacy bootconsole [sbi0] enabled
[    0.000000][    T0] [paging_init] Test of earlycon printk
[    0.000000][    T0] Zone ranges:
[    0.000000][    T0]   Normal   [mem 0x0000000080000000-0x0000000081f9ffff]
[    0.000000][    T0] Movable zone start for each node
[    0.000000][    T0] Early memory node ranges
[    0.000000][    T0]   node   0: [mem 0x0000000080000000-0x0000000081f9ffff]
[    0.000000][    T0] Initmem setup node 0 [mem 0x0000000080000000-0x0000000081f9ffff]
[    0.000000][    T0] riscv: base ISA extensions aim
[    0.000000][    T0] riscv: ELF capabilities aim
[    0.000000][    T0] pcpu-alloc: s0 r0 d32768 u32768 alloc=1*32768
[    0.000000][    T0] pcpu-alloc: [0] 0 
[    0.000000][    T0] Kernel command line: earlycon=sbi console=ttyS0,115200 root=/dev/ram0
[    0.000000][    T0] Dentry cache hash table entries: 4096 (order: 2, 16384 bytes, linear)
[    0.000000][    T0] Inode-cache hash table entries: 2048 (order: 1, 8192 bytes, linear)
[    0.000000][    T0] Built 1 zonelists, mobility grouping on.  Total pages: 8032
[    0.000000][    T0] mem auto-init: stack:off, heap alloc:off, heap free:off
[    0.000000][    T0] Memory: 25672K/32384K available (2003K kernel code, 180K rwdata, 409K rodata, 3688K init, 69K bss, 6712K reserved, 0K cma-reserved)
[    0.000000][    T0] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=1, Nodes=1
[    0.000000][    T0] NR_IRQS: 64, nr_irqs: 64, preallocated irqs: 0
[    0.000000][    T0] riscv-intc: 32 local interrupts mapped
[    0.000000][    T0] plic: interrupt-controller@c000000: mapped 16 interrupts with 1 handlers for 2 contexts.
[    0.000000][    T0] clocksource: riscv_clocksource: mask: 0xffffffffffffffff max_cycles: 0x171024e7e0, max_idle_ns: 440795205315 ns
[    0.000004][    T0] sched_clock: 64 bits at 100MHz, resolution 10ns, wraps every 4398046511100ns
[    0.001214][    T0] Console: colour dummy device 80x25
[    0.001421][    T0] Calibrating delay loop (skipped), value calculated using timer frequency.. 200.00 BogoMIPS (lpj=400000)
[    0.001636][    T0] pid_max: default: 32768 minimum: 301
[    0.002144][    T0] Mount-cache hash table entries: 1024 (order: 0, 4096 bytes, linear)
[    0.002335][    T0] Mountpoint-cache hash table entries: 1024 (order: 0, 4096 bytes, linear)
[    0.008176][    T1] ASID allocator using 9 bits (512 entries)
[    0.010342][    T1] devtmpfs: initialized
[    0.015524][    T1] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 7645041785100000 ns
[    0.015743][    T1] futex hash table entries: 256 (order: 0, 7168 bytes, linear)
[    0.019971][    T1] platform soc: Fixed dependency cycle(s) with /soc/interrupt-controller@c000000
[    0.026063][    T1] clocksource: Switched to clocksource riscv_clocksource
[    0.082379][    T1] workingset: timestamp_bits=30 max_order=13 bucket_order=0
[    0.084394][    T1] io scheduler mq-deadline registered
[    0.084513][    T1] io scheduler kyber registered
[    0.533974][    T1] Serial: 8250/16550 driver, 4 ports, IRQ sharing disabled
[    0.541813][    T1] serial8250_probe
[    0.542378][    T1] dw8250_probe
[    0.542904][    T1] devm_ioremap membase 2751467520 regs->start 307232768 resource_size(regs) 256
[    0.543209][    T1] uart_16550_compatible, handle_irq = 3223019076
[    0.543336][    T1] dw8250_probe: p->type 1
[    0.545478][    T1] printk: legacy console [ttyS0] disabled
[    0.566918][    T1] 12500000.uart: ttyS0 at MMIO 0x12500000 (irq = 2, base_baud = 72000) is a 16550A
[    0.567187][    T1] printk: legacy console [ttyS0] enabled
[    0.567187][    T1] printk: legacy console [ttyS0] enabled
[    0.567522][    T1] printk: legacy bootconsole [sbi0] disabled
[    0.567522][    T1] printk: legacy bootconsole [sbi0] disabled
[    0.569739][    T1] serial8250_register_8250_port: 0
[    0.575056][    T1] brd: module loaded
[    0.575224][    T1] start plist test
[    0.645973][    T1] end plist test
[    0.684215][    T1] clk: Disabling unused clocks
[    0.685918][    T1] request_irq 0 up->port.irq 2 up->port.irqflags 128 ttyS0 dev_id 3230340672 handle_irq 3223019076
[    0.701479][    T1] Freeing unused kernel image (initmem) memory: 3688K
[    0.701684][    T1] Kernel memory protection not selected by kernel config.
[    0.701903][    T1] Run /init as init process
[    0.702096][    T1]   with arguments:
[    0.702225][    T1]     /init
[    0.702346][    T1]   with environment:
[    0.702480][    T1]     HOME=/
[    0.702602][    T1]     TERM=linux
Saving 256 bits of non-creditable seed for next boot
Starting syslogd: OK
Starting klogd: OK
Running sysctl: OK
Starting network: ip: socket: Function not implemented
ip: socket: Function not implemented
Waiting for interface eth0 to appear............... timeout!
run-parts: /etc/network/if-pre-up.d/wait_iface: exit status 1
FAIL

Welcome to Buildroot
buildroot login: root
login[63]: root login on 'console'
# uname -a
Linux buildroot 6.8.0-rc4-00006-gd90188ef7a29-dirty #172 Wed Mar 13 14:26:22 CST 2024 riscv32 GNU/Linux
# cat /proc/cpuinfo
processor       : 0
hart            : 0
isa             : rv32ima
mmu             : sv32
mvendorid       : 0x0
marchid         : 0x0
mimpid          : 0x0
hart isa        : rv32ima

# 
```

## Help

Any advise for common problems or issues.
```
command to run if program contains helper info
```

## Authors

Contributors names and contact info

ex. Dominique Pizzie  
ex. [@DomPizzie](https://twitter.com/dompizzie)

## Version History

* 0.2
    * Various bug fixes and optimizations
    * See [commit change]() or See [release history]()
* 0.1
    * Initial Release

## License

This project is licensed under the [NAME HERE] License - see the LICENSE.md file for details

## Acknowledgments

Inspiration, code snippets, etc.
* [awesome-readme](https://github.com/matiassingers/awesome-readme)
* [PurpleBooth](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2)
* [dbader](https://github.com/dbader/readme-template)
* [zenorocha](https://gist.github.com/zenorocha/4526327)
* [fvcproductions](https://gist.github.com/fvcproductions/1bfc2d4aecb01a834b46)