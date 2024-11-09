#include "AudioProcess.h"
#include <iostream>
#include <fstream>

AudioProcess::AudioProcess() : sample_rate(16000), channels(1), encoder(nullptr), decoder(nullptr) {
    if (!initializeOpus()) {
        Log("Failed to initialize Opus encoder/decoder.", ERROR);
    }
}

AudioProcess::~AudioProcess() {
    cleanupOpus();
}

void AudioProcess::Log(const std::string& message, LogLevel level) {
    std::string prefix = (level == ERROR) ? "[ERROR] " : "[INFO] ";
    std::cout << "[AudioProcess] " << prefix << message << std::endl;
}

bool AudioProcess::initializeOpus() {
    int error;

    // 初始化 Opus 编码器
    encoder = opus_encoder_create(sample_rate, channels, OPUS_APPLICATION_VOIP, &error);
    if (error != OPUS_OK) {
        Log("Opus encoder initialization failed: " + std::string(opus_strerror(error)), ERROR);
        return false;
    }

    // 初始化 Opus 解码器
    decoder = opus_decoder_create(sample_rate, channels, &error);
    if (error != OPUS_OK) {
        Log("Opus decoder initialization failed: " + std::string(opus_strerror(error)), ERROR);
        opus_encoder_destroy(encoder);
        return false;
    }
    return true;
}

void AudioProcess::cleanupOpus() {
    if (encoder) {
        opus_encoder_destroy(encoder);
    }
    if (decoder) {
        opus_decoder_destroy(decoder);
    }
}

bool AudioProcess::startRecording() {
    // 启动录音线程，录音逻辑依赖于实际音频库
    // 假设录音会填充内部的音频缓冲区
    return true;
}

void AudioProcess::stopRecording() {
    // 停止录音
}

std::vector<int16_t> AudioProcess::getRecordedAudio() {
    std::lock_guard<std::mutex> lock(audio_mutex);
    // 返回录音缓冲区中的数据
    return {};  // 返回实际录音数据
}

// 读取音频文件
bool AudioProcess::loadAudioFromFile(const std::string& filename, std::vector<int16_t>& audio_data) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        Log("Failed to open audio file: " + filename, ERROR);
        return false;
    }

    // 读取 PCM 数据到 audio_data
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    audio_data.resize(file_size / sizeof(int16_t));
    file.read(reinterpret_cast<char*>(audio_data.data()), file_size);
    file.close();

    Log("Loaded audio file successfully: " + filename, INFO);
    return true;
}

std::vector<std::vector<uint8_t>> AudioProcess::encodeSegments(const std::vector<int16_t>& pcm_data) {

    std::vector<std::vector<uint8_t>> encoded_segments;
    int frame_size = 20;  // 帧大小，单位为毫秒

    size_t num_frames = pcm_data.size() / frame_size;

    for (size_t i = 0; i < num_frames; ++i) {
        // 为当前帧准备数据
        const int16_t* frame_data = pcm_data.data() + i * frame_size;

        // 创建存储编码后的数据的缓冲区
        std::vector<uint8_t> encoded_data(frame_size);  // 预分配缓冲区

        // 对当前帧进行编码
        int encoded_bytes = opus_encode(encoder, frame_data, frame_size, encoded_data.data(), encoded_data.size());

        if (encoded_bytes < 0) {
            Log("Encoding failed at frame " + std::to_string(i) + ": " + std::string(opus_strerror(encoded_bytes)), ERROR);
            continue;  // 跳过失败的帧
        }

        // 调整编码数据的大小为实际编码的字节数
        encoded_data.resize(encoded_bytes);
        encoded_segments.push_back(encoded_data);

        Log("Frame " + std::to_string(i) + " encoded successfully. Encoded bytes: " + std::to_string(encoded_bytes), INFO);
    }

    return encoded_segments;
}


std::vector<int16_t> AudioProcess::decode(const std::vector<uint8_t>& encoded_data) {
    std::vector<int16_t> pcm_data(4096);  // 大小根据帧大小调整
    int decoded_samples = opus_decode(decoder, encoded_data.data(), encoded_data.size(), pcm_data.data(), pcm_data.size(), 0);

    if (decoded_samples < 0) {
        Log("Decoding failed: " + std::string(opus_strerror(decoded_samples)), ERROR);
        return {};
    }
    pcm_data.resize(decoded_samples * channels);
    Log("Decoding completed successfully.", INFO);
    return pcm_data;
}
