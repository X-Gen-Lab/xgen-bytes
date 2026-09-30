# 仓库工作约定

- 始终使用简体中文与用户沟通；修改前阅读 CONTRIBUTING.md、docs/standards.md 和 PROVENANCE.md。
- 遵循 Nexus 格式和反斜杠 Doxygen；使用受控 .clang-format，不回退到原 core 的排版。
- 生产保持 C11、无堆、无全局可变状态和外部组件依赖；Host GoogleTest 测试使用 C++17。
- 保持 12 个整数读写接口的大小端、非对齐和 NULL 行为。缓冲区容量是明确的调用前提，不擅自改为协议解析器或引入 status 依赖。
- 纯迁移先验证来源基线；新行为和缺陷修复记录真实 RED/GREEN。测试链接生产 target，不复制生产算法来计算期望值。
- 源码、安装和预置 target 消费统一验证版本/ABI；配置与构建不隐式下载依赖，不假设兄弟仓库路径。
- 质量检查统一调用 tools/quality.py，通用实现属于显式安装的 xgen-quality 包；不在本仓库复制 runner。
- 保护未提交成果，只处理当前任务；本地测试、远端 CI、发布及硬件结果分别记录，不虚构许可证或测量结果。
