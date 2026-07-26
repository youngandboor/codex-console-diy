# 实机验证记录

## 2026-07-26：ESP32-S3-DevKitC-1-N8R8 首次上板

测试环境：macOS、ESP32-S3-DevKitC-1-N8R8、原生 USB 数据连接。

| 项目 | 结果 | 记录 |
| --- | --- | --- |
| 系统识别开发板 | 通过 | 初始设备为 Espressif USB Serial/JTAG |
| 芯片识别 | 通过 | ESP32-S3 revision v0.2 |
| Flash | 通过 | 8 MB |
| PSRAM | 通过 | 工具报告 Embedded PSRAM 8 MB (AP_3v3) |
| 协议单元测试 | 通过 | 6/6 |
| N8R8 固件编译 | 通过 | `qio_opi`、`BOARD_HAS_PSRAM` |
| 固件烧录与校验 | 通过 | 全部区段 Hash 校验成功 |
| USB 产品名 | 通过 | Codex Micro |
| USB 厂商名 | 通过 | Work Louder |
| USB VID/PID | 通过 | `0x303A` / `0x8360` |
| ChatGPT Desktop 识别 | 通过 | 用户确认应用中出现 Codex Micro |
| BOOT 按键触发 Agent 1 | 通过 | 快速双击后 ChatGPT 切换到对应任务并回到前台；使用 GPIO0，仅用于开发板台架 |
| BLE 配对与识别 | 待测 | USB 单控件验证后进行 |
| USB/BLE 切换 | 待测 | 两种独立传输通过后进行 |

首次通过手动 `BOOT + RST/EN` 进入下载模式，然后使用原生 USB Serial/JTAG 端口
烧录。烧录完成后单独按 `RST/EN` 启动应用，设备重新枚举为 Codex Micro。

Agent 1 的单击行为不会主动将 ChatGPT 带到前台，首次测试因此看起来没有响应。
改用 350 ms 内快速双击后，ChatGPT 正确切换到对应任务并回到前台，确认 USB HID
按键事件链路已打通。
