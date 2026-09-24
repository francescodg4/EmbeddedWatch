#ifndef GUI_H
#define GUI_H

#include <stdbool.h>

#include "raylib.h"
#include "EWatch.h"

#define GUI_SCREEN_WIDTH 440
#define GUI_SCREEN_HEIGHT 640
#define GUI_MAX_THEMES 8

enum GuiButton {
    GB_CLOCK,
    GB_ALARM,
    GB_STOPWATCH,
    GB_TIMESET,
    GB_PLUS,
    GB_MINUS,
    GB_THEME,
    GB_COUNT
};

/* Snapshot of the watch taken once per frame; themes only read this. */
typedef struct {
    int hours;
    int minutes;
    int seconds;
    int tenths;
    enum EWatchMode mode;
    enum AlarmState alarmState;
    int alarmHours;
    int alarmMinutes;
    bool stopwatchRunning;
    bool editingHours;
    float time;
} WatchView;

/* Screen-space hit boxes, filled in by the active theme. */
typedef struct {
    Rectangle buttons[GB_COUNT];
    Rectangle menuItems[GUI_MAX_THEMES];
} GuiLayout;

typedef struct {
    bool hovered[GB_COUNT];
    bool pressed[GB_COUNT];
    bool menuOpen;
    int menuHovered;
    int currentTheme;
    int themeCount;
    const char* themeNames[GUI_MAX_THEMES];
} GuiInput;

typedef struct {
    const char* name;
    void (*layout)(GuiLayout* layout, int themeCount);
    void (*draw)(const WatchView* view, const GuiLayout* layout, const GuiInput* input);
    void (*unload)(void);
} Theme;

extern const Theme ThemeModern;
extern const Theme ThemeWinamp;
extern const Theme ThemeEmerald;

/* ---- Shared helpers (GuiCommon.c) ---- */

typedef struct {
    Font light;
    Font ui;
    Font uiBold;
    Font uiSmall;
} GuiFonts;

void Gui_LoadFonts(void);
void Gui_UnloadFonts(void);
const GuiFonts* Gui_Fonts(void);

const char* Gui_ButtonLabel(enum GuiButton button, const WatchView* view);
const char* Gui_ModeName(enum EWatchMode mode);
const char* Gui_Hint(const WatchView* view);
const char* Gui_AlarmStateName(enum AlarmState state);

/* True when the given time field should be hidden for the edit blink. */
bool Gui_BlinkOff(const WatchView* view, bool hoursField);

float Gui_Roundness(Rectangle rect, float radius);
Rectangle Gui_Inset(Rectangle rect, float amount);
Rectangle Gui_Scale(Rectangle rect, float scale);

void Gui_TextCentered(Font font, const char* text, Rectangle rect, float size, float spacing, Color color);
void Gui_TextLeft(Font font, const char* text, Vector2 pos, float size, float spacing, Color color);

/* Classic seven-segment digit; `digit` < 0 draws only unlit segments. */
void Gui_SegmentDigit(Rectangle rect, float thickness, int digit, Color on, Color off, bool rounded);
void Gui_SegmentColon(Rectangle rect, float thickness, Color color);

#endif /* GUI_H */
