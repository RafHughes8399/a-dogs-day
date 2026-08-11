#include "debug_logger.h"

#include "config.h"
#include "events_interface.h"

#include <chrono>
#include <cstdio>
#include <ctime>

debug::logger& debug::logger::get_instance(){
    static logger instance;
    return instance;
}

debug::logger::logger()
: state_(std::make_unique<inactive>()),
debug_log_handler_([this](const events::debug_log& event) -> void {on_debug_log_event(event);}),
messages_({}),
frame_(0),
subscribed_(false),
paused_(false){}

void debug::logger::inactive::render(logger& logger){
    (void) logger;
}

void debug::logger::active::render(logger& logger){
    logger.render_backdrop();
    logger.render_messages();
}

void debug::logger::update(float delta){
    (void) delta;
    if(IsKeyPressed(debug_logger_config::toggle_key)){
        toggle();
    }
    if(subscribed_ and IsKeyPressed(debug_logger_config::pause_key)){
        toggle_pause();
    }
}

void debug::logger::render(){
    state_->render(*this);
}

void debug::logger::toggle(){
    if(subscribed_){
        unsubscribe();
        paused_ = false;
        state_ = std::make_unique<inactive>();
        return;
    }
    subscribe();
    state_ = std::make_unique<active>();
}

void debug::logger::toggle_pause(){
    paused_ = not paused_;
}

void debug::logger::set_frame(int frame){
    frame_ = frame;
}

void debug::logger::on_debug_log_event(const events::debug_log& event){
    if(paused_){
        return;
    }
    add_message(event.get_message());
}

void debug::logger::subscribe(){
    if(subscribed_){
        return;
    }
    event_interface::subscribe<events::debug_log>(debug_log_handler_);
    subscribed_ = true;
}

void debug::logger::unsubscribe(){
    if(not subscribed_){
        return;
    }
    event_interface::unsubscribe<events::debug_log>(debug_log_handler_);
    subscribed_ = false;
}

std::string debug::logger::timestamp(){
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &now_time);
#else
    localtime_r(&now_time, &local);
#endif

    char clock_buffer[16];
    std::strftime(clock_buffer, sizeof(clock_buffer), "%H:%M:%S", &local);

    char buffer[48];
    std::snprintf(buffer, sizeof(buffer), "[%s.%03d f%06d] ",
        clock_buffer, static_cast<int>(milliseconds.count()), frame_);
    return std::string(buffer);
}

void debug::logger::add_message(const std::string& message){
    messages_.push_back(timestamp() + message);
    while(messages_.size() > debug_logger_config::max_messages){
        messages_.pop_front();
    }
}

void debug::logger::render_backdrop(){
    auto screen_width = static_cast<float>(GetScreenWidth());
    auto screen_height = static_cast<float>(GetScreenHeight());
    auto backdrop = Rectangle{
        0.0f,
        screen_height * debug_logger_config::logger_y_position_scalar,
        screen_width,
        screen_height * debug_logger_config::logger_height_ratio
    };
    DrawRectangleRec(backdrop, debug_logger_config::backdrop);
}

void debug::logger::render_messages(){
    auto screen_height = static_cast<float>(GetScreenHeight());
    auto start_x = debug_logger_config::padding_x;
    auto start_y = static_cast<int>(screen_height * debug_logger_config::logger_y_position_scalar)
        + debug_logger_config::padding_y;

    int line_index = 0;
    for(const auto& message : messages_){
        DrawText(
            message.c_str(),
            start_x,
            start_y + (line_index * debug_logger_config::line_height),
            debug_logger_config::font_size,
            debug_logger_config::text);
        ++line_index;
    }
}
