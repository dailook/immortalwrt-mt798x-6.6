# 欢迎来到克隆padavanonly 的mt798x源码仓库

# Project ImmortalWrt

ImmortalWrt 是 [OpenWrt](https://openwrt.org ) 的一个分支，移植了更多软件包，支持更多设备，提供默认优化配置以及面向中国大陆用户的本地化修改。
与上游相比，我们允许使用（无法上游化的）修改/技巧，以提供更好的功能/性能/支持。

默认登录地址：http://192.168.6.1 或 http://immortalwrt.lan   用户名：root，密码：无。

## 下载
若要下载开源无线驱动的 ImmortalWrt 官方固件在这里下载。

- [ImmortalWrt 固件选择器](https://firmware-selector.immortalwrt.org/ )

如果您的设备受支持，请点击 **信息** 链接查看安装说明，或查阅下方列出的支持资源。

## 开发
要自行编译固件，您需要 GNU/Linux、BSD 或 macOS 系统（必须区分大小写的文件系统）。Cygwin 不受支持，因为其文件系统不区分大小写。

  ### 编译要求
  推荐使用 Debian 11 或者 Ubuntu20、Ubuntu22 系统进行编译。CPU 需为 AMD64 架构，至少 4 GB 内存，25 GB 可用磁盘空间。Ubuntu24.04系统也可以编译，需要自行搜索研究，需要安装的依赖更少。请确保可正常访问互联网。

  编译 ImmortalWrt 需要以下工具，不同发行版的包名可能不同。

  - 以 Debian/Ubuntu 为例：<br/>
    - 方法一：
      <details>
        <summary>通过 APT 安装依赖</summary>

        ```bash
        sudo apt update -y
        sudo apt full-upgrade -y
        sudo apt install -y ack antlr3 asciidoc autoconf automake autopoint binutils bison build-essential \
          bzip2 ccache clang cmake cpio curl device-tree-compiler ecj fastjar flex gawk gettext gcc-multilib \
          g++-multilib git gnutls-dev gperf haveged help2man intltool lib32gcc-s1 libc6-dev-i386 libelf-dev \
          libglib2.0-dev libgmp3-dev libltdl-dev libmpc-dev libmpfr-dev libncurses-dev libpython3-dev \
          libreadline-dev libssl-dev libtool libyaml-dev libz-dev lld llvm lrzsz mkisofs msmtp nano \
          ninja-build p7zip p7zip-full patch pkgconf python3 python3-pip python3-ply python3-docutils \
          python3-pyelftools qemu-utils re2c rsync scons squashfs-tools subversion swig texinfo uglifyjs \
          upx-ucl unzip vim wget xmlto xxd zlib1g-dev zstd
        ```
      </details>
    - 方法二：
      ```bash
      sudo bash -c 'bash &lt;(curl -s https://build-scripts.immortalwrt.org/init_build_environment.sh )'
      ```

  注意：
  - 全程使用普通用户操作，勿用 root 或 sudo。
  - 其他架构的 CPU 也可编译 ImmortalWrt，但需更多额外操作，请自行研究。
  - 工作路径及文件夹名称中不得包含空格或非 ASCII 字符。
  - 若使用 Windows 子系统 Linux（WSL），需从 PATH 中移除 Windows 目录，详见 [WSL 编译系统设置](https://openwrt.org/docs/guide-developer/build-system/wsl )。
  - __不建议__ 使用 macOS 作为编译主机，恕不保证。可参考 [macOS 编译系统设置](https://openwrt.org/docs/guide-developer/build-system/buildroot.exigence.macosx )。
  - 更多细节请阅读 [编译系统设置](https://openwrt.org/docs/guide-developer/build-system/install-buildsystem ) 文档。

  ### 快速开始编译基于MT798X的路由器固件
  1. 执行 `git clone -b openwrt-24.10-6.6 --single-branch --filter=blob:none https://github.com/dailook/immortalwrt-mt798x-6.6 immortalwrt-mt798x-6.6` 克隆源码。
  2. 执行 `cd immortalwrt-mt798x-6.6` 进入源码目录。
  3. 执行 `./scripts/feeds update -a` 获取 feeds.conf / feeds.conf.default 中定义的全部最新软件包定义。
  4. 执行 `./scripts/feeds install -a` 将所有获取到的软件包链接到 package/feeds/ 中。
  5. 执行cp -f defconfig 目录下对应您设备的配置文件`.config`
     
     ```
     # MT7981
     cp -f defconfig/mt7981-ax3000.config .config

     # MT7986
     cp -f defconfig/mt7986-ax6000.config .config

     # MT7986 ax4200无线
     cp -f defconfig/mt7986-ax4200-bpir3_mini.config .config

     # MT7986 mt7975高功率/netcore n60pro、ruijie rg-x60/x60-new
     cp -f defconfig/mt7975-ipailna-high-power.config .config

     # mt7981无wifi
     cp -f defconfig/mt7981-nowifi.config .config

     # mt7986无wifi 例如：clx_20m
     cp -f defconfig/mt7986-nowifi.config .config

  6. 执行 `make menuconfig` 选择你所需要的机型和插件。
  7. 执行 `make download -j$(nproc)` 下载编译所需的DL库。
  8. 执行 `make V=s -j1` 编译固件。为免报错和硬件配置低而失败，首次编译建议用make V=s -j1命令。
  9. 首次编译成功后，之后的编译可以用快速编译命令 `make V=s -j$(nproc)` 。

  ### 相关仓库
  主仓库通过多个子仓库管理不同类别的软件包。所有软件包均通过 OpenWrt 的包管理器 opkg 安装。若您想开发 Web 界面或为 ImmortalWrt 移植软件包，请查看下方合适的仓库。
  - [LuCI Web 界面](https://github.com/immortalwrt/luci)：通过浏览器控制设备的现代模块化界面。
  - [ImmortalWrt 软件包](https://github.com/immortalwrt/packages)：社区移植的软件包仓库。
  - [OpenWrt 路由](https://github.com/openwrt/routing)：专注（网状）路由的软件包。
  - [OpenWrt 视频](https://github.com/openrt/video)：专注显示服务器及客户端（Xorg 与 Wayland）的软件包。

## 支持信息
支持的设备列表见 [OpenWrt 硬件数据库](https://openwrt.org/supported_devices )
  ### 文档
  - [快速入门指南](https://openwrt.org/docs/guide-quick-start/start )
  - [用户手册](https://openwrt.org/docs/guide-user/start )
  - [开发者文档](https://openwrt.org/docs/guide-developer/start )
  - [技术参考](https://openwrt.org/docs/techref/start )

## 许可证
ImmortalWrt 基于 [GPL-2.0-only](https://spdx.org/licenses/GPL-2.0-only.html ) 发布。
