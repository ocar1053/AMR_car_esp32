#include <Arduino.h> // 必須引入此標頭以使用 millis()
#include "EncoderManager.h"

EncoderManager::EncoderManager(uint8_t pins[][2], const uint8_t encoderCount, const uint16_t encoder_resolutions[]) {
    this->encoderCount = encoderCount;
    this->encoderResolutions = encoder_resolutions;

    // 初始化編碼器
    encoders = new ESP32Encoder[encoderCount];
    prevCounts = new int[encoderCount];
    for (uint8_t i = 0; i < encoderCount; i++) {
        // 單邊編碼器 (一圈 1000)
        encoders[i].attachSingleEdge(pins[i][0], pins[i][1]);
        // 半正交編碼器 (一圈 2000)
        // encoders[i].attachHalfQuad(pins[i][0], pins[i][1]);
        // 全正交編碼器 (一圈 4000)
        // encoders[i].attachFullQuad(pins[i][0], pins[i][1]);

        encoders[i].clearCount(); // 清除計數器
        encoders[i].setCount(0);  // 設置初始計數
        prevCounts[i] = 0;        // 設置先前計數
    }
    prevTime = millis(); // 記錄初始化時間
}

int *EncoderManager::getCounts() {
    static int counts[MAX_ENCODERS]; // 靜態陣列來儲存編碼器的計數值

    for (int i = 0; i < encoderCount; i++) {
        counts[i] = encoders[i].getCount(); // 獲取每個編碼器的當前計數
    }

    return counts; // 返回計數陣列
}

float *EncoderManager::getAngularVel() {
    static float vels[MAX_ENCODERS]; // 靜態陣列來儲存角速度值

    unsigned long currentTime = millis(); // 獲取當前時間
    float deltaTime = (currentTime - prevTime) / 1000.0; // 計算時間差（秒）
    int *counts = getCounts(); // 獲取當前編碼器計數
    for (int i = 0; i < this->encoderCount; i++) {
        int encoderDiff = counts[i] - prevCounts[i]; // 計算編碼器的增量
        if (encoderDiff == 0) {
            vels[i] = 0; // 如果沒有變化，速度為 0
            continue;
        }
        // 計算角速度 (單位：弧度/秒)
        vels[i] = (2 * PI * encoderDiff / this->encoderResolutions[i]) / deltaTime;
        // 計算角速度 (單位：每分鐘轉數, RPM)
        // vels[i] = (60.0 * encoderDiff / encoderResolutions[i]) / deltaTime;

        prevCounts[i] = counts[i]; // 更新先前計數值
    }

    prevTime = currentTime; // 更新先前時間

    return vels; // 返回角速度陣列
}
