#include <cstdio>
#include <array>
#include <functional>

#include <string>
#include <raylib.h>

#include <EWatch.h>
#include "EWatchEnums.h"
#include "timer.hpp"

namespace Settings {

constexpr int DefaultScreenWidth = 200;
constexpr int DefaultScreenHeight = 100;
constexpr Color BackgroundColor = Color { 0x34, 0x61, 0x31, 0xff };
constexpr Color TextColor = BLACK;
constexpr int TextSize = 16;

} // namespace Settings

class Application {
public:
    enum class Event {
        ClockTick,
        ActionPlus,
        ActionMinus,
        StopwatchModeSelected,
        ClockModeSelected,
        TimeSetModeSelected,
        AlarmModeSelected
    };

public:
    Application()
    {
        m_font = LoadFont("C:/Windows/Fonts/calibri.ttf");
        EWatch_Init(&m_ewatch);
    }

    ~Application()
    {
        UnloadFont(m_font);
    }

    void draw();

    void handle(const Event& event);

private:
    void draw_clock_mode();
    void draw_stopwatch_mode();
    void draw_alarm_mode();
    void draw_timeset_mode();

private:
    Font m_font;
    EWatch m_ewatch;
};

void Application::draw()
{
    BeginDrawing();
    {
        switch (EWatch_GetMode(&m_ewatch)) {
        case EWatchMode::CLOCK_MODE:
            draw_clock_mode();
            break;
        case EWatchMode::ALARM_MODE:
            draw_alarm_mode();
            break;
        case EWatchMode::TIMESET_MODE:
            draw_timeset_mode();
            break;
        case EWatchMode::STOPWATCH_MODE:
            draw_stopwatch_mode();
            break;
        default:
            break;
        }
    }
    EndDrawing();
}

void Application::draw_clock_mode()
{
    ClearBackground(Settings::BackgroundColor);

    std::array<char, 256> text;
    std::snprintf(
        text.data(),
        text.size(),
        "%02d:%02d:%02d",
        EWatch_GetHours(&m_ewatch),
        EWatch_GetMinutes(&m_ewatch),
        EWatch_GetSeconds(&m_ewatch));

    DrawTextEx(m_font, text.data(), Vector2 { 50, 50 }, Settings::TextSize, 1, Settings::TextColor);
}

void Application::draw_stopwatch_mode()
{
    ClearBackground(PURPLE);

    std::array<char, 256> text;
    std::snprintf(
        text.data(),
        text.size(),
        "%02d:%02d:%02d.%d",
        EWatch_GetHours(&m_ewatch),
        EWatch_GetMinutes(&m_ewatch),
        EWatch_GetSeconds(&m_ewatch),
        EWatch_GetTenths(&m_ewatch));

    DrawTextEx(m_font, text.data(), Vector2 { 50, 50 }, Settings::TextSize, 1, Settings::TextColor);
}

void Application::draw_alarm_mode()
{
    ClearBackground(BLUE);

    std::array<char, 256> text;

    std::string alarm_state;

    switch (EWatch_GetAlarmState(&m_ewatch)) {
    case AlarmState::ALARM_EXPIRED:
        alarm_state = "Expired";
        break;
    case AlarmState::ALARM_ON:
        alarm_state = "On";
        break;
    case AlarmState::ALARM_OFF:
        alarm_state = "Off";
        break;
    default:
        alarm_state = "";
        break;
    }

    std::snprintf(
        text.data(),
        text.size(),
        "Alarm: %s - %02d:%02d",
        alarm_state.c_str(),
        EWatch_GetHours(&m_ewatch),
        EWatch_GetMinutes(&m_ewatch));

    DrawTextEx(m_font, text.data(), Vector2 { 5, 50 }, Settings::TextSize, 1, Settings::TextColor);
}

void Application::draw_timeset_mode()
{
    ClearBackground(ORANGE);

    std::array<char, 256> text;
    std::snprintf(
        text.data(),
        text.size(),
        "%02d:%02d",
        EWatch_GetHours(&m_ewatch),
        EWatch_GetMinutes(&m_ewatch));

    EWatchTimesetState timeset_state = EWatch_GetTimesetState(&m_ewatch);

    if (timeset_state == EWatchTimesetState::TS_SET_HOURS_STATE) {
        DrawRectangle(50, 65, Settings::TextSize, 2, BLACK);

    } else if (timeset_state == EWatchTimesetState::TS_SET_MINUTES_STATE) {
        DrawRectangle(50 + 7 + Settings::TextSize, 65, Settings::TextSize, 2, BLACK);
    } else {
    }

    DrawTextEx(m_font, text.data(), Vector2 { 50, 50 }, Settings::TextSize, 1, Settings::TextColor);
}

void Application::handle(const Event& event)
{
    switch (event) {
    case Event::ClockTick:
        EWatch_Dispatch(&m_ewatch, EW_CLOCK_TICK_SIG);
        break;
    case Event::ActionMinus:
        EWatch_Dispatch(&m_ewatch, EW_BUTTON_M_SIG);
        break;
    case Event::ActionPlus:
        EWatch_Dispatch(&m_ewatch, EW_BUTTON_P_SIG);
        break;
    case Event::ClockModeSelected:
        EWatch_Dispatch(&m_ewatch, EW_CLOCK_MODE_SIG);
        break;
    case Event::StopwatchModeSelected:
        EWatch_Dispatch(&m_ewatch, EW_STOPWATCH_MODE_SIG);
        break;
    case Event::AlarmModeSelected:
        EWatch_Dispatch(&m_ewatch, EW_ALARM_MODE_SIG);
        break;
    case Event::TimeSetModeSelected:
        EWatch_Dispatch(&m_ewatch, EW_TIMESET_MODE_SIG);
        break;
    default:
        break;
    }
}

int main()
{
    SetTraceLogLevel(LOG_INFO);

    InitWindow(Settings::DefaultScreenWidth, Settings::DefaultScreenHeight, "Embedded Watch");

    SetWindowPosition(2500, 300);

    SetTargetFPS(30);

    Application app;

    Timer timer([&app]() -> void {
        app.handle(Application::Event::ClockTick);
    });

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_P)) {
            app.handle(Application::Event::ActionPlus);
        } else if (IsKeyPressed(KEY_M)) {
            app.handle(Application::Event::ActionMinus);
        } else if (IsKeyPressed(KEY_ONE)) {
            app.handle(Application::Event::ClockModeSelected);
        } else if (IsKeyPressed(KEY_TWO)) {
            app.handle(Application::Event::StopwatchModeSelected);
        } else if (IsKeyPressed(KEY_THREE)) {
            app.handle(Application::Event::AlarmModeSelected);
        } else if (IsKeyPressed(KEY_FOUR)) {
            app.handle(Application::Event::TimeSetModeSelected);
        }

        app.draw();
    }

    CloseWindow();

    return 0;
}
