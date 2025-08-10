This repository is forked for learning the implementation of Tokio and C++20 coroutines.

The code may be adjusted based on my understanding and the compiler toolchain I am using.

# Zedio

[![C++23](https://img.shields.io/static/v1?label=standard&message=C%2B%2B23&color=blue&logo=c%2B%2B&&logoColor=white&style=flat)](https://en.cppreference.com/w/cpp/compiler_support)
![Platform](https://img.shields.io/static/v1?label=platform&message=linux&color=dimgray&style=flat)

```
  ______  ______   _____    _____    ____  
 |___  / |  ____| |  __ \  |_   _|  / __ \ 
    / /  | |__    | |  | |   | |   | |  | |
   / /   |  __|   | |  | |   | |   | |  | |
  / /__  | |____  | |__| |  _| |_  | |__| |
 /_____| |______| |_____/  |_____|  \____/ 
                                                                       
```

Documentation: https://8sileus.github.io/zedio/

Zedio is an event-driven header library for writing asynchronous applications in modern C++:

## Feature:
+ Multithreaded, work-stealing based task scheduler. (reference [tokio](https://tokio.rs/))
+ Proactor event handling backed by [io_uring](https://github.com/axboe/liburing).
+ Zero overhead abstraction, no virtual, no dynamic

## Sub library: 
+ **I/O**
+ **NetWorking** 
+ **FileSystem** 
+ **Time** 
+ **Sync** 
+ **Log** 

It's being developed, if you're interested in zedio and want to participate in its development, see [contributing](./docs/zedio/contributing.md)

# Example
```C++
// An echo server
// Ignore all errors
#include "zedio/core.hpp"
#include "zedio/net.hpp"

using namespace zedio;
using namespace zedio::async;
using namespace zedio::net;

auto process(TcpStream stream) -> Task<void> {
    char buf[1024]{};
    while (true) {
        auto len = (co_await (stream.read(buf))).value();
        if (len == 0) {
            break;
        }
        co_await stream.write_all({buf, len});
    }
}

auto server() -> Task<void> {
    auto addr = SocketAddr::parse("localhost", 9999).value();
    auto listener = TcpListener::bind(addr).value();
    while (true) {
        auto [stream, addr] = (co_await listener.accept()).value();
        spawn(process(std::move(stream)));
    }
}

auto main() -> int {
    // zedio::runtime::CurrentThreadBuilder::default_create().block_on(server());
    zedio::runtime::MultiThreadBuilder::default_create().block_on(server());
}
```

## Prepare the compilation toolchain

### Prerequisites
The version of the operating system I am using is Ubuntu 24.04 (WSL).  
The version of the gcc compiler toolchain included is: 13.3.0.  
> For instructions on how to upgrade an existing WSL to Ubuntu 24.04, you can refer to (the backup step can be skipped): https://phoenixnap.com/kb/wsl-upgrade-ubuntu.  
> If you encounter `Please install all available updates for your release before upgrading.`, you can refer to: https://zhuanlan.zhihu.com/p/687756716.

Also, please refer to the [vcpkg](https://github.com/microsoft/vcpkg) documentation to download and successfully configure vcpkg.

vcpkg is a package manager that can be conveniently integrated with CMake. By adding the following parameters when executing the CMake command, the download and installation of third-party packages can be automatically completed:
```
cmake -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
```

### Install the Clang compiler
For better code quality, it is recommended to use the LLVM compilation toolchain (clang/clang++).  
Additionally, this ensures that error hints in your code (clangd) are consistent with the actual compilation results.

You can use the following command to install the latest version of the LLVM compilation toolchain in one step. For reference, see [clickhouse docs](https://clickhouse.com/docs/development/build#install-the-clang-compiler).
```curl
sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
```
You can also manually download the pre-built package from [github release](https://github.com/llvm/llvm-project/releases/), and after extracting it, manually specify the path in [CLion Toolchain](https://www.jetbrains.com/help/clion/how-to-create-toolchain-in-clion.html).

### Install cmake & ninja
Please download the pre-built packages manually:
[cmake](https://github.com/Kitware/CMake/releases)

[ninja](https://github.com/ninja-build/ninja/releases)

After extracting, fill in the binary path in the CLion Toolchain.

At this point, all environment configurations are complete. Use the CLion IDE to run with one click!