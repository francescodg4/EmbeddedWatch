#include "Gui.h"

#include <time.h>

#define TICK_SECONDS 0.1f
#define KEY_FLASH_SECONDS 0.12f

static const Theme* const THEMES[] = {
    &ThemeModern,
    &ThemeWinamp,
    &ThemeEmerald,
};

static const int THEME_COUNT = (int)(sizeof(THEMES) / sizeof(THEMES[0]));

static void setClockFromSystem(EWatch* watch)
{
    time_t now = time(NULL);
    struct tm* local = localtime(&now);
    if (local != NULL) {
        EWatchClock_Set(&watch->clock, convertToTenths(local->tm_hour, local->tm_min, local->tm_sec, 0));
    }
}

static WatchView makeView(EWatch* watch, float elapsed)
{
    WatchView view;
    view.hours = EWatch_GetHours(watch);
    view.minutes = EWatch_GetMinutes(watch);
    view.seconds = EWatch_GetSeconds(watch);
    view.tenths = EWatch_GetTenths(watch);
    view.mode = EWatch_GetMode(watch);
    view.alarmState = EWatch_GetAlarmState(watch);
    view.alarmHours = EWatchAlarm_GetHours(&watch->alarm);
    view.alarmMinutes = EWatchAlarm_GetMinutes(&watch->alarm);
    view.stopwatchRunning = watch->stopwatch.state == ST_RUNNING_STATE;
    view.editingHours = watch->timeset.state == TS_SET_HOURS_STATE;
    view.time = elapsed;
    return view;
}

static void pressButton(EWatch* watch, enum GuiButton button, GuiInput* input)
{
    switch (button) {
    case GB_CLOCK:
        EWatch_Dispatch(watch, EW_CLOCK_MODE_SIG);
        break;
    case GB_ALARM:
        EWatch_Dispatch(watch, EW_ALARM_MODE_SIG);
        break;
    case GB_STOPWATCH:
        EWatch_Dispatch(watch, EW_STOPWATCH_MODE_SIG);
        break;
    case GB_TIMESET:
        EWatch_Dispatch(watch, EW_TIMESET_MODE_SIG);
        break;
    case GB_PLUS:
        EWatch_Dispatch(watch, EW_BUTTON_P_SIG);
        break;
    case GB_MINUS:
        EWatch_Dispatch(watch, EW_BUTTON_M_SIG);
        break;
    case GB_THEME:
        input->menuOpen = !input->menuOpen;
        input->menuHovered = -1;
        break;
    default:
        break;
    }
}

static int keyboardButton(void)
{
    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_C)) {
        return GB_CLOCK;
    }
    if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_A)) {
        return GB_ALARM;
    }
    if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_S)) {
        return GB_STOPWATCH;
    }
    if (IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_E)) {
        return GB_TIMESET;
    }
    if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD) || IsKeyPressed(KEY_UP)) {
        return GB_PLUS;
    }
    if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT) || IsKeyPressed(KEY_DOWN)) {
        return GB_MINUS;
    }
    if (IsKeyPressed(KEY_T)) {
        return GB_THEME;
    }
    return -1;
}

static void updateMenu(GuiInput* input, const GuiLayout* layout, Vector2 mouse)
{
    input->menuHovered = -1;
    for (int i = 0; i < input->themeCount; i++) {
        if (CheckCollisionPointRec(mouse, layout->menuItems[i])) {
            input->menuHovered = i;
        }
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if (input->menuHovered >= 0) {
            input->currentTheme = input->menuHovered;
            input->menuOpen = false;
        } else if (!CheckCollisionPointRec(mouse, layout->buttons[GB_THEME])) {
            input->menuOpen = false;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_T)) {
        input->menuOpen = false;
    }
    for (int i = 0; i < input->themeCount && i < 9; i++) {
        if (IsKeyPressed(KEY_ONE + i)) {
            input->currentTheme = i;
            input->menuOpen = false;
        }
    }
}

int main(void)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT, "EmbeddedWatch");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    Gui_LoadFonts();

    EWatch watch;
    EWatch_Init(&watch);
    setClockFromSystem(&watch);
    EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);

    GuiInput input = { 0 };
    input.menuHovered = -1;
    input.themeCount = THEME_COUNT;
    for (int i = 0; i < THEME_COUNT; i++) {
        input.themeNames[i] = THEMES[i]->name;
    }

    float keyFlash[GB_COUNT] = { 0 };
    float tickAccumulator = 0.0f;
    float elapsed = 0.0f;
    int armedButton = -1;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        elapsed += dt;

        /* Drive the state machine at 10 Hz, independent of frame rate. */
        tickAccumulator += dt;
        for (int guard = 0; tickAccumulator >= TICK_SECONDS && guard < 50; guard++) {
            EWatch_Dispatch(&watch, EW_CLOCK_TICK_SIG);
            tickAccumulator -= TICK_SECONDS;
        }

        const Theme* theme = THEMES[input.currentTheme];
        GuiLayout layout = { 0 };
        theme->layout(&layout, THEME_COUNT);

        Vector2 mouse = GetMousePosition();
        bool menuWasOpen = input.menuOpen;

        for (int i = 0; i < GB_COUNT; i++) {
            input.hovered[i] = !menuWasOpen && CheckCollisionPointRec(mouse, layout.buttons[i]);
        }
        input.hovered[GB_THEME] = CheckCollisionPointRec(mouse, layout.buttons[GB_THEME]);

        if (menuWasOpen) {
            updateMenu(&input, &layout, mouse);
        }

        /* Buttons fire on release, only if the press also started on them. */
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            armedButton = -1;
            for (int i = 0; i < GB_COUNT; i++) {
                if (input.hovered[i]) {
                    armedButton = i;
                }
            }
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (armedButton >= 0 && input.hovered[armedButton]) {
                if (armedButton != GB_THEME || !menuWasOpen || input.menuOpen) {
                    pressButton(&watch, (enum GuiButton)armedButton, &input);
                }
            }
            armedButton = -1;
        }

        if (!menuWasOpen) {
            int key = keyboardButton();
            if (key >= 0) {
                pressButton(&watch, (enum GuiButton)key, &input);
                keyFlash[key] = KEY_FLASH_SECONDS;
            }
        }

        for (int i = 0; i < GB_COUNT; i++) {
            if (keyFlash[i] > 0.0f) {
                keyFlash[i] -= dt;
            }
            input.pressed[i] = keyFlash[i] > 0.0f
                || (armedButton == i && input.hovered[i] && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
        }

        /* A theme switch may change the layout; recompute it for drawing. */
        theme = THEMES[input.currentTheme];
        theme->layout(&layout, THEME_COUNT);
        WatchView view = makeView(&watch, elapsed);

        BeginDrawing();
        theme->draw(&view, &layout, &input);
        EndDrawing();
    }

    for (int i = 0; i < THEME_COUNT; i++) {
        if (THEMES[i]->unload != NULL) {
            THEMES[i]->unload();
        }
    }
    Gui_UnloadFonts();
    CloseWindow();
    return 0;
}
