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
    std::cout << "[AudioProcess]" << prefix << message << std::endl;
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

std::queue<std::vector<int16_t>> AudioProcess::loadAudioFromFile(const std::string& filename, int frame_duration_ms) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return {};
    }

    // 获取文件大小
    infile.seekg(0, std::ios::end);
    std::streampos fileSize = infile.tellg();
    infile.seekg(0, std::ios::beg);

    // 计算样本数量
    size_t numSamples = static_cast<size_t>(fileSize) / sizeof(int16_t);

    // 读取音频数据
    std::vector<int16_t> audio_data(numSamples);
    infile.read(reinterpret_cast<char*>(audio_data.data()), fileSize);

    if (!infile) {
        std::cerr << "Error reading file: " << filename << std::endl;
        return {};
    }

    // 计算每帧的样本数量
    int frame_size = sample_rate / 1000 * frame_duration_ms;

    // 将音频数据切分成帧
    std::queue<std::vector<int16_t>> audio_frames;
    for (size_t i = 0; i < numSamples; i += frame_size) {
        size_t remaining_samples = numSamples - i;
        size_t current_frame_size = (remaining_samples > frame_size) ? frame_size : remaining_samples;

        std::vector<int16_t> frame(current_frame_size);
        std::copy(audio_data.begin() + i, audio_data.begin() + i + current_frame_size, frame.begin());
        audio_frames.push(frame);
    }

    return audio_frames;
}


bool AudioProcess::encode(const std::vector<int16_t>& pcm_frame, uint8_t* opus_data, size_t& opus_data_size) {

    if (!encoder) {
        Log("encoder not init", ERROR);
        return false;
    }

    int frame_size = pcm_frame.size();

    if (frame_size <= 0) {
        Log("Invalid PCM frame size: " + std::to_string(frame_size), ERROR);
        return false;
    }

    // 对当前帧进行编码
    int encoded_bytes_size = opus_encode(encoder, pcm_frame.data(), frame_size, opus_data, 2048); //max 2048 bytes

    if (encoded_bytes_size < 0) {
        Log("Encoding failed: " + std::string(opus_strerror(encoded_bytes_size)), ERROR);
        return false;
    }

    opus_data_size = static_cast<size_t>(encoded_bytes_size);
    return true;
    
}


bool AudioProcess::decode(const uint8_t* opus_data, size_t opus_data_size, std::vector<int16_t>& pcm_frame) {
    if (!decoder) {
        Log("Decoder not initialized", ERROR);
        return false;
    }

    int frame_size = 960;  // 20ms 帧, 16000Hz 采样率, 理论上应该是 320 个样本，但是 Opus 限制为 960
    pcm_frame.resize(frame_size * channels);

    // 对当前帧进行解码
    int decoded_samples = opus_decode(decoder, opus_data, static_cast<int>(opus_data_size), pcm_frame.data(), frame_size, 0);

    if (decoded_samples < 0) {
        Log("Decoding failed: " + std::string(opus_strerror(decoded_samples)), ERROR);
        return false;
    }

    pcm_frame.resize(decoded_samples * channels);
    return true;
}

BinProtocol* AudioProcess::PackBinFrame(const uint8_t* payload, size_t payload_size) {

    // Allocate memory for BinaryProtocol + payload
    auto pack = (BinProtocol*)malloc(sizeof(BinProtocol) + payload_size);
    if (!pack) {
        std::cerr << "Memory allocation failed" << std::endl;
        return nullptr;
    }
    
    pack->version = htons(1);
    pack->type = htons(0);  // Indicate audio data type
    pack->payload_size = htonl(payload_size);
    assert(sizeof(BinProtocol) == 8);
    
    // Copy payload data
    memcpy(pack->payload, payload, payload_size);
    
    return pack;
}
