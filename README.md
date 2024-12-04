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

[上车红外测试](https://github.com/dreamflyforever/easyai/blob/main/video/ir.mov)

[上车正常测试](https://github.com/dreamflyforever/easyai/blob/main/video/realscene.mov)

[多人测试](https://github.com/dreamflyforever/easyai/blob/main/video/multipeople.mov)

[模型测试](https://github.com/dreamflyforever/easyai/blob/main/video/testmodel.mov)

## license
MIT License by Jim
