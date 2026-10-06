/*
 * Winamp theme: Winamp 5 Modern / Frutiger Aero (skills/WINAMP.Graphics.md).
 * Brushed-metal frame, navy title bar and a cobalt LCD with a
 * seven-segment clock, reduced to the controls the watch needs:
 * one bevelled button per mode, separate + / - keys and the theme picker.
 */
#include "Gui.h"

#include <math.h>
#include <stdio.h>

static const Color FRAME_TOP = { 216, 222, 232, 255 };
static const Color FRAME_BOTTOM = { 160, 170, 184, 255 };
static const Color BEVEL_DARK = { 64, 72, 88, 255 };
static const Color TITLE_TOP = { 24, 72, 136, 255 };
static const Color TITLE_BOTTOM = { 8, 24, 48, 255 };
static const Color LCD_BG = { 9, 26, 56, 255 };
static const Color LCD_GLOW = { 112, 176, 255, 255 };
static const Color INK = { 24, 32, 48, 255 };

static const Rectangle LCD = { 14.0f, 44.0f, 412.0f, 268.0f };

static const enum GuiButton MODES[4] = { GB_CLOCK, GB_ALARM, GB_STOPWATCH, GB_TIMESET };

static void layout(GuiLayout* out, int themeCount, enum EWatchMode mode)
{
    for (int i = 0; i < 4; i++) {
        out->buttons[MODES[i]] = (Rectangle) { 30.0f + 100.0f * i, 340.0f, 80.0f, 80.0f };
    }

    if (mode == STOPWATCH_MODE) {
        /* round transport buttons: play/pause, then reset */
        out->buttons[GB_PLUS] = (Rectangle) { 130.0f, 462.0f, 76.0f, 76.0f };
        out->buttons[GB_MINUS] = (Rectangle) { 234.0f, 462.0f, 76.0f, 76.0f };
    } else {
        out->buttons[GB_PLUS] = (Rectangle) { 14.0f, 470.0f, 200.0f, 72.0f };
        out->buttons[GB_MINUS] = (Rectangle) { 226.0f, 470.0f, 200.0f, 72.0f };
    }
    out->buttons[GB_THEME] = (Rectangle) { 130.0f, 584.0f, 180.0f, 32.0f };

    for (int i = 0; i < themeCount && i < GUI_MAX_THEMES; i++) {
        out->menuItems[i] = (Rectangle) { 130.0f, 584.0f - 6.0f - 26.0f * (themeCount - i), 180.0f, 26.0f };
    }
}

static bool modeActive(enum GuiButton button, const WatchView* view)
{
    return (button == GB_CLOCK && view->mode == CLOCK_MODE)
        || (button == GB_ALARM && view->mode == ALARM_MODE)
        || (button == GB_STOPWATCH && view->mode == STOPWATCH_MODE)
        || (button == GB_TIMESET && view->mode == TIMESET_MODE);
}

static void bevel(Rectangle r, bool sunken)
{
    Color light = sunken ? BEVEL_DARK : WHITE;
    Color dark = sunken ? WHITE : BEVEL_DARK;
    DrawLineEx((Vector2) { r.x, r.y + 0.5f }, (Vector2) { r.x + r.width, r.y + 0.5f }, 1.0f, light);
    DrawLineEx((Vector2) { r.x + 0.5f, r.y }, (Vector2) { r.x + 0.5f, r.y + r.height }, 1.0f, light);
    DrawLineEx((Vector2) { r.x, r.y + r.height - 0.5f }, (Vector2) { r.x + r.width, r.y + r.height - 0.5f }, 1.0f, dark);
    DrawLineEx((Vector2) { r.x + r.width - 0.5f, r.y }, (Vector2) { r.x + r.width - 0.5f, r.y + r.height }, 1.0f, dark);
}

static void insetPanel(Rectangle r, Color fill)
{
    DrawRectangleRec(r, fill);
    /* inset 2px 2px 4px shadow */
    for (int i = 0; i < 4; i++) {
        float a = 0.36f * (4 - i) / 4.0f;
        DrawRectangleRec((Rectangle) { r.x, r.y + i, r.width, 1.0f }, Fade(BLACK, a));
        DrawRectangleRec((Rectangle) { r.x + i, r.y, 1.0f, r.height }, Fade(BLACK, a));
    }
    bevel(Gui_Inset(r, -1.0f), true);
}

static void drawFrame(void)
{
    DrawRectangleGradientV(0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT, FRAME_TOP, FRAME_BOTTOM);
    /* brushed metal grain */
    for (int y = 0; y < GUI_SCREEN_HEIGHT; y += 3) {
        DrawLine(0, y, GUI_SCREEN_WIDTH, y, Fade(WHITE, (y % 9 == 0) ? 0.10f : 0.04f));
    }
    bevel((Rectangle) { 0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT }, false);
    bevel((Rectangle) { 4, 4, GUI_SCREEN_WIDTH - 8, GUI_SCREEN_HEIGHT - 8 }, true);
}

static void drawTitleBar(void)
{
    const GuiFonts* fonts = Gui_Fonts();
    Rectangle bar = { 6.0f, 6.0f, GUI_SCREEN_WIDTH - 12.0f, 26.0f };
    DrawRectangleGradientV((int)bar.x, (int)bar.y, (int)bar.width, (int)bar.height, TITLE_TOP, TITLE_BOTTOM);
    DrawRectangle((int)bar.x, (int)bar.y, (int)bar.width, 1, Fade(WHITE, 0.4f));

    /* reflection ridges either side of the brand */
    for (int i = 0; i < 4; i++) {
        float y = bar.y + 8.0f + 3.0f * i;
        DrawLineEx((Vector2) { bar.x + 10.0f, y }, (Vector2) { 160.0f, y }, 1.0f, Fade(LCD_GLOW, 0.35f));
        DrawLineEx((Vector2) { 280.0f, y }, (Vector2) { bar.x + bar.width - 10.0f, y }, 1.0f, Fade(LCD_GLOW, 0.35f));
    }
    Gui_TextCentered(fonts->uiBold, "EWATCH", (Rectangle) { 160.0f, bar.y, 120.0f, bar.height }, 17.0f, 3.0f, WHITE);
}

static void lcdDigit(Rectangle r, float t, int digit, bool dim)
{
    Color on = dim ? Fade(LCD_GLOW, 0.15f) : LCD_GLOW;
    BeginBlendMode(BLEND_ADDITIVE);
    Gui_SegmentDigit(Gui_Inset(r, -1.5f), t + 3.0f, digit, Fade(LCD_GLOW, dim ? 0.03f : 0.18f), BLANK, false);
    EndBlendMode();
    Gui_SegmentDigit(r, t, digit, on, Fade(LCD_GLOW, 0.07f), false);
}

static float badge(float x, float y, const char* text, bool lit)
{
    const GuiFonts* fonts = Gui_Fonts();
    Vector2 size = MeasureTextEx(fonts->uiSmall, text, 14.0f, 0.0f);
    Rectangle r = { x, y, size.x + 14.0f, 18.0f };
    DrawRectangleRec(r, lit ? Fade(LCD_GLOW, 0.22f) : Fade(LCD_GLOW, 0.05f));
    DrawRectangleLinesEx(r, 1.0f, Fade(LCD_GLOW, lit ? 0.8f : 0.2f));
    Gui_TextCentered(fonts->uiSmall, text, r, 14.0f, 0.0f, lit ? LCD_GLOW : Fade(LCD_GLOW, 0.3f));
    return r.width;
}

static void drawLcd(const WatchView* view)
{
    const GuiFonts* fonts = Gui_Fonts();
    insetPanel(LCD, LCD_BG);

    /* Header: current mode and alarm status */
    char alarmText[32];
    snprintf(alarmText, sizeof(alarmText), "ALARM %02d:%02d %s", view->alarmHours, view->alarmMinutes, Gui_AlarmStateName(view->alarmState));
    Gui_TextLeft(fonts->uiSmall, "MODE", (Vector2) { LCD.x + 16.0f, LCD.y + 16.0f }, 14.0f, 0.0f, Fade(LCD_GLOW, 0.55f));
    Gui_TextLeft(fonts->uiSmall, Gui_ModeName(view->mode), (Vector2) { LCD.x + 58.0f, LCD.y + 16.0f }, 14.0f, 0.0f, LCD_GLOW);
    bool ringing = view->alarmState == ALARM_EXPIRED && fmodf(view->time, 0.5f) < 0.25f;
    Color alarmColor = view->alarmState == ALARM_OFF ? Fade(LCD_GLOW, 0.55f) : LCD_GLOW;
    if (view->alarmState == ALARM_EXPIRED) {
        alarmColor = ringing ? WHITE : Fade(LCD_GLOW, 0.4f);
    }
    float alarmW = MeasureTextEx(fonts->uiSmall, alarmText, 14.0f, 0.0f).x;
    Gui_TextLeft(fonts->uiSmall, alarmText, (Vector2) { LCD.x + LCD.width - 16.0f - alarmW, LCD.y + 16.0f }, 14.0f, 0.0f, alarmColor);

    /* Time: HH:MM:SS in seven-segment, tenths on the side */
    float dw = 40.0f, dh = 84.0f, t = 8.0f, gap = 8.0f, colon = 16.0f;
    float total = 6.0f * (dw + gap) + 2.0f * (colon + gap) + 20.0f;
    float x = LCD.x + floorf((LCD.width - total) / 2.0f);
    float y = LCD.y + 52.0f;
    int digits[6] = { view->hours / 10, view->hours % 10, view->minutes / 10, view->minutes % 10, view->seconds / 10, view->seconds % 10 };
    bool secondsDim = view->mode == ALARM_MODE || view->mode == TIMESET_MODE;
    for (int i = 0; i < 6; i++) {
        bool blank = (i < 2 && Gui_BlinkOff(view, true)) || (i >= 2 && i < 4 && Gui_BlinkOff(view, false));
        lcdDigit((Rectangle) { x, y, dw, dh }, t, blank ? -1 : digits[i], i >= 4 && secondsDim);
        x += dw + gap;
        if (i == 1 || i == 3) {
            bool colonOn = !(view->mode == CLOCK_MODE && view->tenths >= 5);
            Gui_SegmentColon((Rectangle) { x, y, colon, dh }, 8.0f, colonOn ? LCD_GLOW : Fade(LCD_GLOW, 0.15f));
            x += colon + gap;
        }
    }
    lcdDigit((Rectangle) { x, y + dh - 36.0f, 20.0f, 36.0f }, 4.0f, view->tenths, view->mode != STOPWATCH_MODE);

    /* Mode indicators */
    float by = LCD.y + 168.0f;
    float bx = LCD.x + 16.0f;
    bx += badge(bx, by, "CLK", view->mode == CLOCK_MODE) + 6.0f;
    bx += badge(bx, by, "ALM", view->mode == ALARM_MODE) + 6.0f;
    bx += badge(bx, by, "STW", view->mode == STOPWATCH_MODE) + 6.0f;
    bx += badge(bx, by, "SET", view->mode == TIMESET_MODE) + 18.0f;
    bx += badge(bx, by, "RUN", view->stopwatchRunning) + 6.0f;
    badge(bx, by, view->alarmState == ALARM_EXPIRED ? "RING" : "ARM", view->alarmState == ALARM_ON || ringing);

    /* Static dot-matrix hint line */
    Rectangle hint = { LCD.x + 10.0f, LCD.y + 204.0f, LCD.width - 20.0f, 40.0f };
    DrawRectangleRec(hint, Fade(BLACK, 0.35f));
    Gui_TextCentered(GetFontDefault(), Gui_Hint(view), hint, 20.0f, 2.0f, LCD_GLOW);

    /* Scanlines over the whole glass */
    for (float sy = LCD.y; sy < LCD.y + LCD.height; sy += 2.0f) {
        DrawRectangleRec((Rectangle) { LCD.x, sy, LCD.width, 1.0f }, Fade(BLACK, 0.22f));
    }
    DrawRectangleGradientV((int)LCD.x, (int)LCD.y, (int)LCD.width, 40, Fade(WHITE, 0.07f), Fade(WHITE, 0.0f));
}

static void capsule(Rectangle r, bool pressed, bool hovered)
{
    float round = Gui_Roundness(r, 12.0f);
    DrawRectangleRounded((Rectangle) { r.x + 1.0f, r.y + 2.0f, r.width, r.height }, round, 12, Fade(BLACK, pressed ? 0.1f : 0.3f));
    DrawRectangleRounded(r, round, 12, BEVEL_DARK);
    Rectangle face = Gui_Inset(r, 1.0f);
    Color top = pressed ? FRAME_BOTTOM : WHITE;
    Color bottom = pressed ? FRAME_TOP : FRAME_BOTTOM;
    DrawRectangleRounded(face, round, 12, bottom);
    DrawRectangleRounded((Rectangle) { face.x, face.y, face.width, face.height * 0.55f }, round, 12, ColorLerp(top, bottom, 0.3f));
    DrawRectangleRounded((Rectangle) { face.x + 6.0f, face.y + 2.0f, face.width - 12.0f, face.height * 0.35f }, round, 12, Fade(WHITE, pressed ? 0.1f : 0.45f));
    if (hovered) {
        DrawRectangleRoundedLinesEx(Gui_Inset(r, -1.0f), round, 12, 2.0f, Fade(LCD_GLOW, 0.9f));
    }
}

static void modeGlyph(enum GuiButton button, Vector2 c, Color color)
{
    switch (button) {
    case GB_CLOCK:
        DrawRing(c, 10.0f, 13.0f, 0.0f, 360.0f, 32, color);
        DrawLineEx(c, (Vector2) { c.x, c.y - 8.0f }, 2.5f, color);
        DrawLineEx(c, (Vector2) { c.x + 6.0f, c.y }, 2.5f, color);
        break;
    case GB_ALARM:
        DrawCircleSector((Vector2) { c.x, c.y + 4.0f }, 12.0f, 180.0f, 360.0f, 24, color);
        DrawRectangleRec((Rectangle) { c.x - 14.0f, c.y + 3.0f, 28.0f, 4.0f }, color);
        DrawCircleV((Vector2) { c.x, c.y + 10.0f }, 3.0f, color);
        break;
    case GB_STOPWATCH:
        DrawRing((Vector2) { c.x, c.y + 2.0f }, 9.0f, 12.0f, 0.0f, 360.0f, 32, color);
        DrawRectangleRec((Rectangle) { c.x - 4.0f, c.y - 14.0f, 8.0f, 4.0f }, color);
        DrawLineEx((Vector2) { c.x, c.y + 2.0f }, (Vector2) { c.x + 4.0f, c.y - 4.0f }, 2.5f, color);
        break;
    case GB_TIMESET:
        for (int i = 0; i < 3; i++) {
            float y = c.y - 9.0f + 9.0f * i;
            DrawLineEx((Vector2) { c.x - 13.0f, y }, (Vector2) { c.x + 13.0f, y }, 2.0f, color);
            DrawCircleV((Vector2) { c.x - 7.0f + 7.0f * ((i * 2) % 3), y }, 4.0f, color);
        }
        break;
    default:
        break;
    }
}

/* Circular bevel-edged transport button; `lit` gives it the blue accent. Returns its centre. */
static Vector2 roundButton(Rectangle r, bool pressed, bool lit, bool hovered)
{
    float radius = r.width / 2.0f;
    Vector2 c = { r.x + radius, r.y + radius };

    DrawCircleV((Vector2) { c.x + 1.0f, c.y + 3.0f }, radius, Fade(BLACK, 0.3f));
    Color top = pressed ? FRAME_BOTTOM : WHITE;
    Color bottom = pressed ? WHITE : FRAME_BOTTOM;
    if (lit) {
        top = pressed ? (Color) { 30, 80, 160, 255 } : (Color) { 170, 215, 255, 255 };
        bottom = pressed ? (Color) { 170, 215, 255, 255 } : (Color) { 30, 90, 180, 255 };
    }
    DrawCircleV(c, radius, bottom);
    DrawCircleGradient((int)c.x, (int)(c.y - radius * 0.35f), radius * 0.8f, top, Fade(top, 0.0f));
    DrawRing(c, radius - 1.5f, radius, 0.0f, 360.0f, 64, BEVEL_DARK);
    DrawRing(c, radius - 7.0f, radius - 6.0f, 200.0f, 340.0f, 32, Fade(WHITE, 0.7f));
    if (hovered) {
        DrawRing(c, radius + 1.0f, radius + 3.0f, 0.0f, 360.0f, 64, Fade(LCD_GLOW, 0.9f));
    }
    return pressed ? (Vector2) { c.x + 1.0f, c.y + 1.0f } : c;
}

static void drawModeButtons(const WatchView* view, const GuiLayout* layout, const GuiInput* input)
{
    const GuiFonts* fonts = Gui_Fonts();

    for (int i = 0; i < 4; i++) {
        enum GuiButton id = MODES[i];
        Rectangle r = layout->buttons[id];
        bool active = modeActive(id, view);
        Vector2 c = roundButton(r, input->pressed[id], active, input->hovered[id]);
        modeGlyph(id, c, active ? WHITE : INK);

        Gui_TextCentered(fonts->uiSmall, Gui_ButtonLabel(id, view), (Rectangle) { r.x - 10.0f, r.y + r.height + 8.0f, r.width + 20.0f, 14.0f }, 14.0f, 0.0f, INK);
    }
}

static void drawTransportButtons(const WatchView* view, const GuiLayout* layout, const GuiInput* input)
{
    const GuiFonts* fonts = Gui_Fonts();
    enum GuiButton ids[2] = { GB_PLUS, GB_MINUS };

    for (int i = 0; i < 2; i++) {
        enum GuiButton id = ids[i];
        Rectangle r = layout->buttons[id];
        bool lit = id == GB_PLUS && view->stopwatchRunning;
        Vector2 c = roundButton(r, input->pressed[id], lit, input->hovered[id]);
        Gui_AdjustIcon(id, view, c, 14.0f, 6.0f, lit ? WHITE : INK);

        const char* label = id == GB_PLUS ? (view->stopwatchRunning ? "PAUSE" : "PLAY") : "RESET";
        Gui_TextCentered(fonts->uiSmall, label, (Rectangle) { r.x - 10.0f, r.y + r.height + 8.0f, r.width + 20.0f, 14.0f }, 14.0f, 0.0f, INK);
    }
}

static void drawAdjustButtons(const WatchView* view, const GuiLayout* layout, const GuiInput* input)
{
    const GuiFonts* fonts = Gui_Fonts();
    enum GuiButton ids[2] = { GB_MINUS, GB_PLUS };

    for (int i = 0; i < 2; i++) {
        enum GuiButton id = ids[i];
        Rectangle r = layout->buttons[id];
        bool pressed = input->pressed[id];
        capsule(r, pressed, input->hovered[id]);

        float shift = pressed ? 1.0f : 0.0f;
        Vector2 c = { r.x + 44.0f + shift, r.y + r.height / 2.0f + shift };
        Gui_AdjustIcon(id, view, c, 14.0f, 6.0f, INK);

        const char* label;
        if (view->mode == STOPWATCH_MODE) {
            label = id == GB_PLUS ? (view->stopwatchRunning ? "STOP" : "START") : "RESET";
        } else {
            label = id == GB_PLUS ? "INCREASE" : "DECREASE";
        }
        Vector2 size = MeasureTextEx(fonts->uiBold, label, 17.0f, 1.0f);
        Gui_TextLeft(fonts->uiBold, label, (Vector2) { c.x + 34.0f, c.y - size.y / 2.0f }, 17.0f, 1.0f, INK);
    }
}

static void drawThemeButton(const WatchView* view, const GuiLayout* layout, const GuiInput* input)
{
    const GuiFonts* fonts = Gui_Fonts();
    Rectangle r = layout->buttons[GB_THEME];
    capsule(r, input->pressed[GB_THEME] || input->menuOpen, input->hovered[GB_THEME]);
    Gui_TextCentered(fonts->uiSmall, Gui_ButtonLabel(GB_THEME, view), r, 14.0f, 0.0f, INK);
}

static void drawMenu(const GuiLayout* layout, const GuiInput* input)
{
    const GuiFonts* fonts = Gui_Fonts();
    Rectangle first = layout->menuItems[0];
    Rectangle panel = { first.x - 3.0f, first.y - 3.0f, first.width + 6.0f, 26.0f * input->themeCount + 6.0f };
    DrawRectangleRec((Rectangle) { panel.x + 4.0f, panel.y + 4.0f, panel.width, panel.height }, Fade(BLACK, 0.35f));
    DrawRectangleGradientV((int)panel.x, (int)panel.y, (int)panel.width, (int)panel.height, FRAME_TOP, FRAME_BOTTOM);
    bevel(panel, false);

    for (int i = 0; i < input->themeCount; i++) {
        Rectangle item = layout->menuItems[i];
        bool hovered = i == input->menuHovered;
        if (hovered) {
            DrawRectangleGradientV((int)item.x, (int)item.y, (int)item.width, (int)item.height, TITLE_TOP, TITLE_BOTTOM);
        }
        Color color = hovered ? WHITE : INK;
        if (i == input->currentTheme) {
            DrawCircleV((Vector2) { item.x + 12.0f, item.y + item.height / 2.0f }, 3.0f, color);
        }
        Gui_TextLeft(fonts->uiSmall, input->themeNames[i], (Vector2) { item.x + 24.0f, item.y + 6.0f }, 14.0f, 0.0f, color);
    }
}

static void draw(const WatchView* view, const GuiLayout* layout, const GuiInput* input)
{
    drawFrame();
    drawTitleBar();
    drawLcd(view);
    drawModeButtons(view, layout, input);
    if (view->mode == STOPWATCH_MODE) {
        drawTransportButtons(view, layout, input);
    } else if (Gui_ButtonAvailable(GB_PLUS, view->mode)) {
        drawAdjustButtons(view, layout, input);
    }
    drawThemeButton(view, layout, input);

    if (input->menuOpen) {
        drawMenu(layout, input);
    }
}

const Theme ThemeWinamp = {
    "Winamp Modern",
    layout,
    draw,
    NULL,
};
