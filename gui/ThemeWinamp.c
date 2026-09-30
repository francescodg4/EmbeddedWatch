/*
 * Winamp theme: Winamp 5 Modern / Frutiger Aero (skills/WINAMP.Graphics.md).
 * Brushed-metal frame, navy title bar, cobalt LCD telemetry with a
 * seven-segment clock, spectrum analyzer and marquee, bevelled transport
 * deck and a decorative equalizer module.
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

static const Rectangle LCD = { 14.0f, 58.0f, 412.0f, 196.0f };
static const Rectangle SEEK = { 20.0f, 266.0f, 400.0f, 10.0f };
static const Rectangle EQ = { 14.0f, 424.0f, 412.0f, 170.0f };

static const enum GuiButton DECK[6] = { GB_MINUS, GB_CLOCK, GB_ALARM, GB_STOPWATCH, GB_TIMESET, GB_PLUS };

static void layout(GuiLayout* out, int themeCount, enum EWatchMode mode)
{
    (void)mode;
    for (int i = 0; i < 6; i++) {
        out->buttons[DECK[i]] = (Rectangle) { 24.0f + 68.0f * i, 294.0f, 56.0f, 56.0f };
    }

    out->buttons[GB_THEME] = (Rectangle) { 292.0f, 604.0f, 134.0f, 26.0f };

    for (int i = 0; i < themeCount && i < GUI_MAX_THEMES; i++) {
        out->menuItems[i] = (Rectangle) { 250.0f, 604.0f - 6.0f - 26.0f * (themeCount - i), 176.0f, 26.0f };
    }
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
        float a = 0.6f * (4 - i) / 4.0f;
        DrawRectangleRec((Rectangle) { r.x, r.y + i, r.width, 1.0f }, Fade(BLACK, a * 0.6f));
        DrawRectangleRec((Rectangle) { r.x + i, r.y, 1.0f, r.height }, Fade(BLACK, a * 0.6f));
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
    Rectangle bar = { 6.0f, 6.0f, GUI_SCREEN_WIDTH - 12.0f, 24.0f };
    DrawRectangleGradientV((int)bar.x, (int)bar.y, (int)bar.width, (int)bar.height, TITLE_TOP, TITLE_BOTTOM);
    DrawRectangle((int)bar.x, (int)bar.y, (int)bar.width, 1, Fade(WHITE, 0.4f));

    /* reflection ridges either side of the brand */
    for (int i = 0; i < 4; i++) {
        float y = bar.y + 7.0f + 3.0f * i;
        DrawLineEx((Vector2) { bar.x + 10.0f, y }, (Vector2) { 160.0f, y }, 1.0f, Fade(LCD_GLOW, 0.35f));
        DrawLineEx((Vector2) { 280.0f, y }, (Vector2) { 360.0f, y }, 1.0f, Fade(LCD_GLOW, 0.35f));
    }
    Gui_TextCentered(fonts->uiBold, "EWATCH", (Rectangle) { 160.0f, bar.y, 120.0f, bar.height }, 17.0f, 3.0f, WHITE);

    /* window controls */
    const char* glyphs[3] = { "_", "=", "x" };
    for (int i = 0; i < 3; i++) {
        Rectangle b = { 366.0f + 20.0f * i, bar.y + 5.0f, 16.0f, 14.0f };
        DrawRectangleGradientV((int)b.x, (int)b.y, (int)b.width, (int)b.height, FRAME_TOP, FRAME_BOTTOM);
        bevel(b, false);
        Gui_TextCentered(fonts->uiSmall, glyphs[i], b, 14.0f, 0.0f, INK);
    }

    const char* menus[5] = { "File", "Mode", "Options", "View", "Help" };
    float x = 14.0f;
    for (int i = 0; i < 5; i++) {
        Gui_TextLeft(fonts->uiSmall, menus[i], (Vector2) { x, 36.0f }, 14.0f, 0.0f, INK);
        x += MeasureTextEx(fonts->uiSmall, menus[i], 14.0f, 0.0f).x + 16.0f;
    }
}

static void lcdDigit(Rectangle r, float t, int digit, bool dim)
{
    Color on = dim ? Fade(LCD_GLOW, 0.15f) : LCD_GLOW;
    BeginBlendMode(BLEND_ADDITIVE);
    Gui_SegmentDigit(Gui_Inset(r, -1.5f), t + 3.0f, digit, Fade(LCD_GLOW, dim ? 0.03f : 0.18f), BLANK, false);
    EndBlendMode();
    Gui_SegmentDigit(r, t, digit, on, Fade(LCD_GLOW, 0.07f), false);
}

static void badge(float x, float y, const char* text, bool lit)
{
    const GuiFonts* fonts = Gui_Fonts();
    Vector2 size = MeasureTextEx(fonts->uiSmall, text, 14.0f, 0.0f);
    Rectangle r = { x, y, size.x + 10.0f, 16.0f };
    DrawRectangleRec(r, lit ? Fade(LCD_GLOW, 0.22f) : Fade(LCD_GLOW, 0.05f));
    DrawRectangleLinesEx(r, 1.0f, Fade(LCD_GLOW, lit ? 0.8f : 0.2f));
    Gui_TextCentered(fonts->uiSmall, text, r, 14.0f, 0.0f, lit ? LCD_GLOW : Fade(LCD_GLOW, 0.3f));
}

static float bandLevel(const WatchView* view, int band, float speed)
{
    float t = view->time * speed;
    float v = 0.5f + 0.25f * sinf(t * (1.3f + 0.21f * band) + band * 1.7f) + 0.2f * sinf(t * (3.1f + 0.37f * band) + band);
    v *= 1.0f - band * 0.025f;
    return v < 0.05f ? 0.05f : (v > 1.0f ? 1.0f : v);
}

static void drawLcd(const WatchView* view)
{
    const GuiFonts* fonts = Gui_Fonts();
    insetPanel(LCD, LCD_BG);

    /* Time: HH:MM:SS in seven-segment */
    float dw = 30.0f, dh = 56.0f, t = 6.0f;
    float x = 28.0f, y = 72.0f;
    int digits[6] = { view->hours / 10, view->hours % 10, view->minutes / 10, view->minutes % 10, view->seconds / 10, view->seconds % 10 };
    bool secondsDim = view->mode == ALARM_MODE || view->mode == TIMESET_MODE;
    for (int i = 0; i < 6; i++) {
        bool blank = (i < 2 && Gui_BlinkOff(view, true)) || (i >= 2 && i < 4 && Gui_BlinkOff(view, false));
        lcdDigit((Rectangle) { x, y, dw, dh }, t, blank ? -1 : digits[i], i >= 4 && secondsDim);
        x += dw + 8.0f;
        if (i == 1 || i == 3) {
            bool colonOn = !(view->mode == CLOCK_MODE && view->tenths >= 5);
            Gui_SegmentColon((Rectangle) { x - 6.0f, y, 12.0f, dh }, 6.0f, colonOn ? LCD_GLOW : Fade(LCD_GLOW, 0.15f));
            x += 10.0f;
        }
    }
    lcdDigit((Rectangle) { x + 2.0f, y + dh - 26.0f, 14.0f, 26.0f }, 3.0f, view->tenths, view->mode != STOPWATCH_MODE);

    /* Telemetry column */
    float cx = 318.0f;
    char alarmText[16];
    snprintf(alarmText, sizeof(alarmText), "%02d:%02d", view->alarmHours, view->alarmMinutes);
    Gui_TextLeft(fonts->uiSmall, "ALM", (Vector2) { cx, 70.0f }, 14.0f, 0.0f, Fade(LCD_GLOW, 0.55f));
    Gui_TextLeft(fonts->uiSmall, alarmText, (Vector2) { cx + 32.0f, 70.0f }, 14.0f, 0.0f, LCD_GLOW);
    Gui_TextLeft(fonts->uiSmall, "MODE", (Vector2) { cx, 88.0f }, 14.0f, 0.0f, Fade(LCD_GLOW, 0.55f));
    Gui_TextLeft(fonts->uiSmall, Gui_ModeName(view->mode), (Vector2) { cx + 38.0f, 88.0f }, 14.0f, 0.0f, LCD_GLOW);

    badge(cx, 110.0f, "CLK", view->mode == CLOCK_MODE);
    badge(cx + 34.0f, 110.0f, "ALM", view->mode == ALARM_MODE);
    badge(cx + 68.0f, 110.0f, "STW", view->mode == STOPWATCH_MODE);
    badge(cx, 130.0f, "SET", view->mode == TIMESET_MODE);
    badge(cx + 34.0f, 130.0f, "RUN", view->stopwatchRunning);
    bool ringing = view->alarmState == ALARM_EXPIRED && fmodf(view->time, 0.5f) < 0.25f;
    badge(cx + 68.0f, 130.0f, view->alarmState == ALARM_EXPIRED ? "RING" : "ARM", view->alarmState == ALARM_ON || ringing);

    /* Spectrum analyzer */
    float speed = view->stopwatchRunning ? 3.0f : 1.0f;
    float bx = 28.0f, by = 196.0f, barW = 10.0f;
    for (int b = 0; b < 22; b++) {
        float level = bandLevel(view, b, speed);
        if (view->alarmState == ALARM_EXPIRED) {
            level = fminf(1.0f, level + 0.4f * fabsf(sinf(view->time * 12.0f)));
        }
        int cells = (int)(level * 12.0f);
        for (int c = 0; c < 12; c++) {
            Color color = c < cells ? ColorLerp(LCD_GLOW, WHITE, c / 24.0f) : Fade(LCD_GLOW, 0.06f);
            DrawRectangleRec((Rectangle) { bx + b * (barW + 3.0f), by - c * 4.0f, barW, 3.0f }, color);
        }
    }

    /* Dot-matrix marquee */
    Rectangle marquee = { 22.0f, 214.0f, 396.0f, 28.0f };
    DrawRectangleRec(marquee, Fade(BLACK, 0.35f));
    char text[160];
    snprintf(text, sizeof(text), "***  EMBEDDEDWATCH  -  %s MODE  -  ALARM %s %s  -  %s  ", Gui_ModeName(view->mode), alarmText, Gui_AlarmStateName(view->alarmState), Gui_Hint(view));
    Font dot = GetFontDefault();
    float width = MeasureTextEx(dot, text, 20.0f, 2.0f).x;
    float offset = fmodf(view->time * 50.0f, width);
    BeginScissorMode((int)marquee.x, (int)marquee.y, (int)marquee.width, (int)marquee.height);
    for (int k = 0; k < 3; k++) {
        DrawTextEx(dot, text, (Vector2) { floorf(marquee.x + 4.0f - offset + k * width), marquee.y + 4.0f }, 20.0f, 2.0f, LCD_GLOW);
    }
    EndScissorMode();

    /* Scanlines over the whole glass */
    for (float sy = LCD.y; sy < LCD.y + LCD.height; sy += 2.0f) {
        DrawRectangleRec((Rectangle) { LCD.x, sy, LCD.width, 1.0f }, Fade(BLACK, 0.22f));
    }
    DrawRectangleGradientV((int)LCD.x, (int)LCD.y, (int)LCD.width, 40, Fade(WHITE, 0.07f), Fade(WHITE, 0.0f));
}

static void capsule(Rectangle r, bool pressed, bool lit)
{
    float round = 1.0f;
    DrawRectangleRounded(r, round, 12, BEVEL_DARK);
    Rectangle face = Gui_Inset(r, 1.0f);
    Color top = pressed ? FRAME_BOTTOM : WHITE;
    Color bottom = pressed ? FRAME_TOP : FRAME_BOTTOM;
    if (lit) {
        top = pressed ? (Color) { 40, 100, 180, 255 } : (Color) { 150, 200, 255, 255 };
        bottom = pressed ? (Color) { 150, 200, 255, 255 } : (Color) { 40, 100, 180, 255 };
    }
    DrawRectangleRounded(face, round, 12, bottom);
    DrawRectangleRounded((Rectangle) { face.x, face.y, face.width, face.height * 0.55f }, round, 12, ColorLerp(top, bottom, 0.3f));
    DrawRectangleRounded((Rectangle) { face.x + 4.0f, face.y + 1.0f, face.width - 8.0f, face.height * 0.4f }, round, 12, Fade(WHITE, pressed ? 0.1f : 0.45f));
}

static void drawSeek(const WatchView* view)
{
    insetPanel(SEEK, (Color) { 40, 48, 64, 255 });
    float fraction = (view->seconds + view->tenths / 10.0f) / 60.0f;
    if (view->mode == ALARM_MODE || view->mode == TIMESET_MODE) {
        fraction = (view->hours * 60 + view->minutes) / 1440.0f;
    }
    DrawRectangleRec((Rectangle) { SEEK.x + 2.0f, SEEK.y + 3.0f, (SEEK.width - 4.0f) * fraction, 4.0f }, Fade(LCD_GLOW, 0.7f));
    Rectangle thumb = { SEEK.x + (SEEK.width - 34.0f) * fraction, SEEK.y - 4.0f, 34.0f, 18.0f };
    capsule(thumb, false, false);
    for (int i = -1; i <= 1; i++) {
        DrawLineEx((Vector2) { thumb.x + 17.0f + 4.0f * i, thumb.y + 5.0f }, (Vector2) { thumb.x + 17.0f + 4.0f * i, thumb.y + 13.0f }, 1.0f, BEVEL_DARK);
    }
}

static void deckGlyph(enum GuiButton button, Vector2 c, Color color, const WatchView* view)
{
    switch (button) {
    case GB_MINUS:
        DrawTriangle((Vector2) { c.x - 1.0f, c.y - 8.0f }, (Vector2) { c.x - 13.0f, c.y }, (Vector2) { c.x - 1.0f, c.y + 8.0f }, color);
        DrawTriangle((Vector2) { c.x + 11.0f, c.y - 8.0f }, (Vector2) { c.x - 1.0f, c.y }, (Vector2) { c.x + 11.0f, c.y + 8.0f }, color);
        break;
    case GB_PLUS:
        DrawTriangle((Vector2) { c.x - 11.0f, c.y - 8.0f }, (Vector2) { c.x - 11.0f, c.y + 8.0f }, (Vector2) { c.x + 1.0f, c.y }, color);
        DrawTriangle((Vector2) { c.x + 1.0f, c.y - 8.0f }, (Vector2) { c.x + 1.0f, c.y + 8.0f }, (Vector2) { c.x + 13.0f, c.y }, color);
        break;
    case GB_STOPWATCH:
        if (view->mode == STOPWATCH_MODE && view->stopwatchRunning) {
            DrawRectangleRec((Rectangle) { c.x - 8.0f, c.y - 8.0f, 5.0f, 16.0f }, color);
            DrawRectangleRec((Rectangle) { c.x + 3.0f, c.y - 8.0f, 5.0f, 16.0f }, color);
        } else {
            DrawTriangle((Vector2) { c.x - 6.0f, c.y - 9.0f }, (Vector2) { c.x - 6.0f, c.y + 9.0f }, (Vector2) { c.x + 9.0f, c.y }, color);
        }
        break;
    case GB_CLOCK:
        DrawRing(c, 8.0f, 10.0f, 0.0f, 360.0f, 24, color);
        DrawLineEx(c, (Vector2) { c.x, c.y - 6.0f }, 2.0f, color);
        DrawLineEx(c, (Vector2) { c.x + 5.0f, c.y }, 2.0f, color);
        break;
    case GB_ALARM:
        DrawCircleSector((Vector2) { c.x, c.y + 3.0f }, 9.0f, 180.0f, 360.0f, 16, color);
        DrawRectangleRec((Rectangle) { c.x - 11.0f, c.y + 2.0f, 22.0f, 3.0f }, color);
        DrawCircleV((Vector2) { c.x, c.y + 8.0f }, 2.5f, color);
        break;
    case GB_TIMESET:
        DrawRectangleRec((Rectangle) { c.x - 9.0f, c.y - 9.0f, 18.0f, 18.0f }, color);
        break;
    default:
        break;
    }
}

static void drawDeck(const WatchView* view, const GuiLayout* layout, const GuiInput* input)
{
    const GuiFonts* fonts = Gui_Fonts();
    const char* captions[6] = { "-", "CLOCK", "ALARM", "STOPW", "SET", "+" };

    for (int i = 0; i < 6; i++) {
        enum GuiButton id = DECK[i];
        Rectangle r = layout->buttons[id];
        Vector2 c = { r.x + r.width / 2.0f, r.y + r.height / 2.0f };
        bool pressed = input->pressed[id];
        bool active = (id == GB_CLOCK && view->mode == CLOCK_MODE)
            || (id == GB_ALARM && view->mode == ALARM_MODE)
            || (id == GB_STOPWATCH && view->mode == STOPWATCH_MODE)
            || (id == GB_TIMESET && view->mode == TIMESET_MODE);

        DrawCircleV((Vector2) { c.x + 1.0f, c.y + 2.0f }, 28.0f, Fade(BLACK, 0.35f));
        DrawCircleV(c, 28.0f, BEVEL_DARK);
        Color top = pressed ? FRAME_BOTTOM : WHITE;
        Color bottom = pressed ? WHITE : FRAME_BOTTOM;
        if (active) {
            top = pressed ? (Color) { 30, 80, 160, 255 } : (Color) { 170, 215, 255, 255 };
            bottom = pressed ? (Color) { 170, 215, 255, 255 } : (Color) { 30, 90, 180, 255 };
        }
        DrawCircleGradient((int)c.x, (int)(c.y - 8.0f), 34.0f, top, bottom);
        DrawRing(c, 27.0f, 29.0f, 0.0f, 360.0f, 48, BEVEL_DARK);
        DrawRing(c, 22.0f, 23.0f, 200.0f, 340.0f, 24, Fade(WHITE, 0.7f));
        if (input->hovered[id]) {
            DrawRing(c, 29.0f, 31.0f, 0.0f, 360.0f, 48, Fade(LCD_GLOW, 0.8f));
        }
        deckGlyph(id, pressed ? (Vector2) { c.x + 1.0f, c.y + 1.0f } : c, active ? WHITE : INK, view);

        Gui_TextCentered(fonts->uiSmall, captions[i], (Rectangle) { r.x - 6.0f, r.y + r.height + 4.0f, r.width + 12.0f, 14.0f }, 14.0f, 0.0f, INK);
    }
}

static void drawAlarmSlider(const WatchView* view)
{
    const GuiFonts* fonts = Gui_Fonts();
    Gui_TextLeft(fonts->uiSmall, "ALARM", (Vector2) { 20.0f, 386.0f }, 14.0f, 0.0f, INK);
    Rectangle track = { 70.0f, 391.0f, 250.0f, 6.0f };
    insetPanel(track, (Color) { 40, 48, 64, 255 });
    for (int i = 0; i <= 24; i += 3) {
        float tx = track.x + track.width * i / 24.0f;
        DrawLineEx((Vector2) { tx, track.y + 9.0f }, (Vector2) { tx, track.y + 13.0f }, 1.0f, BEVEL_DARK);
    }
    float fraction = (view->alarmHours * 60 + view->alarmMinutes) / 1440.0f;
    capsule((Rectangle) { track.x + (track.width - 22.0f) * fraction, track.y - 6.0f, 22.0f, 18.0f }, false, view->alarmState != ALARM_OFF);

    /* lightning toggle: lit while the alarm is armed or ringing */
    Rectangle bolt = { 336.0f, 382.0f, 30.0f, 22.0f };
    capsule(bolt, false, view->alarmState != ALARM_OFF);
    Vector2 b = { bolt.x + 15.0f, bolt.y + 11.0f };
    Color boltColor = view->alarmState != ALARM_OFF ? WHITE : INK;
    DrawTriangle((Vector2) { b.x + 2.0f, b.y - 8.0f }, (Vector2) { b.x - 5.0f, b.y + 1.0f }, (Vector2) { b.x + 1.0f, b.y + 1.0f }, boltColor);
    DrawTriangle((Vector2) { b.x - 1.0f, b.y - 1.0f }, (Vector2) { b.x - 2.0f, b.y + 8.0f }, (Vector2) { b.x + 5.0f, b.y - 1.0f }, boltColor);

    Gui_TextLeft(fonts->uiSmall, view->stopwatchRunning ? "RUN" : "STOP", (Vector2) { 378.0f, 386.0f }, 14.0f, 0.0f, INK);
}

static void drawEqualizer(const WatchView* view)
{
    const GuiFonts* fonts = Gui_Fonts();
    DrawRectangleRec(EQ, Fade(WHITE, 0.12f));
    bevel(EQ, false);

    Rectangle title = { EQ.x + 1.0f, EQ.y + 1.0f, EQ.width - 2.0f, 18.0f };
    DrawRectangleGradientV((int)title.x, (int)title.y, (int)title.width, (int)title.height, TITLE_TOP, TITLE_BOTTOM);
    Gui_TextCentered(fonts->uiSmall, "EQUALIZER", title, 14.0f, 2.0f, WHITE);

    capsule((Rectangle) { EQ.x + 10.0f, EQ.y + 26.0f, 38.0f, 18.0f }, false, true);
    Gui_TextCentered(fonts->uiSmall, "ON", (Rectangle) { EQ.x + 10.0f, EQ.y + 26.0f, 38.0f, 18.0f }, 14.0f, 0.0f, WHITE);
    capsule((Rectangle) { EQ.x + 54.0f, EQ.y + 26.0f, 50.0f, 18.0f }, false, false);
    Gui_TextCentered(fonts->uiSmall, "AUTO", (Rectangle) { EQ.x + 54.0f, EQ.y + 26.0f, 50.0f, 18.0f }, 14.0f, 0.0f, INK);
    capsule((Rectangle) { EQ.x + EQ.width - 80.0f, EQ.y + 26.0f, 70.0f, 18.0f }, false, false);
    Gui_TextCentered(fonts->uiSmall, "PRESETS", (Rectangle) { EQ.x + EQ.width - 80.0f, EQ.y + 26.0f, 70.0f, 18.0f }, 14.0f, 0.0f, INK);

    /* Gain curve graph */
    Rectangle graph = { EQ.x + 112.0f, EQ.y + 24.0f, 210.0f, 22.0f };
    insetPanel(graph, LCD_BG);
    DrawLineEx((Vector2) { graph.x, graph.y + graph.height / 2.0f }, (Vector2) { graph.x + graph.width, graph.y + graph.height / 2.0f }, 1.0f, Fade(LCD_GLOW, 0.25f));

    static const char* const bands[11] = { "PRE", "60", "170", "310", "600", "1K", "3K", "6K", "12K", "14K", "16K" };
    Vector2 prev = { 0 };
    for (int i = 0; i < 11; i++) {
        float x = EQ.x + 24.0f + i * 33.0f;
        float top = EQ.y + 60.0f, height = 84.0f;
        float level = i == 0 ? 0.5f : bandLevel(view, i * 2, 0.35f);
        insetPanel((Rectangle) { x - 2.0f, top, 4.0f, height }, (Color) { 40, 48, 64, 255 });
        float ky = top + (1.0f - level) * (height - 12.0f);
        capsule((Rectangle) { x - 9.0f, ky, 18.0f, 12.0f }, false, i == 0);
        Gui_TextCentered(fonts->uiSmall, bands[i], (Rectangle) { x - 16.0f, top + height + 4.0f, 32.0f, 14.0f }, 14.0f, 0.0f, INK);

        if (i > 0) {
            Vector2 p = { graph.x + 6.0f + (i - 1) * (graph.width - 12.0f) / 9.0f, graph.y + 3.0f + (1.0f - level) * (graph.height - 6.0f) };
            if (i > 1) {
                DrawLineEx(prev, p, 1.5f, LCD_GLOW);
            }
            prev = p;
        }
    }
    Gui_TextLeft(fonts->uiSmall, "+12", (Vector2) { EQ.x + EQ.width - 30.0f, EQ.y + 58.0f }, 14.0f, 0.0f, INK);
    Gui_TextLeft(fonts->uiSmall, "0", (Vector2) { EQ.x + EQ.width - 22.0f, EQ.y + 94.0f }, 14.0f, 0.0f, INK);
    Gui_TextLeft(fonts->uiSmall, "-12", (Vector2) { EQ.x + EQ.width - 30.0f, EQ.y + 130.0f }, 14.0f, 0.0f, INK);
}

static void tab(Rectangle r, const char* text, bool selected, bool hovered)
{
    const GuiFonts* fonts = Gui_Fonts();
    Color top = selected ? WHITE : FRAME_TOP;
    Color bottom = selected ? FRAME_TOP : FRAME_BOTTOM;
    if (hovered) {
        top = (Color) { 190, 225, 255, 255 };
    }
    DrawRectangleGradientV((int)r.x, (int)r.y, (int)r.width, (int)r.height, top, bottom);
    bevel(r, false);
    Gui_TextCentered(fonts->uiSmall, text, r, 14.0f, 0.0f, INK);
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
    drawSeek(view);
    drawDeck(view, layout, input);
    drawAlarmSlider(view);
    drawEqualizer(view);

    tab((Rectangle) { 14.0f, 604.0f, 130.0f, 26.0f }, "EQUALIZER", true, false);
    tab((Rectangle) { 150.0f, 604.0f, 136.0f, 26.0f }, "OPTIONS", false, false);
    tab(layout->buttons[GB_THEME], Gui_ButtonLabel(GB_THEME, view), input->menuOpen || input->pressed[GB_THEME], input->hovered[GB_THEME]);

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
