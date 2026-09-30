#include "Gui.h"

#include <math.h>
#include <stddef.h>

static GuiFonts fonts;
static bool fontsLoaded = false;

static const char* const LIGHT_FONTS[] = {
    "C:/Windows/Fonts/segoeuil.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans-ExtraLight.ttf",
    "/System/Library/Fonts/Supplemental/Arial.ttf",
    NULL
};

static const char* const UI_FONTS[] = {
    "C:/Windows/Fonts/segoeui.ttf",
    "C:/Windows/Fonts/tahoma.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/System/Library/Fonts/Supplemental/Arial.ttf",
    NULL
};

static const char* const BOLD_FONTS[] = {
    "C:/Windows/Fonts/tahomabd.ttf",
    "C:/Windows/Fonts/segoeuib.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
    "/System/Library/Fonts/Supplemental/Arial Bold.ttf",
    NULL
};

static const char* const SMALL_FONTS[] = {
    "C:/Windows/Fonts/tahoma.ttf",
    "C:/Windows/Fonts/segoeui.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/System/Library/Fonts/Supplemental/Arial.ttf",
    NULL
};

static Font loadFirst(const char* const* candidates, int size)
{
    for (int i = 0; candidates[i] != NULL; i++) {
        if (FileExists(candidates[i])) {
            Font font = LoadFontEx(candidates[i], size, NULL, 0);
            if (font.texture.id != 0) {
                GenTextureMipmaps(&font.texture);
                SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);
                return font;
            }
        }
    }
    return GetFontDefault();
}

static void unloadIfCustom(Font font)
{
    if (font.texture.id != GetFontDefault().texture.id) {
        UnloadFont(font);
    }
}

void Gui_LoadFonts(void)
{
    fonts.light = loadFirst(LIGHT_FONTS, 128);
    fonts.ui = loadFirst(UI_FONTS, 48);
    fonts.uiBold = loadFirst(BOLD_FONTS, 48);
    fonts.uiSmall = loadFirst(SMALL_FONTS, 14);
    SetTextureFilter(fonts.uiSmall.texture, TEXTURE_FILTER_POINT);
    fontsLoaded = true;
}

void Gui_UnloadFonts(void)
{
    if (!fontsLoaded) {
        return;
    }
    unloadIfCustom(fonts.light);
    unloadIfCustom(fonts.ui);
    unloadIfCustom(fonts.uiBold);
    unloadIfCustom(fonts.uiSmall);
    fontsLoaded = false;
}

const GuiFonts* Gui_Fonts(void)
{
    return &fonts;
}

const char* Gui_ButtonLabel(enum GuiButton button, const WatchView* view)
{
    switch (button) {
    case GB_CLOCK:
        return "CLOCK";
    case GB_ALARM:
        return view->mode == ALARM_MODE ? "SET ALARM" : "ALARM";
    case GB_STOPWATCH:
        return "STOPWATCH";
    case GB_TIMESET:
        return "SET TIME";
    case GB_PLUS:
        return "+";
    case GB_MINUS:
        return "-";
    case GB_THEME:
        return "Select theme";
    default:
        return "";
    }
}

bool Gui_ButtonAvailable(enum GuiButton button, enum EWatchMode mode)
{
    return !(mode == CLOCK_MODE && (button == GB_PLUS || button == GB_MINUS));
}

const char* Gui_ModeName(enum EWatchMode mode)
{
    switch (mode) {
    case CLOCK_MODE:
        return "CLOCK";
    case ALARM_MODE:
        return "ALARM";
    case STOPWATCH_MODE:
        return "STOPWATCH";
    case TIMESET_MODE:
        return "SET TIME";
    default:
        return "";
    }
}

const char* Gui_AlarmStateName(enum AlarmState state)
{
    switch (state) {
    case ALARM_ON:
        return "ON";
    case ALARM_EXPIRED:
        return "RING";
    default:
        return "OFF";
    }
}

const char* Gui_Hint(const WatchView* view)
{
    if (view->alarmState == ALARM_EXPIRED && view->mode != ALARM_MODE) {
        return "ALARM! Press ALARM twice";
    }

    switch (view->mode) {
    case CLOCK_MODE:
        return "SET TIME adjusts the clock";
    case ALARM_MODE:
        if (view->alarmState == ALARM_EXPIRED) {
            return "Press SET ALARM to silence";
        }
        return "SET ALARM: hrs, mins, arm";
    case STOPWATCH_MODE:
        return "Play / pause    Reset";
    case TIMESET_MODE:
        return "SET TIME: hrs/mins  +/- adjust";
    default:
        return "";
    }
}

bool Gui_BlinkOff(const WatchView* view, bool hoursField)
{
    bool editing = view->mode == TIMESET_MODE || (view->mode == ALARM_MODE && view->alarmEditing);
    if (!editing || view->editingHours != hoursField) {
        return false;
    }
    return fmodf(view->time, 1.0f) > 0.6f;
}

float Gui_Roundness(Rectangle rect, float radius)
{
    float side = rect.width < rect.height ? rect.width : rect.height;
    if (side <= 0.0f) {
        return 0.0f;
    }
    float roundness = (2.0f * radius) / side;
    return roundness > 1.0f ? 1.0f : roundness;
}

Rectangle Gui_Inset(Rectangle rect, float amount)
{
    return (Rectangle) { rect.x + amount, rect.y + amount, rect.width - 2.0f * amount, rect.height - 2.0f * amount };
}

Rectangle Gui_Scale(Rectangle rect, float scale)
{
    return (Rectangle) { rect.x * scale, rect.y * scale, rect.width * scale, rect.height * scale };
}

void Gui_TextCentered(Font font, const char* text, Rectangle rect, float size, float spacing, Color color)
{
    Vector2 extent = MeasureTextEx(font, text, size, spacing);
    Vector2 pos = {
        floorf(rect.x + (rect.width - extent.x) / 2.0f),
        floorf(rect.y + (rect.height - extent.y) / 2.0f)
    };
    DrawTextEx(font, text, pos, size, spacing, color);
}

void Gui_TextLeft(Font font, const char* text, Vector2 pos, float size, float spacing, Color color)
{
    DrawTextEx(font, text, (Vector2) { floorf(pos.x), floorf(pos.y) }, size, spacing, color);
}

/* Segment bits: a b c d e f g (top, top-right, bottom-right, bottom, bottom-left, top-left, middle) */
static const unsigned char SEGMENTS[10] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F };

void Gui_SegmentDigit(Rectangle r, float t, int digit, Color on, Color off, bool rounded)
{
    float half = (r.height - 3.0f * t) / 2.0f;
    float gap = t * 0.15f;
    Rectangle seg[7] = {
        { r.x + t, r.y, r.width - 2.0f * t, t },
        { r.x + r.width - t, r.y + t, t, half },
        { r.x + r.width - t, r.y + 2.0f * t + half, t, half },
        { r.x + t, r.y + 2.0f * t + 2.0f * half, r.width - 2.0f * t, t },
        { r.x, r.y + 2.0f * t + half, t, half },
        { r.x, r.y + t, t, half },
        { r.x + t, r.y + t + half, r.width - 2.0f * t, t },
    };
    unsigned char bits = (digit >= 0 && digit <= 9) ? SEGMENTS[digit] : 0;

    for (int i = 0; i < 7; i++) {
        Rectangle s = seg[i];
        if (s.width > s.height) {
            s.x += gap;
            s.width -= 2.0f * gap;
        } else {
            s.y += gap;
            s.height -= 2.0f * gap;
        }
        Color c = (bits & (1u << i)) ? on : off;
        if (c.a == 0) {
            continue;
        }
        if (rounded) {
            DrawRectangleRounded(s, 1.0f, 6, c);
        } else {
            DrawRectangleRec(s, c);
        }
    }
}

/* raylib only fills counter-clockwise triangles; accept either winding. */
static void fillTriangle(Vector2 a, Vector2 b, Vector2 c, Color color)
{
    float cross = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    if (cross < 0.0f) {
        DrawTriangle(a, b, c, color);
    } else {
        DrawTriangle(a, c, b, color);
    }
}

static void iconReset(Vector2 c, float size, float t, Color color)
{
    /* Arc drawn clockwise from just right of the top, round to the upper left;
       the arrowhead at its start points back (counter-clockwise) into the gap. */
    const float startDeg = 285.0f;
    const float endDeg = 205.0f + 360.0f;
    float ring = t * 0.75f;
    DrawRing(c, size - ring, size, startDeg, endDeg, 36, color);

    float a = startDeg * DEG2RAD;
    float mid = size - ring / 2.0f;
    Vector2 p = { c.x + mid * cosf(a), c.y + mid * sinf(a) };
    Vector2 radial = { cosf(a), sinf(a) };
    Vector2 back = { sinf(a), -cosf(a) };
    float w = t * 1.2f;
    float h = t * 1.8f;
    fillTriangle((Vector2) { p.x + back.x * h, p.y + back.y * h },
        (Vector2) { p.x + radial.x * w, p.y + radial.y * w },
        (Vector2) { p.x - radial.x * w, p.y - radial.y * w },
        color);
}

void Gui_AdjustIcon(enum GuiButton button, const WatchView* view, Vector2 c, float size, float t, Color color)
{
    if (view->mode == STOPWATCH_MODE) {
        if (button == GB_MINUS) {
            iconReset(c, size, t, color);
        } else if (view->stopwatchRunning) {
            float bar = size * 0.6f;
            DrawRectangleRec((Rectangle) { c.x - size * 0.75f, c.y - size, bar, 2.0f * size }, color);
            DrawRectangleRec((Rectangle) { c.x + size * 0.75f - bar, c.y - size, bar, 2.0f * size }, color);
        } else {
            fillTriangle((Vector2) { c.x - size * 0.7f, c.y - size },
                (Vector2) { c.x - size * 0.7f, c.y + size },
                (Vector2) { c.x + size, c.y },
                color);
        }
        return;
    }

    DrawRectangleRec((Rectangle) { c.x - size, c.y - t / 2.0f, 2.0f * size, t }, color);
    if (button == GB_PLUS) {
        DrawRectangleRec((Rectangle) { c.x - t / 2.0f, c.y - size, t, 2.0f * size }, color);
    }
}

void Gui_SegmentColon(Rectangle r, float t, Color color)
{
    float cx = r.x + (r.width - t) / 2.0f;
    DrawRectangleRec((Rectangle) { cx, r.y + r.height * 0.28f, t, t }, color);
    DrawRectangleRec((Rectangle) { cx, r.y + r.height * 0.68f, t, t }, color);
}
