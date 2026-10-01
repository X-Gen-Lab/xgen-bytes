# S1 独立 bytes 实施记录

记录日期：2026-10-01。实现分支：`feat/independent-bytes`。源码版本 `0.1.0`、ABI `1`；本记录描述本地实现，不代表远端发布或产品已经迁移。

## 边界与来源基线

提供 `xgen_bytes` 包、`xgb::bytes` target 和 12 个固定宽度无符号整数读写接口。只迁移 core 的 bytes 能力，不引入内存、容器、错误域、协议或平台依赖。

迁移来源为 xgen-core `dc5eb1167ba21de384e7a760a8586b87502b73b2`。协调任务在迁移前运行原 `xgc_bytes`、`xgc_status`，**2/2 通过**；bytes 对应原 `tests/test_core.c` 的大小端向量、非对齐、NULL 与固定种子 4096 次往返。

本组件将 `src/bytes.c` 的公开符号从 `xgc_` 改为 `xgb_`、包含路径改为 `xgen/bytes/bytes.h`，再应用 X-Gen 排版。归一化这两项命名变化并去除空白后，与来源源码完全相同；没有修改算法和 NULL 行为。纯迁移使用来源基线，不人为破坏算法制造 RED。

## 新包契约的 RED / GREEN

新增需求：已有 `xgb::bytes` target 不能绕过包版本/ABI 校验，源码入口与安装入口均拒绝错误版本、错误 ABI 和缺少身份属性的提供者，同时复用兼容提供者。

先创建 6 个拒绝用例，在尚无身份校验的最小包实现上执行：

```sh
ctest --test-dir out/host -C Debug -R '^xgb_(installed|source)_(bad_version|bad_abi|no_identity)$' --no-tests=error --output-on-failure
```

RED 实际结果为 **6/6 失败，退出码 8**，原因均为 `Invalid package request was accepted`。生产和单元测试工程已经成功编译；消费者配置错误接受目标，触发测试断言。日志保存在 `out/evidence/package-red.log` 与 `package-red.xml`，初始根 CMake、包模板和源文件摘要保存在同目录的 `*.red.txt`、`package-red-sources.txt`。

随后实现共享的 `cmake/xgen_bytesCheckTarget.cmake`，由 `add_subdirectory` 路径和安装 package config 同时调用。使用独立的 `XGB_VERSION`、`XGB_ABI_VERSION` 属性；身份不匹配或缺失时明确拒绝。相同 6 个拒绝用例转为通过，兼容提供者的真实 C/C++ 链接消费也通过。

初始单元测试辅助变量曾触发 GoogleTest 宏内 `-Wshadow`，格式化 CMake 模板也曾破坏占位符；两项均修正并保留过程日志。它们是搭建问题，不计作上述有效 RED。

## 实际本地验证

Windows 环境，CMake 4.2、Ninja、GCC/G++ 13.2.0；MSVC 使用 VS2022 `19.40.33811.0`。GoogleTest 均为固定提交 `ff6133ab49b364a883a55ba75c39e520fea6245b` 的 1.16.0，分别使用匹配工具链的外部安装包；MSVC 依赖和测试统一 `/MDd`。

| 验证 | 实际结果 | 本地证据 |
| --- | --- | --- |
| GCC Debug Host | 44/44，20.51 秒 | out/evidence/host-green.log、host-green.xml |
| GCC coverage 完整回归 | 44/44，21.19 秒 | out/evidence/coverage-tests.log、coverage-tests.xml |
| MSVC Debug Host | 44/44，26.00 秒 | out/evidence/msvc-tests.log、msvc-tests.xml |
| 纯 C Release | 配置、构建通过，无 C++ 编译器要求 | out/evidence/release-configure.log、release-build.log |
| 头自包含 | bytes/version 各自 C11 与 C++17 编译通过 | Host 构建日志和 xgb_header_checks |
| 参数化发现 | 30 个稳定命名 unit、14 个 integration | out/evidence/host-discovery.json |
| 生产依赖与符号 | 12 个公开符号，无未解析外部符号 | out/evidence/defined-symbols.txt、undefined-symbols.txt |
| 来源行为与插桩范围 | 命名归一化后源码一致；只有 bytes.c 插桩 | out/evidence/production-contracts.txt |
| X-Gen 格式 | clang-format 19.1.5 对自有 C/C++ dry-run 通过 | 与受控 .clang-format 配套运行 |

参数化测试分别覆盖六种宽度/字节序组合的独立写入向量、独立读取向量、偏移 0–7、周围字节保护、输入不变、全部 NULL 分支、零/最大值/逐位值和原固定随机语料。

14 项消费测试包含源码、显式/默认 component 安装消费、未知可选/必需 component、错误包版本，以及两个入口各自的兼容/错误版本/错误 ABI/无身份提供者。测试每次创建隔离目录，实际安装包并运行 C/C++ 消费程序；负例必须匹配预期诊断，不能凭任意工具失败冒充通过。

## 覆盖率证据

gcovr 8.3 测量 GCC coverage 构建，分母只有真实生产 `src/bytes.c`。测试、GoogleTest、消费者和生成文件不计入分母，测试对象与 GoogleTest 不插桩。

| 指标 | 命中 / 总数 | 比例 |
| --- | --- | --- |
| 行 | 62 / 62 | 100% |
| 函数 | 12 / 12 | 100% |
| 分支 | 24 / 24 | 100% |

原始结果为 `out/evidence/coverage.json` 与 `coverage-summary.json`。使用匹配的 gcov 和已准备的 gcovr 执行：

```sh
gcovr --root . --filter 'src/bytes.c' --json-summary out/evidence/coverage-summary.json --json out/evidence/coverage.json --fail-under-line 80 --fail-under-function 80 --fail-under-branch 80 out/coverage
```

Host 与 coverage 预设默认查找 `out/deps/gtest`；本次实际测试通过命令行 `CMAKE_PREFIX_PATH` 显式提供工作区已准备的安装包，没有把外部绝对路径写入仓库构建。MSVC 使用新的 `out/msvc-host` 构建目录及独立 MSVC 安装包。

## 尚未完成与后续接入

共享工具的本地接入已经完成，实际结果见下节；远端 CI 配置与实际执行分别记录。

尚未执行 Linux 远端 CI、实际 MCU/Boot 镜像链接、上板时序与资源测量。没有远端、发布 tag 或新的源码许可证。core/link 仍使用原接口，后续按提供者、消费者、产品的顺序迁移并移除旧实现；本仓库独立通过不等同于全部 S1 结束。

所有本地过程产物位于忽略的 `out/`。正式评审或发布应由协调任务保留所需日志制品并关联提交；不能只依赖某一开发者的临时目录。

## 共享质量工具验收（2026-10-01）

组件现在调用显式安装的 xgen-quality 0.1.0，源码固定为 `9c957d406d935d27959babe7f01172151f9d26c1`。检查逻辑、工具策略和回归由该独立仓库维护；本仓只保留模块配置与薄入口。最终 wheel 的 SHA256 为 `7a1946e9256d12e49365f064fd1fc44533d58e62addccc6b91268e2d657443f7`；声明的源码提交和实际 wheel 来源分别记录，不能相互代替。

Windows 本地实际完成 text、clang-format 19.1.5、Doxygen 1.16.0、cppcheck 2.21.0、clang-tidy 19.1.0、CTest 与 gcovr 8.3 检查，全部通过。原始结果在 `out/reports/quality-*.json`；工具自身的 66 项 Python 回归在 xgen-quality 执行，组件不再复制这套测试。实现阶段的报告保留当时源码和安装身份；最终 wheel 更新了模板与元数据，运行 Python 和 policy 内容已逐字节核对为受测版本。

已安装本地 pre-commit 钩子；暂存完整自有文件后运行 `python -m pre_commit run --all-files`，文本/配置与 X-Gen C/C++ 格式两项通过，`git diff --cached --check` 通过。三个消费者固定到同一工具提交。远端 CI 仍需真实可读取的工具仓库及 `XGEN_QUALITY_REPOSITORY` 变量，当前未执行。

共享 runner 运行最终 44/44 CTest 通过；自有生产对象的行、函数与分支覆盖率均为 100%。独立虚拟环境的固定依赖安装及 pip check 通过。

## 干净克隆与共同安装验证

在包含空格的新目录中，分别从 CRC `358659cfdf85718d2472dd9aa43bc5cf704d91c9`、bytes `9ce17afa3a572e9aa9db9a2f4e8f4b3802b6326f`、status `d60c8e02f0720b03f30d7ca725bd8834ccf7add3` 创建 `--no-hardlinks` 干净本地克隆，不复制开发缓存或 out。使用 GCC 13.2，保持 tests 默认 OFF，将 CXX 指向不存在路径后，三个纯 C Release 均完成配置、构建和安装。

共同安装前缀新增 CRC 10、bytes 8、status 9 个文件，共 27 个；安装清单没有交叉路径，每次安装前已有文件的 SHA256 均不变。三个生产工程与消费者的缓存均无 CXX/GoogleTest 条目。

真实消费者最小路径只导入 `xgcrc::crc8`、`xgb::bytes`、`xgs::status`，符号检查确认可执行文件不含 CRC16、status strings 和 GoogleTest；另一路径显式选择 strings。两个消费者的 CTest **2/2 通过**。此结果是基础库组合验证，不代表产品、ARM 或 Boot 验收。

可复现脚本和含 39 条命令、36 个检查、源提交及安装摘要的完整报告保存在本工作区 CRC 的 `out/verify_shared_install.py` 与 `out/reports/shared-install.json`。这些构建和检查产物由 out 排除在源码之外；后续提交仅补充和修正文档，不改已验证的生产实现。
