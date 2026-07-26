# 嘉立创 EDA 专业版工程

- 软件版本：嘉立创 EDA 专业版 `V3.2.166`
- 工程文件：`Codex Console Rev A.eprj2`
- 保存模式：本地半离线工程
- 当前内容：`Board1 / Schematic1 / P1 / PCB1 / mechanical-placement`

`mechanical-placement` 已从 `mechanical-placement-kicad.zip` 导入，并在 PCB 编辑器中
确认可见：包含 148 x 110 mm 板框、5 个 M3 安装孔、12 键包络、旋钮、摇杆、触控区、
电池区和天线区。原始的 `PCB1` 仍是空白占位页。

`Schematic1 / P1` 仍为空白，因为此前完成的是产品布局和 PCB 机械基线，并不是正式
电路原理图。后续将在这里完成 ESP32-S3、USB、电源、矩阵、旋钮、摇杆、RGB 的电路、
封装、布线、ERC、DRC 和生产文件导出。

不要直接用 SQLite 工具修改 `.eprj2`。所有设计变更必须通过嘉立创 EDA 专业版完成，
关闭或保存工程后再提交 Git。
