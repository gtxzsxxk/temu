# TEMU (Tailored Emulator)

本项目的英文全称为`Tailored Emulator`，其中的字母`T`绝对与清华大学没有任何关系。本命名为致敬`QEMU`用。

- [快餐：进入内核演示](#jumping-to-kernel)
- [存储器与内存映射](#内存映射)
- [B站视频演示](https://www.bilibili.com/video/BV1s2421P7rZ)

## Description

该项目是一个基于`C`语言编写的`RISC-V`模拟器，支持`rv32ima_zicsr_zicnt_sstc`架构和`sv32`内存分页结构。
模拟器实现了**指令级别**的模拟，即使用`C`**解释**执行其描述的内存操作与运算等。这一过程就像`CPython`执行`Python`字节码一样，
`PVM`会根据字节码逐条执行对应的操作，如加载变量、调用函数、执行算术运算等。本模拟器与`CPython`原理基本相同，`TEMU`虚拟了`SoC`
的常见体系结构，
支持运行主线`Linux`。`TEMU`模拟的`SoC`，在本文档中，都会基于`OpenSBI`+`U-Boot`的方式来进行内核启动前的工作。

## Getting Started

### Dependencies

* `OpenSBI`, `U-Boot`(Optional), `Linux Kernel`的二进制文件
* `cmake`工具链
* `mkimage` (Optional)
* `riscv-gnu-toolchain`(Optional，如果你希望自己编译`RISC-V`平台的目标二进制文件，我这里使用`buildroot`
  制作的`riscv32-buildroot-linux-gnu-`作为我的交叉编译工具链)
* 支持的`host`平台：`macOS with LLVM`, `Linux`, `Windows with MinGW`
* (TODO) 将此项目的代码对`MSVC`友好
* (TODO) 适配`Windows`的终端环境
* (TODO) 使用`Ctrl + A + X`退出
* (TODO) 在根文件系统中添加其它软件等
* (TODO) 添加显示屏等外设
* (TODO) 使用`cmake`自动下载`TEMU`二进制
* (TODO) 将定时器也移植到`port`下
* (TODO) 实现休眠，利用`WFI`让出CPU

### Compiling （建议略过，直接跳到下一步，体验已经准备好的二进制）

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

这里还需要用到本项目移植过`llep`实验平台的`Kernel`。需要注意的是，**我们将根文件系统与内核捆绑在一起**
，所以需要保证你有已经制作好的`cpio`格式的文件系统。我们的文件系统将作为`ramfs`挂载在内存上。

```
make xxxdefconfig # TODO
make ARCH=riscv CROSS_COMPILE=riscv32-buildroot-linux-gnu- -j64
```

`arch/riscv/boot/Image`就是我们内核的无压缩的镜像。这个镜像包含了内核从虚拟地址`0xc0000000`
开始一直到结束的内存的所有内容。`0xc0000000`是我们
进入内核的入口的虚拟地址，`TEMU`将从起始地址（此时内核没有开启虚拟内存，这个地址通常为`0x80000000`
，等页表生效后这个地址就被映射到了`0xc0000000`）开始取第一条内核的指令并继续运行。但是`U-Boot`
并不能直接运行这个纯二进制文件，因为`U-Boot`
不知道这是什么类型的操作系统，不知道应该怎么传参或者运行它。所以我们需要使用`mkimage`工具将`arch/riscv/boot/Image`
打包为`U-Boot`能识别的`uImage`文件，其实就是给`arch/riscv/boot/Image`加了一个`0x40`大小的头。

```
mkimage -A riscv -O linux -T kernel -C none -a 0x80000000 -e 0x80000000 -d arch/riscv/boot/Image uImage
```

由于我们没有实现`rvc`压缩指令，所以我们的内核二进制文件会比较大。我们需要压缩`uImage`到`uImage.gz`。这个`uImage.gz`
不像`arm`架构的`zImage`，可以实现自解压，因此我们在进入`U-Boot`后还需要手动解压`uImage.gz`到指定的地址（即`0x80000000`）。

```
gzip -c uImage > uImage.gz
```

### 使用TEMU运行主线Linux系统

* 确保你拥有`RISC-V`指令集的二进制文件，本项目目前仅支持装载`bin`，即`objcopy`的输出，暂时不支持直接加载ELF，且加载ELF对于系统级别的模拟意义不大
* 这些二进制文件可以在这里下载： [TEMU Booting Binaries v0.1.1](https://cloud.tsinghua.edu.cn/f/9e6c7a13b2914654bcd3/?dl=1)，可以直接使用`wget`下载到`linux`中。
    * Changes since `TEMU Booting Binaries v0.1.0`
        * 添加`htop`工具
        * 修复了`U-Boot`中一个引起`Store/AMO Access Fault`异常的bug
    * Changes since `TEMU Booting Binaries v0.0.2`
        * 由于根文件系统的增大（`rootfs`预留量由`6MiB`扩大到了`12MiB`），因此`U-Boot`的启动参数发生了改变
        * 这个版本的`U-Boot`不需要手动输启动参数，等3秒就会`autoboot`。
        * 先前的二进制由于受到大小限制，~~偷偷删去了`libstdc++.so`，这次`libstdc++.so`得到了保留~~
        * 去掉了`Buildroot`自动配置`DHCP`导致启动时间超极长的问题。由于体积原因内核裁掉了网络，而`Buildroot`
          一直在尝试等待`eth0`出现
        * 在根文件系统中添加了以下包：
            * `ascii_invaders`: An ASCII-art game like Space Invaders using `ncurses`.
            * `micropython`: Micro Python is a lean and fast implementation of the Python 3
              programming language that is optimised to run on a microcontroller.
            * `lrzsz`: 用串口传文件。
            * `nano`: 文本编辑工具
* 这里给出使用`TEMU`加载`OpenSBI`，`U-Boot`，`Kernel`二进制文件的使用例。

```
Usage: temu [-ram/-rom/-addr 0x80000000] [-printreg] -exec=program.bin [-with=addr#file.bin]

Example:
--addr=0x81fa0000
--exec=fw_jump.bin
--with=0x80000000#u-boot.bin
--with=0x81ffd800#u-boot.dtb
--with=0x81000000#uImage.gz
```

使用以下命令启动`TEMU`：

```
./temu --addr=0x81fa0000 --exec=fw_jump.bin \
--with=0x80000000#u-boot.bin \
--with=0x81ffd800#u-boot.dtb \
--with=0x81000000#uImage.gz
```

如果成功启动，`TEMU`首先会打印`OpenSBI`的启动信息，接着打印`U-Boot`的信息，然后就进入内核。

```console
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


U-Boot 2024.04-rc2-ge92a78c7e7-dirty (Apr 28 2024 - 21:39:07 +0800) Low-speed Linux Experimental Platform

DRAM:  31.6 MiB
Core:  11 devices, 8 uclasses, devicetree: separate
Loading Environment from nowhere... OK
In:    uart@12500000
Out:   uart@12500000
Err:   uart@12500000
Net:   No ethernet found.
Hit any key to stop autoboot:  0
Uncompressed size: 11712300 = 0xB2B72C
## Booting kernel from Legacy Image at 80000000 ...
   Image Name:
   Created:      2024-04-28  14:03:36 UTC
   Image Type:   RISC-V Linux Kernel Image (uncompressed)
   Data Size:    11712236 Bytes = 11.2 MiB
   Load Address: 80000000
   Entry Point:  80000000
   Verifying Checksum ... OK
## Flattened Device Tree blob at 81ffd800
   Booting using the fdt blob at 0x81ffd800
Working FDT set to 81ffd800
   Loading Kernel Image to 80000000
   Loading Device Tree to 81f32000, end 81f365e3 ... OK
Working FDT set to 81f32000

Starting kernel ...

[    0.000000][    T0] Linux version 6.8.0-rc4-00008-g6f4f7080ab52-dirty (root@hyz-wsl) (riscv32-buildroot-linux-gnu-gcc.br_real (Buildroot -ge725bb3-dirty) 12.3.0, GNU ld (GNU Binutils) 2.40) #178 Sun Apr 28 22:03:29 CST 2024
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
[    0.000000][    T0] Memory: 20516K/32384K available (2002K kernel code, 177K rwdata, 408K rodata, 8845K init, 68K bss, 11868K reserved, 0K cma-reserved)
[    0.000000][    T0] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=1, Nodes=1
[    0.000000][    T0] NR_IRQS: 64, nr_irqs: 64, preallocated irqs: 0
[    0.000000][    T0] riscv-intc: 32 local interrupts mapped
[    0.000000][    T0] plic: interrupt-controller@c000000: mapped 16 interrupts with 1 handlers for 2 contexts.
[    0.000000][    T0] clocksource: riscv_clocksource: mask: 0xffffffffffffffff max_cycles: 0x171024e7e0, max_idle_ns: 440795205315 ns
[    0.000000][    T0] sched_clock: 64 bits at 100MHz, resolution 10ns, wraps every 4398046511100ns
[    0.006451][    T0] Console: colour dummy device 80x25
[    0.007573][    T0] Calibrating delay loop (skipped), value calculated using timer frequency.. 200.00 BogoMIPS (lpj=400000)
[    0.008577][    T0] pid_max: default: 32768 minimum: 301
[    0.012323][    T0] Mount-cache hash table entries: 1024 (order: 0, 4096 bytes, linear)
[    0.013313][    T0] Mountpoint-cache hash table entries: 1024 (order: 0, 4096 bytes, linear)
[    0.048807][    T1] ASID allocator using 9 bits (512 entries)
[    0.061496][    T1] devtmpfs: initialized
[    0.094684][    T1] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 7645041785100000 ns
[    0.095702][    T1] futex hash table entries: 256 (order: 0, 7168 bytes, linear)
[    0.121384][    T1] platform soc: Fixed dependency cycle(s) with /soc/interrupt-controller@c000000
[    0.156950][    T1] clocksource: Switched to clocksource riscv_clocksource
[    0.478313][    T1] workingset: timestamp_bits=30 max_order=13 bucket_order=0
[    0.498619][    T1] io scheduler mq-deadline registered
[    0.499108][    T1] io scheduler kyber registered
[    3.868154][    T1] Serial: 8250/16550 driver, 4 ports, IRQ sharing disabled
[    3.992753][    T1] printk: legacy console [ttyS0] disabled
[    4.024866][    T1] 12500000.uart: ttyS0 at MMIO 0x12500000 (irq = 2, base_baud = 72000) is a 16550A
[    4.026612][    T1] printk: legacy console [ttyS0] enabled
[    4.026612][    T1] printk: legacy console [ttyS0] enabled
[    4.036093][    T1] printk: legacy bootconsole [sbi0] disabled
[    4.036093][    T1] printk: legacy bootconsole [sbi0] disabled
[    4.134996][    T1] brd: module loaded
[    4.141977][    T1] start plist test
[    4.438989][    T1] end plist test
[    4.957636][    T1] clk: Disabling unused clocks
[    5.231312][    T1] Freeing unused kernel image (initmem) memory: 8844K
[    5.234550][    T1] Kernel memory protection not selected by kernel config.
[    5.237485][    T1] Run /init as init process
[    5.239592][    T1]   with arguments:
[    5.240876][    T1]     /init
[    5.242666][    T1]   with environment:
[    5.244228][    T1]     HOME=/
[    5.246093][    T1]     TERM=linux
Saving 256 bits of non-creditable seed for next boot
Starting syslogd: OK
Starting klogd: OK
Running sysctl: OK
Starting network: ip: socket: Function not implemented
ip: socket: Function not implemented
FAIL

Welcome to Buildroot
buildroot login:
```

进入内核后，可以尝试以下玩法：

```console
Welcome to Buildroot
buildroot login: root
login[60]: root login on 'console'
# uname -a
Linux buildroot 6.8.0-rc4-00008-g6f4f7080ab52-dirty #178 Sun Apr 28 22:03:29 CST 2024 riscv32 GNU/Linux
# cat /proc/cpuinfo
processor       : 0
hart            : 0
isa             : rv32ima
mmu             : sv32
mvendorid       : 0x0
marchid         : 0x0
mimpid          : 0x0
hart isa        : rv32ima

# cat /proc/interrupts
           CPU0
  2:        161  SiFive PLIC  10 Level     ttyS0
  5:      10132  RISC-V INTC   5 Edge      riscv-timer
# micro
microcom     micropython
# micropython
MicroPython v1.22.0 on 2024-04-28; linux [GCC 12.3.0] version
Use Ctrl-D to exit, Ctrl-E for paste mode
>>> 0.1 + 0.2
0.3
>>> print("Because we don't use the real IEEE754 float points")
Because we don't use the real IEEE754 float points
>>> a = "Hello"
>>> a[-1]
'o'
>>>
# ascii_invaders



                          _ _   _                     _
            __ _ ___  ___(_|_) (_)_ ____   ____ _  __| | ___ _ __ ___
           / _` / __|/ __| | | | | '_ \ \ / / _` |/ _` |/ _ \ '__/ __|
          | (_| \__ \ (__| | | | | | | \ V / (_| | (_| |  __/ |  \__ \
           \__,_|___/\___|_|_| |_|_| |_|\_/ \__,_|\__,_|\___|_|  |___/


                                _/MM\_  = ?  points
                                qWAAWp

                                 {@@}   = 30 points
                                 /""\

                                 dOOb   = 20 points
                                 ^/\^

                                 /MM\   = 10 points
                                 |~~|



                   https://github.com/macdice/ascii-invaders

```

## Documentation (TODO)

### CPU Architect

我们没有对微架构的描述，因为我们是解释执行指令，所以简化了许多微架构设计的内容。本项目模拟的SoC实现了`TLB`
与`I-Cache`和`D-Cache`，都是以组相连的方式实现，由于CPU的串行性。
也不存在任何总线，我们对SoC的仿真类似于操作系统中的宏内核的概念，指令解释与外设模拟都是同时实现的，可以直接调用。

以下是CPU解释执行指令的有关代码：

- `src/decode.c`：解释并执行指令
- `src/machine.c`：取指，模拟外设，中断等特权态处理
- `src/mem.c`：模拟内存控制器对内存进行读写
- `src/vm.c`：实现对虚拟地址的转换，以及对物理内存的读写
- `src/zicsr.c`：实现特权架构

#### 支持的ISA

**rv32ima_zicsr_zicnt_sstc**

- `i`：整数指令
- `m`：硬件乘法
- `a`：原子指令
- `zicsr`：特权架构支持
- `zicnt`：CSR寄存器的计数器支持
- `sstc`：特权架构`Supervisor`级别定时器中断支持。这个扩展给运行在`Supervisor`模式的软件
  （后续都称为操作系统）也添加了`time`和`timecmp`寄存器，允许操作系统读写这两个寄存器来实现直接在`Supervisor`模式进行处理的
  定时器中断。（如果你学习过`xv6`，你会发现`xv6`只会在`Machine`特权态下处理定时器的中断，并且它还在手册里明确指出了`risc-v`
  规定
  定时器中断只能在`Machine Mode`下处理。但是如果实现了`sstc`拓展，这个过程可以得到很大的简化。）

### 内存映射

| 类型   | Base Address | Size     |
|------|--------------|----------|
| ROM  | 0x0000_0000  | 64KiB    |
| RAM  | 0x8000_0000  | 32MiB    |
| UART | 0x1250_0000  | 0x100    |
| PLIC | 0x0c00_0000  | 0x400000 |

对于`OpenSBI`+`U-Boot`+`Linux`启动方案，RAM在上电时应该由`TEMU`模拟`first stage bootloader`，将二进制文件加载到以下位置。

| 地址          | 可用空间   | 描述                                             |
|-------------|--------|------------------------------------------------|
| 0x8000_0000 | (x)    | 上电时这里为u-boot.bin。u-boot重定位后，内核将被解压到此处，并从此处开始运行 |
| 0x8100_0000 | 10MiB  | 压缩过的内核，即uImage.gz，存储于此地址                       |
| 0x81da_0000 | 2MiB   | U-Boot将在上电后从0x8000_0000重定位至此处                  |
| 0x81fa_0000 | 384KiB | OpenSBI的固定地址。这一段逻辑上是只读的                        |
| 0x81ff_d800 | 10KiB  | 设备树的二进制文件                                      |

在`32MiB`内存方案下，内核可用的内存大小约为**30.625MiB**

进入系统后执行`free -h`得到的结果是：

```
# free -h
              total        used        free      shared  buff/cache   available
Mem:          28.7M        4.4M       13.4M        8.0K       10.9M       12.9M
Swap:             0           0           0
```

进入系统后执行`df -h`得到的结果是：

```
# df -h
Filesystem                Size      Used Available Use% Mounted on
devtmpfs                 10.0M         0     10.0M   0% /dev
tmpfs                    14.3M         0     14.3M   0% /dev/shm
tmpfs                    14.3M         0     14.3M   0% /tmp
tmpfs                    14.3M      8.0K     14.3M   0% /run
```

### MMU模型

实现`sv32`内存模型。具体的`risc-v`特权态手册里都写了。这个页表就两级，实现起来比较容易。

### CSR寄存器支持

实现的CSR请参考`include/zicsr.h`，但是真正使用过的CSR是以下几个：

- `sstatus`
- `sie`
- `stvec`
- `sepc`
- `scause`
- `stval`
- `sip`
- `stimecmp`
- `stimecmph`
- `satp`
- `mstatus`
- `medeleg`
- `mideleg`
- `mie`
- `mtvec`
- `mepc`
- `mcause`
- `mcause`
- `mtval`
- `mip`
- `scontext`
- `cycle`
- `time`
- `cycleh`
- `timeh`
- `mvendorid`

需要注意的是，以上列出的寄存器，只是我们代码中显式调用了的。对于某些软件，你必须实现我们在`include/zicsr.h`的这些寄存器。它必须存在，尽管
我并没有实现它们具体的功能。

### 异常与中断

(WIP)

### uart 16550a

(WIP)

### PLIC中断控制器

(WIP)

我们实现的是`t-head`的`plic`，而不是`sifive`的`plic`控制器。

## Authors

Obvious.

## Version History

This is blamed to git history.

## Acknowledgments

Inspiration, code snippets, etc.

* [awesome-readme](https://github.com/matiassingers/awesome-readme)
* [PurpleBooth](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2)
* [dbader](https://github.com/dbader/readme-template)
* [zenorocha](https://gist.github.com/zenorocha/4526327)
* [fvcproductions](https://gist.github.com/fvcproductions/1bfc2d4aecb01a834b46)