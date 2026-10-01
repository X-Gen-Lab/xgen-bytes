# 来源与授权记录

生产实现提取自 X-Gen-Lab/xgen-core 提交 `dc5eb1167ba21de384e7a760a8586b87502b73b2`。原 core 的来源记录将字节序列化追溯到 xgen-link 拆分基线 `a3b71e5`。保留原 X-Gen Lab 作者信息。

| 本仓库 | 来源与改动 |
| --- | --- |
| src/bytes.c | core src/bytes.c，仅 `xgc_` 改为 `xgb_`、公开包含路径与源码排版调整 |
| include/xgen/bytes/bytes.h | core include/xgc/bytes.h 的 12 个接口，补齐所有权、长度、空指针和并发契约 |
| tests/unit/test_bytes.cpp | core tests/test_core.c 的 bytes 向量和固定种子 4096 次往返；增加各对齐、全部空指针路径、边界及周围字节保护 |
| cmake 与集成测试 | 沿用 xgen-crc 试点的独立包模式；新增共享的预置 target 身份检查及对应 RED/GREEN 测试 |

来源没有确认完整的顶层源码许可证。本仓库不擅自增加 MIT、SPDX 或其他授权声明；正式公开发布前由仓库所有者确认授权。GoogleTest 和开发工具保留其各自上游授权，不随生产包安装。

## 开发配置与依赖

- GoogleTest 1.16.0：`https://github.com/google/googletest.git`，固定提交 `ff6133ab49b364a883a55ba75c39e520fea6245b`，显式安装的 Host 测试依赖。
- 格式及 EditorConfig 初始基线来自 Nexus 提交 `7a203266082ad1f655b6686713b2b7950ba31ec6`，通过 roadmap 规范 1.0.0 受控同步；初次副本仅调整配置标题和验证工具版本注释，格式选项保持一致。后续空行增补另记，不将当前配置等同于初始来源的全部选项。
- roadmap 规范基线：本地提交 `c018272a1bfa0a0ad0e811ceedcb22862640ed98`；规范正文属于 roadmap，本仓库仅维护采用声明。
- 通用质量检查由独立 xgen-quality 包提供；本仓库只保留薄入口和组件配置，不复制通用 runner 或其测试。

新公开符号不兼容旧 `xgc_` 链接名称；本仓库不提供旧名称包装。现有 core/link 消费者尚未切换，其旧实现由后续跨仓迁移统一退出，不能把本次独立实现视为迁移已经结束。

## 2026-10-01 格式配置增补

按 xgen-roadmap 尚未发布的工程规范 1.0.0 local 增补 C-020、C-021、DOC-013，受控 `.clang-format` 增加 `SeparateDefinitionBlocks: Always`、`KeepEmptyLines` 三项 false 和 `LineEnding: LF`，保留 `MaxEmptyLinesToKeep: 1`。当前格式模板 SHA-256 为 `ffdb331b03ae4f6c5f75ee54d4afaa6d4741f5ac3ec57f55b0f04ad8ec396a2a`；`.editorconfig` 的来源不变。

本轮源码迁移仅调整空行和 LF，不改变代码行为；来源、作者和许可证事实保持。共享质量工具的当前固定来源见 [tools/quality.json](tools/quality.json)，格式检查边界见 [规范采用记录](docs/standards.md)。这份记录不代表远端 CI 已执行。
