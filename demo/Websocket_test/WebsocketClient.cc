#include "WebsocketClient.h"
#include <websocketpp/common/asio.hpp>

// WebSocketClient 构造函数
WebSocketClient::WebSocketClient(const std::string& address, int port, const std::string& token, const std::string& deviceId, const std::string& protocolVersion) {
    ws_client_.init_asio();
    ws_client_.set_open_handler(bind(&WebSocketClient::on_open, this, std::placeholders::_1));
    ws_client_.set_message_handler(bind(&WebSocketClient::on_message, this, std::placeholders::_1, std::placeholders::_2));
    ws_client_.set_close_handler(bind(&WebSocketClient::on_close, this, std::placeholders::_1)); // 设置关闭处理器

    uri_ = "ws://" + address + ":" + std::to_string(port);

    headers_["Authorization"] = "Bearer " + token;
    headers_["Device-Id"] = deviceId;
    headers_["Protocol-Version"] = protocolVersion;

    websocketpp::lib::error_code ec;
    client_t::connection_ptr con = ws_client_.get_connection(uri_, ec);
    if (ec) {
        ws_client_.get_alog().write(websocketpp::log::alevel::fail, "Could not create connection: " + ec.message());
        return;
    }
    connection_hdl_ = con->get_handle();
}

// 日志记录方法
void WebSocketClient::Log(const std::string& message) {
    ws_client_.get_alog().write(websocketpp::log::alevel::app, message);
    // std::cout << "[WebSocketClient] " << message << std::endl; // 记录日志
}

// 连接函数
void WebSocketClient::Connect() {
    websocketpp::lib::error_code ec;
    client_t::connection_ptr con = ws_client_.get_connection(uri_, ec);
    if (ec) {
        ws_client_.get_alog().write(websocketpp::log::alevel::fail, "Could not create connection: " + ec.message());
        return;
    }

    for (const auto& header : headers_) {
        con->append_header(header.first, header.second);
    }
    connection_hdl_ = con->get_handle();
    ws_client_.connect(con);
    ws_client_.run();
    is_connected_ = false;
}

// 发送文本消息
void WebSocketClient::SendText(const std::string& message) {
    ws_client_.send(connection_hdl_, message, websocketpp::frame::opcode::text);
}

// 发送二进制数据
void WebSocketClient::SendBinary(const uint8_t* data, size_t size) {
    ws_client_.send(connection_hdl_, data, size, websocketpp::frame::opcode::binary);
}


// 设置消息回调
void WebSocketClient::SetMessageCallback(message_callback_t callback) {
    on_message_ = callback;
}

// 设置关闭回调
void WebSocketClient::SetCloseCallback(close_callback_t callback) {
    on_close_ = callback;
}

// 连接打开的回调
void WebSocketClient::on_open(websocketpp::connection_hdl hdl) {
    connection_hdl_ = hdl;
    ws_client_.get_alog().write(websocketpp::log::alevel::app, "Connection established!");
    is_connected_ = true;
}

// 消息接收的回调
void WebSocketClient::on_message(websocketpp::connection_hdl hdl, client_t::message_ptr msg) {
    if (on_message_) {
        on_message_(msg->get_payload());
    }
}

// 连接关闭的回调
void WebSocketClient::on_close(websocketpp::connection_hdl hdl) {
    ws_client_.get_alog().write(websocketpp::log::alevel::app, "Connection closed!");
    if (on_close_) {  // 调用用户设置的关闭回调
        on_close_();
    }
}
