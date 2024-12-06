## easyai
AI detects driver drowsiness using the RV1103 platform.

## compile
./build-linux.sh

## usage
excute './adb_push.sh'

## flowchart

```mermaid
flowchart TD
    A["开机"] --> B{"是否检测到人脸？"}
    B -->|是| C["播放提示音：已检测到驾驶员"]
    B -->|否| D["播放提示音：摄像头正对前方"]
    C --> E["开始检测驾驶机状态"]
    E --> F["检测2秒后发现闭眼"]
    E --> G["玩手机5秒"]
    E --> H["左顾右盼7秒"]
    E --> I["检测低头2秒"]
    E --> J["检测打哈欠2秒"]
    F --> K["播放提示音：注意疲劳驾驶"]
    G --> L["播放提示音：请将手机放下专注驾驶"]
    H --> M["播放提示音：请勿左顾右盼，请专心"]
    I --> N["播放提示音：请将目视前方，保持专注"]
    J --> O["播放提示音：注意疲劳，请合理安排休息"]
    D --> B
```

[上车测试视频](https://www.bilibili.com/video/BV1fTizYMEKV?vd_source=1b1673dce03ec3272c7540d6228cc14b)

## license
MIT License by Jim
