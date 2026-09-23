#include "debug_logger.h"

#include "config.h"
#include "events_interface.h"

#include <filesystem>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <iterator>
#include <string>
#include <utility>

debug::logger& debug::logger::get_instance(){
    static logger instance;
    return instance;
}


void debug::logger::inactive::render(logger& logger){
    (void) logger;
}

void debug::logger::active::render(logger& logger){
    logger.render_backdrop();
    logger.render_messages();
}

void debug::logger::render(){
    state_->render(*this);
}

void debug::logger::toggle(){
    if(active_){
        state_ = std::make_unique<inactive>();
    }
    else{
        state_ = std::make_unique<active>();
    }
    active_ = not active_;
}
void debug::logger::set_frame(int frame){
    frame_ = frame;
}

void debug::logger::on_debug_log_event(const events::debug_log& event){
    add_message(event.get_message());
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
    auto line = timestamp() + message;
    output_file_ << line << std::endl;
    messages_.push_back(std::move(line));
    while(messages_.size() > debug_logger_config::max_messages){
        messages_.pop_front();
    }
}

void debug::logger::render_backdrop(){
    auto screen_width = level_config::screen_width;
    auto screen_height = level_config::screen_height;
    auto backdrop = Rectangle{
        0.0f,
        screen_height * debug_logger_config::logger_y_position_scalar,
        screen_width,
        screen_height * debug_logger_config::logger_height_ratio
    };
    DrawRectangleRec(backdrop, debug_logger_config::backdrop);
}

void debug::logger::render_messages(){
    auto screen_height = level_config::screen_height;
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

size_t debug::logger::get_num_log_files(){
    auto files = std::filesystem::directory_iterator(debug_logger_config::log_directory);
    return static_cast<size_t>(std::distance(std::filesystem::begin(files), std::filesystem::end(files)));
}

std::filesystem::path debug::logger::get_log_file_path(){
    std::filesystem::create_directories(debug_logger_config::log_directory);
    auto run = get_num_log_files();
    std::filesystem::path path;
    do{
        ++run;
        path = std::filesystem::path(debug_logger_config::log_directory) / ("debug_run_" + std::to_string(run) + ".txt");
    } while(std::filesystem::exists(path));
    return path;
}
