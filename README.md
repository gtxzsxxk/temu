[Temu Running Binaries](https://cloud.tsinghua.edu.cn/f/8638fe2edb6d4762a7c1/?dl=1)

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

这里还需要用到本项目移植过`llep`实验平台的`Kernel`。

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
* 这里给出使用`TEMU`加载`OpenSBI`，`U-Boot`，`Kernel`二进制文件的使用例。

#### Usage
```
Usage: temu [-ram/-rom/-addr 0x02000000] [-printreg] -exec=program.bin [-with=addr#file.bin]

Example:
--addr=0x80db5000
--exec=fw_jump.bin
--with=0x80000000#u-boot.bin
--with=0x80dfd800#u-boot.dtb
--with=0x80ab5000#uImage.gz
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