import time
import pyaudio
import numpy as np

import time

# 全局变量，记录上次有声音的时间戳
last_silence_check_time = time.time()

def detect_silence(buf, silence_threshold=10000, silence_timeout=2):

    global last_silence_check_time

    audio_data = np.frombuffer(buf, dtype=np.int16)
    energy = np.sum(audio_data ** 2)

    current_time = time.time()
    print(energy,"\n")
    # 判断当前是否静音
    is_silent = energy <= silence_threshold

    # 更新静默检查时间或判断是否超时
    if is_silent:
        # 如果是静音，记录或更新开始静默的时间
        if current_time - last_silence_check_time >= silence_timeout:
            # 静默时间达到阈值，标记超时
            is_timeout = True
            last_silence_check_time = current_time  # 重置计时，防止连续超时立即触发
        else:
            is_timeout = False
    else:
        # 有声音，重置静默检测时间
        last_silence_check_time = current_time
        is_timeout = False

    return is_silent, is_timeout

if __name__ == "__main__":
    p = pyaudio.PyAudio()
    stream = p.open(format=pyaudio.paInt16, channels=1, rate=16000, input=True, frames_per_buffer=8000)
    frameSize = 8000  # 每一帧的音频大小
    intervel = 0.04  # 发送音频间隔(单位:s)
    while(1):
        buf = stream.read(frameSize)
        is_silent, is_long_silent = detect_silence(buf, silence_threshold=2000)
        if is_long_silent:
            print("检测到静默，停止录音。")
            break

        time.sleep(intervel)
    stream.stop_stream()
    stream.close()
    # pyaudio释放
    p.terminate()

