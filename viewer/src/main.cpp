#include <cstdio>
#include <raylib.h>

#include <EWatch.h>

namespace settings {

const int DefaultScreenWidth = 800;
const int DefaultScreenHeight = 800;
const Color DefaultBackgroundColor { 0x00, 0x00, 0x00, 0x00 };

}; // namespace settings

int main(void)
{
    SetTraceLogLevel(LOG_INFO);

    InitWindow(settings::DefaultScreenWidth, settings::DefaultScreenHeight, "EWatch - Viewer");

    SetTargetFPS(20);

    EWatch watch;
    EWatch_Init(&watch);

    const float FixedDeltaTime = 0.1f; // Fixed tick every 100ms
    float accumulator = 0.0f;

    while (!WindowShouldClose()) {
        accumulator += GetFrameTime();

        while (accumulator >= FixedDeltaTime) {
            EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
            accumulator -= FixedDeltaTime;
        }

        if (IsKeyPressed(KEY_ONE)) {
            EWatch_Dispatch(&watch, EW_CLOCK_MODE_SIG);
        } else if (IsKeyPressed(KEY_TWO)) {
            EWatch_Dispatch(&watch, EW_STOPWATCH_MODE_SIG);
        } else if (IsKeyPressed(KEY_THREE)) {
            EWatch_Dispatch(&watch, EW_TIMESET_MODE_SIG);
        } else if (IsKeyPressed(KEY_FOUR)) {
            EWatch_Dispatch(&watch, EW_ALARM_MODE_SIG);
        } else if (IsKeyDown(KEY_KP_ADD)) {
            EWatch_Dispatch(&watch, EW_BUTTON_P_SIG);
        } else if (IsKeyDown(KEY_KP_SUBTRACT)) {
            EWatch_Dispatch(&watch, EW_BUTTON_M_SIG);
        }

        int hours = EWatch_GetHours(&watch);
        int minutes = EWatch_GetMinutes(&watch);
        int seconds = EWatch_GetSeconds(&watch);
        int tenths = EWatch_GetTenths(&watch);
        AlarmState alarm_state = EWatch_GetAlarmState(&watch);
        EWatchMode mode = EWatch_GetMode(&watch);

        BeginDrawing();
        {
            ClearBackground(settings::DefaultBackgroundColor);

            switch (mode) {
            case CLOCK_MODE: {
                char text[250];
                if (alarm_state == AlarmState::ALARM_OFF) {
                    snprintf(text, sizeof(text), "Clock: %02d:%02d:%02d", hours, minutes, seconds);
                } else if (alarm_state == AlarmState::ALARM_ON) {
                    snprintf(text, sizeof(text), "Clock: %02d:%02d:%02d - [ON]", hours, minutes, seconds);
                } else {
                    snprintf(text, sizeof(text), "Clock: %02d:%02d:%02d - [EXP]", hours, minutes, seconds);
                }
                DrawText(text, 50, 50, 32, WHITE);
                break;
            }
            case ALARM_MODE: {
                char text[250];
                if (alarm_state == AlarmState::ALARM_OFF) {
                    snprintf(text, sizeof(text), "Alarm: %02d:%02d - [OFF]", hours, minutes);
                } else if (alarm_state == AlarmState::ALARM_ON) {
                    snprintf(text, sizeof(text), "Alarm: %02d:%02d - [ON]", hours, minutes);
                } else {
                    snprintf(text, sizeof(text), "Alarm: %02d:%02d - [EXP]", hours, minutes);
                }
                DrawText(text, 50, 50, 32, WHITE);

                if (EWatchAlarm_GetAlarmDisplayState(&watch.alarm) == ALARM_DISPLAY_SET_HOURS) {
                    DrawText("SetHours", 50, 150, 32, WHITE);
                } else if (EWatchAlarm_GetAlarmDisplayState(&watch.alarm) == ALARM_DISPLAY_SET_MINUTES) {
                    DrawText("SetMinutes", 50, 150, 32, WHITE);
                }

                break;
            }
            case STOPWATCH_MODE: {
                char text[250];
                snprintf(text, sizeof(text), "Stopwatch: %02d:%02d:%02d.%01d", hours, minutes, seconds, tenths);
                DrawText(text, 50, 50, 32, WHITE);
                break;
            }
            case TIMESET_MODE: {
                char text[250];
                snprintf(text, sizeof(text), "Timeset: %02d:%02d", hours, minutes);
                DrawText(text, 50, 50, 32, WHITE);

                if (watch.timeset.state == TS_SET_HOURS_STATE) {
                    DrawText("SetHours", 50, 150, 32, WHITE);
                } else if (watch.timeset.state == TS_SET_MINUTES_STATE) {
                    DrawText("SetMinutes", 50, 150, 32, WHITE);
                }

                break;
            }
            }
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
