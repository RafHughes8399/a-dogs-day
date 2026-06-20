#include "debug_logger.h"

#include "config.h"
#include "events_interface.h"

debug::logger& debug::logger::get_instance(){
    static logger instance;
    return instance;
}

debug::logger::logger()
: state_(std::make_unique<inactive>()),
debug_log_handler_([this](const events::debug_log& event) -> void {on_debug_log_event(event);}),
messages_({}),
subscribed_(false){}

void debug::logger::inactive::render(logger& logger){
    (void) logger;
}

void debug::logger::active::render(logger& logger){
    logger.render_backdrop();
    logger.render_messages();
}

void debug::logger::update(float delta){
    (void) delta;
    bool wants_toggle = IsKeyPressed(debug_logger_config::toggle_key)
        && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT));
    if(wants_toggle){
        toggle();
    }
}

void debug::logger::render(){
    state_->render(*this);
}

void debug::logger::toggle(){
    if(subscribed_){
        unsubscribe();
        state_ = std::make_unique<inactive>();
        return;
    }
    subscribe();
    state_ = std::make_unique<active>();
}

void debug::logger::on_debug_log_event(const events::debug_log& event){
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
    if(! subscribed_){
        return;
    }
    event_interface::unsubscribe<events::debug_log>(debug_log_handler_);
    subscribed_ = false;
}

void debug::logger::add_message(const std::string& message){
    messages_.push_back(message);
    if(messages_.size() > debug_logger_config::max_messages){
        messages_.erase(messages_.begin());
    }
}

void debug::logger::render_backdrop(){
    auto screen_width = static_cast<float>(GetScreenWidth());
    auto screen_height = static_cast<float>(GetScreenHeight());
    auto backdrop = Rectangle{
        0.0f,
        screen_height * debug_logger_config::backdrop_y_ratio,
        screen_width,
        screen_height * debug_logger_config::backdrop_height_ratio
    };
    DrawRectangleRec(backdrop, debug_logger_config::backdrop);
}

void debug::logger::render_messages(){
    auto screen_height = static_cast<float>(GetScreenHeight());
    auto start_x = debug_logger_config::padding_x;
    auto start_y = static_cast<int>(screen_height * debug_logger_config::backdrop_y_ratio)
        + debug_logger_config::padding_y;
    auto max_visible_lines = static_cast<size_t>(
        ((screen_height * debug_logger_config::backdrop_height_ratio)
            - static_cast<float>(debug_logger_config::padding_y * 2))
        / static_cast<float>(debug_logger_config::line_height));
    auto first_message = messages_.size() > max_visible_lines
        ? messages_.size() - max_visible_lines
        : 0;

    for(size_t i = first_message; i < messages_.size(); ++i){
        auto line_index = static_cast<int>(i - first_message);
        DrawText(
            messages_[i].c_str(),
            start_x,
            start_y + (line_index * debug_logger_config::line_height),
            debug_logger_config::font_size,
            debug_logger_config::text);
    }
}
