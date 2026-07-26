# 实机验证记录

## 2026-07-26：ESP32-S3-DevKitC-1-N8R8 首次上板

测试环境：macOS、ESP32-S3-DevKitC-1-N8R8、原生 USB 数据连接。该开发板的
UART 和原生 USB 接口均为 Micro-USB（Micro-B），不是 USB-C。

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
| 外接 GPIO4 按键 | 通过 | 常开按键接在 GPIO4 与 GND 之间；双击后 ChatGPT 正确回到前台 |
| BLE 配对与识别 | 通过 | macOS 显示 BLE 连接、VID/PID 正确、模拟电量 100% |
| BLE 外接按键 | 通过 | 仅通过 UART 口供电，原生 USB HID 断开；双击后 ChatGPT 正确回到前台 |
| USB/BLE 切换 | 通过 | 两接口同时连接时按键日志显示 USB；拔掉原生 USB 后自动显示 BLE |

首次通过手动 `BOOT + RST/EN` 进入下载模式，然后使用原生 USB Serial/JTAG 端口
烧录。烧录完成后单独按 `RST/EN` 启动应用，设备重新枚举为 Codex Micro。

Agent 1 的单击行为不会主动将 ChatGPT 带到前台，首次测试因此看起来没有响应。
改用 350 ms 内快速双击后，ChatGPT 正确切换到对应任务并回到前台，确认 USB HID
按键事件链路已打通。

外接按键首次接线时误将杜邦线套到与 GPIO4 相邻的 RST，引起按键时 USB HID
连接失效。ChatGPT 日志表现为 `could not read from HID device` 和
`transport-unavailable`。断电后将线移至 GPIO4，再次双击验证通过。后续接线检查必须
同时核对丝印和针脚位置，不能只按相邻顺序计数。

BLE 验证时将数据线从原生 USB 口移至 UART 口，仅由 UART 口为开发板供电。
macOS 确认原生 USB Codex Micro 不存在，同时 BLE Codex Micro 已连接，地址为开发板
蓝牙地址，VID/PID 为 `0x303A` / `0x8360`。外接 GPIO4 按键双击后 ChatGPT 正确
回到前台，确认事件未借道原生 USB。

传输切换验证时同时连接 UART 和原生 USB。UART 串口日志对同一外接按键明确显示
`Agent 1 down/up over USB`，证明 BLE 在线时原生 USB 具有优先级。随后保留 UART
供电并拔掉原生 USB，日志显示 `Agent 1 down/up over BLE`，证明无需重启即可自动
回退到 BLE。开发板接口类型只影响当前验证线材，不改变最终 PCB 使用 USB-C 的规划。
