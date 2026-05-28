# Linux Net Watchdog

Linux Net Watchdog is a small network status monitor for Linux devices. It is
designed for embedded Linux, edge devices, gateways, and lab boards that need a
simple way to check whether key network services are reachable.

The project is written in C and uses Linux/POSIX system calls. It focuses on
network connectivity checks, TCP port probing, log recording, and simple
configuration loading.

## Features

- Periodically checks TCP connectivity for configured targets.
- Records success, failure, latency, and error reason in a local log file.
- Loads monitor targets from a plain text configuration file.
- Handles `SIGINT` and `SIGTERM` for clean shutdown.
- Uses a small modular C structure suitable for embedded Linux learning.
- Builds with `make` and has no third-party runtime dependencies.

## Directory Structure

```text
linux-net-watchdog/
├── Makefile
├── README.md
├── config/
│   └── watchdog.conf
├── include/
│   ├── config.h
│   ├── logger.h
│   ├── monitor.h
│   └── util.h
└── src/
    ├── config.c
    ├── logger.c
    ├── main.c
    ├── monitor.c
    └── util.c
```

## Build

```bash
make
```

The output binary is generated at:

```bash
build/net-watchdog
```

## Run

```bash
./build/net-watchdog -c config/watchdog.conf
```

Optional arguments:

```bash
./build/net-watchdog -c config/watchdog.conf -i 5 -o logs/watchdog.log
```

- `-c`: configuration file path
- `-i`: check interval in seconds
- `-o`: log file path

## Configuration

Each monitor target uses this format:

```text
name host port timeout_ms
```

Example:

```text
gateway 192.168.1.1 80 800
dns 223.5.5.5 53 1000
web example.com 443 1500
```

Lines starting with `#` are ignored.

## Example Log

```text
2026-05-28 14:05:11 [INFO] gateway 192.168.1.1:80 OK latency=4ms
2026-05-28 14:05:11 [WARN] dns 223.5.5.5:53 FAIL timeout after 1000ms
```

## Resume Description

**Linux 网络设备状态监控工具**  
技术栈：C、Linux、Socket、Makefile、日志模块

设计并实现一个运行于 Linux 环境的网络状态监控工具，模拟嵌入式设备网络诊断场景。项目支持通过配置文件管理检测目标，周期性检测目标 IP/域名的 TCP 端口可达性，记录检测结果、延迟和错误原因，并支持进程信号退出与日志持久化。

主要工作：

- 使用 C 语言与 POSIX Socket 实现 TCP 端口探测、超时控制和错误状态识别。
- 设计配置解析模块，支持按 `名称 主机 端口 超时时间` 加载多个检测目标。
- 实现日志模块，记录检测时间、目标地址、连接状态、延迟和失败原因，便于问题追踪。
- 使用 Makefile 管理编译流程，按配置、日志、监控、工具函数拆分项目结构。
- 在 Linux/WSL 环境下验证端口关闭、目标不可达、超时等异常场景。

## Notes

This is a learning project for Linux network programming and embedded software
internship preparation. It does not replace professional monitoring systems.
