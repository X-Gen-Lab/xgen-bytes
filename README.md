# xgen-bytes

独立的 C11 整数与字节转换组件。提供 16、32、64 位无符号整数的大小端读写，支持任意字节对齐，不分配内存，也不依赖 core、status、RTOS 或其他 X-Gen 组件。当前源码版本为 0.1.0，ABI 为 1；这是本地实现，尚未配置远端或公开发布。

| 项目 | 接口 |
| --- | --- |
| CMake 包 | `xgen_bytes` |
| 公开 target / component | `xgb::bytes` / `bytes` |
| 公开头 | `xgen/bytes/bytes.h` |
| 版本头 | `xgen/bytes/version.h` |
| 写入 | `xgb_serialize_u16_le` 等，宽度为 16/32/64，后缀为 le/be |
| 读取 | `xgb_deserialize_u16_le` 等，宽度为 16/32/64，后缀为 le/be |

## 行为契约

- 调用者提供并拥有缓冲区，非空时至少有对应的 2、4、8 个可读或可写字节；接口不检查外层消息长度。
- 空指针写入不执行操作，空指针读取返回零，保持来源行为。
- 写入恰好修改对应宽度；读取不修改输入。不会保留指针，不要求初始化。
- 调用同步完成，工作量固定，没有锁或阻塞等待。多字节读写不保证原子性；共享缓冲区的冲突访问由调用者同步。
- 不定义线协议，不依据本机结构体布局或本机字节序。真实 MCU 的时序与完整 Boot 资源预算另行验证。

```c
#include <xgen/bytes/bytes.h>

uint8_t bytes[4];
xgb_serialize_u32_be(bytes, UINT32_C(0x01020304));
uint32_t value = xgb_deserialize_u32_be(bytes);
```

## 构建与消费

需要 CMake 3.24 及以上、C11 编译器；预设使用 Ninja。默认关闭测试，纯 C 构建不需要 C++ 编译器或 GoogleTest。

```sh
cmake --preset release
cmake --build --preset release
cmake --install out/release --prefix out/install
```

安装消费者通过 `CMAKE_PREFIX_PATH` 指向实际安装前缀：

```cmake
find_package(xgen_bytes 0.1 CONFIG REQUIRED COMPONENTS bytes)
target_link_libraries(my_app PRIVATE xgb::bytes)
```

不指定 component 时导入 `bytes`。未知的必需 component 或不兼容版本会失败；未知的可选 component 标记为不可用。源码消费者可显式 `add_subdirectory` 后链接同一公开 target，不要求固定工作区布局。

若父工程已提供 `xgb::bytes`，源码和安装入口都会核对其 `XGB_VERSION` 与 `XGB_ABI_VERSION`，必须分别与当前包的 `0.1.0`、`1` 一致。兼容 target 被复用；不兼容或身份属性缺失时失败，不重复构建或静默替换提供者。

## 测试与维护

按 [贡献指南](CONTRIBUTING.md) 显式准备 GoogleTest 1.16.0 和质量工具后运行：

```sh
cmake --preset host
cmake --build --preset host
ctest --preset host
```

Host 使用 C++17 / GoogleTest 测试真实 C11 target。测试覆盖大小端向量、非对齐、空指针、边界值、固定随机序列，以及独立源码/安装消费和包兼容性。

来源与授权见 [PROVENANCE.md](PROVENANCE.md)，规范采用见 [docs/standards.md](docs/standards.md)，实际证据见 [实施记录](docs/implementation-log.md)。本地验证、远端 CI、公开发布和硬件验证分别记录。
