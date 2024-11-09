#ifndef AUDIOPROCESS_H
#define AUDIOPROCESS_H

#include <vector>
#include <mutex>
#include <condition_variable>
#include <opus/opus.h>
#include <cstdint>

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
    bool loadAudioFromFile(const std::string& filename, std::vector<int16_t>& audio_data);

    // 编码和解码
    std::vector<std::vector<uint8_t>> AudioProcess::encodeSegments(const std::vector<int16_t>& pcm_data);
    std::vector<int16_t> decode(const std::vector<uint8_t>& encoded_data);

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
