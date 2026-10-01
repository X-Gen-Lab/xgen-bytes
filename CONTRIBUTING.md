# 贡献指南

先阅读 [规范采用](docs/standards.md)、[来源](PROVENANCE.md) 和 [实施记录](docs/implementation-log.md)。新增行为和缺陷修复遵循 RED → GREEN → Refactor；纯算法迁移先运行原行为基线。记录实际失败原因、命令、源码状态和回归结果，不要求为了流程提交不可用代码。

## 显式准备依赖

生产构建不需要 GoogleTest。Host 测试使用 GoogleTest 1.16.0，来源为 `https://github.com/google/googletest.git` 的固定提交 `ff6133ab49b364a883a55ba75c39e520fea6245b`。

```sh
git clone --no-checkout https://github.com/google/googletest.git out/deps/gtest-src
git -C out/deps/gtest-src checkout --detach ff6133ab49b364a883a55ba75c39e520fea6245b
cmake -S out/deps/gtest-src -B out/deps/gtest-build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_GMOCK=OFF -DINSTALL_GTEST=ON -Dgtest_force_shared_crt=ON
cmake --build out/deps/gtest-build
cmake --install out/deps/gtest-build --prefix out/deps/gtest
```

这些是显式准备命令。已有同版本安装包可以通过 `CMAKE_PREFIX_PATH` 指定；其编译器、架构、配置和 C++ 运行库必须与测试匹配。Windows 可在 MSVC 开发终端使用 Ninja，也可给依赖及组件同时显式设置 GCC/G++。切换工具链时换一个构建目录。

质量工具由显式提供的 `xgen_quality-0.1.0-py3-none-any.whl` 安装进 Python 虚拟环境，依赖版本由 wheel 元数据固定。wheel 由组织构建或交付，不假设它已经公开发布；安装是显式准备步骤。

工具源码固定在 `tools/quality.json` 的 `quality_source.revision`。离线环境需先准备当前平台的 wheelhouse，再用 `--no-index --find-links` 安装；不要求某个固定兄弟目录存在。

```sh
python -m venv out/venv
```

PowerShell 激活 `./out/venv/Scripts/Activate.ps1`，POSIX shell 激活 `. out/venv/bin/activate`。随后使用实际 wheel 路径执行 `python -m pip install`，安装本地钩子使用 `python -m pre_commit install`。Cppcheck 与 Doxygen 根据质量包要求另行显式准备。配置、编译和检查期间不得自动下载。

## 构建与验证

```sh
cmake --preset host
cmake --build --preset host
ctest --preset host
cmake --preset release
cmake --build --preset release
```

Host 当前预期 **44** 个 CTest：30 个 GoogleTest 参数化用例与 14 个消费契约测试。集成测试使用新隔离目录，实际安装库并配置 C/C++ 消费者，包含未知 component、版本和预置 target 身份的拒绝路径。失败诊断必须对应预期契约，不能将任意配置失败当作通过。

质量包就绪后，本地和 CI 使用同一薄入口：

```sh
python tools/quality.py text
python tools/quality.py format
python tools/quality.py test --build-dir out/host
python tools/quality.py cppcheck --build-dir out/host
python tools/quality.py tidy --build-dir out/host
python tools/quality.py docs
python -m pre_commit run --all-files
```

工具缺失、版本不匹配、必需测试为空或漏跑必须失败。格式检查只读；主动修复使用固定的 clang-format 19.1.5，之后审阅差异。版本头模板包含 CMake 占位符，不直接用 C/C++ formatter 改写占位符；实际生成头通过独立 C/C++ 编译验证。

## 覆盖率

```sh
cmake --preset coverage
cmake --build --preset coverage
ctest --preset coverage
python tools/quality.py coverage --build-dir out/coverage
```

coverage preset 使用 GCC，需显式准备匹配的 GoogleTest、gcov 与质量工具；通过 `CMAKE_PREFIX_PATH` 可覆盖包位置。只插桩真实生产 `src/bytes.c`，测试与 GoogleTest 不进入生产分母。行、函数、分支分别至少 80%，不能靠平均值达标；每次测量使用干净的 coverage 构建目录，区分真实结果与未执行项。

## 评审与发布

任务说明包含来源基线、新契约 RED/GREEN、边界覆盖、独立消费、工具版本和实际验证范围。源码版本、ABI 与产品线协议版本分别管理；本组件不定义线协议。迁移顺序为提供者验证、消费者切换、产品更新固定源码组合，最后移除旧 core 实现。

当前未设置远端、未发布 tag，源码授权仍待确认。主机通过不代表 Linux CI 已运行，也不代表 64 KiB Flash / 8 KiB RAM 的完整 MCU/Boot 镜像已经验收。

CI 显式从 `X-Gen-Lab/xgen-quality` 获取配置中固定的工具提交，允许用 `XGEN_QUALITY_REPOSITORY` 覆盖为受控镜像。仓库覆盖、固定提交不可获取或安装版本不符时失败；不跟随依赖主分支，远端验证与本地结果分别记录。
