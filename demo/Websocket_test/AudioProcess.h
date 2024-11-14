#ifndef AUDIOPROCESS_H
#define AUDIOPROCESS_H

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <opus/opus.h>
#include <cstdint>
#include "WebsocketClient.h"

class AudioProcess {
public:
    AudioProcess();
    ~AudioProcess();

    enum LogLevel { INFO, ERROR };
    void Log(const std::string& message, LogLevel level = INFO);

    // 录音方法
    bool startRecording();
    void stopRecording();
    std::vector<int16_t> getRecordedAudio();

    // 读取本地音频
    std::queue<std::vector<int16_t>> loadAudioFromFile(const std::string& filename, int frame_duration_ms);

    // 编码
    bool encode(const std::vector<int16_t>& pcm_frame, uint8_t* opus_data, size_t& opus_data_size);

    // 解码
    bool decode(const uint8_t* opus_data, size_t opus_data_size, std::vector<int16_t>& pcm_frame);

    // 打包音频帧为定义的二进制协议
    BinProtocol* PackBinFrame(const uint8_t* payload, size_t payload_size);

private:

    // 录音设备和线程的相关数据
    std::mutex audio_mutex;
    std::condition_variable audio_condition;

    // Opus 编码器和解码器的状态
    OpusEncoder* encoder;
    OpusDecoder* decoder;

    // 设置录音的采样率和通道数
    int sample_rate;
    int channels;

    // 初始化和释放编码器、解码器
    bool initializeOpus();
    void cleanupOpus();
};

#endif // AUDIOPROCESS_H
