#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <string>
#include <functional>
#include <map>
#include <iostream>  

struct BinProtocol {
    uint16_t version;
    uint16_t type;
    uint32_t payload_size;
    uint8_t payload[];
} __attribute__((packed));

class WebSocketClient {
public:
    using message_callback_t = std::function<void(const std::string&)>;
    using close_callback_t = std::function<void()>;

    WebSocketClient(const std::string& address, int port, const std::string& token, const std::string& deviceId, const std::string& protocolVersion);
    void Connect();
    void SendText(const std::string& message);
    void SendBinary(const uint8_t* data, size_t size);
    
    void SetMessageCallback(message_callback_t callback);
    void SetCloseCallback(close_callback_t callback); // 设置关闭回调
    void Log(const std::string& message); // 日志记录方法
    bool IsConnected() const { return is_connected_; }

private:
    using client_t = websocketpp::client<websocketpp::config::asio_client>;
    client_t ws_client_;
    websocketpp::connection_hdl connection_hdl_;
    std::map<std::string, std::string> headers_;
    message_callback_t on_message_;
    close_callback_t on_close_;  // 关闭回调
    std::string uri_;
    bool is_connected_ = false;

    void on_open(websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl hdl, client_t::message_ptr msg);
    void on_close(websocketpp::connection_hdl hdl);  // 处理关闭事件
};

#endif // WEBSOCKETCLIENT_H
