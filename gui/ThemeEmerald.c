/*
 * Emerald theme: GBA storage-box interface (skills/Emerald.Graphics.md).
 * Everything is drawn on a 220x320 canvas snapped to an 8px tile grid and
 * upscaled 2x with nearest-neighbour filtering for crisp pixels.
 */
#include "Gui.h"

#include <math.h>
#include <stdio.h>

#define CANVAS_W (GUI_SCREEN_WIDTH / 2)
#define CANVAS_H (GUI_SCREEN_HEIGHT / 2)
#define PIXEL_SCALE 2.0f

static const Color PRIMARY = { 56, 152, 136, 255 };
static const Color DARK = { 24, 80, 72, 255 };
static const Color CREAM = { 248, 248, 224, 255 };
static const Color BORDER = { 40, 40, 40, 255 };
static const Color LCD_BLUE = { 120, 168, 192, 255 };
static const Color KBD_BLUE = { 128, 192, 216, 255 };
static const Color KBD_ORANGE = { 216, 152, 120, 255 };
static const Color KBD_GREEN = { 144, 208, 136, 255 };
static const Color METAL = { 184, 184, 192, 255 };
static const Color METAL_DARK = { 112, 112, 128, 255 };
static const Color SHADOW = { 160, 160, 136, 255 };

static RenderTexture2D canvas;
static bool canvasLoaded = false;

/* Canvas-space rectangles; the layout reports them scaled to the screen. */
static const Rectangle BANNER = { 8, 8, 204, 32 };
static const Rectangle BOX = { 8, 48, 204, 104 };
static const Rectangle HINT = { 8, 266, 204, 20 };

static Rectangle canvasButton(enum GuiButton button)
{
    switch (button) {
    case GB_CLOCK:
        return (Rectangle) { 8, 160, 100, 24 };
    case GB_ALARM:
        return (Rectangle) { 112, 160, 100, 24 };
    case GB_STOPWATCH:
        return (Rectangle) { 8, 190, 100, 24 };
    case GB_TIMESET:
        return (Rectangle) { 112, 190, 100, 24 };
    case GB_PLUS:
        return (Rectangle) { 8, 224, 100, 32 };
    case GB_MINUS:
        return (Rectangle) { 112, 224, 100, 32 };
    case GB_THEME:
        return (Rectangle) { 56, 294, 108, 20 };
    default:
        return (Rectangle) { 0 };
    }
}

static Rectangle canvasMenuItem(int index, int count)
{
    float top = 294.0f - 8.0f - 18.0f * count - 8.0f;
    return (Rectangle) { 68, top + 8.0f + 18.0f * index, 116, 18 };
}

static void layout(GuiLayout* out, int themeCount, enum EWatchMode mode)
{
    (void)mode;
    for (int i = 0; i < GB_COUNT; i++) {
        out->buttons[i] = Gui_Scale(canvasButton((enum GuiButton)i), PIXEL_SCALE);
    }
    for (int i = 0; i < themeCount && i < GUI_MAX_THEMES; i++) {
        out->menuItems[i] = Gui_Scale(canvasMenuItem(i, themeCount), PIXEL_SCALE);
    }
}

static void pixelText(const char* text, float x, float y, int scale, Color color)
{
    DrawTextEx(GetFontDefault(), text, (Vector2) { floorf(x), floorf(y) }, 10.0f * scale, (float)scale, color);
}

/* GBA-style text: bi-colour drop shadow one pixel down/right. */
static void shadowText(const char* text, float x, float y, int scale, Color color, Color shadow)
{
    pixelText(text, x + 1.0f, y + 1.0f, scale, shadow);
    pixelText(text, x + 1.0f, y, scale, shadow);
    pixelText(text, x, y + 1.0f, scale, shadow);
    pixelText(text, x, y, scale, color);
}

static float pixelTextWidth(const char* text, int scale)
{
    return MeasureTextEx(GetFontDefault(), text, 10.0f * scale, (float)scale).x;
}

static void centeredShadowText(const char* text, Rectangle r, int scale, Color color, Color shadow)
{
    float w = pixelTextWidth(text, scale);
    shadowText(text, r.x + floorf((r.width - w) / 2.0f), r.y + floorf((r.height - 7.0f * scale) / 2.0f), scale, color, shadow);
}

/* Rounded pixel box: 1px corner notch, dark outline, optional inner rim. */
static void pixelBox(Rectangle r, Color outline, Color fill, Color rim)
{
    int x = (int)r.x, y = (int)r.y, w = (int)r.width, h = (int)r.height;
    DrawRectangle(x + 1, y, w - 2, h, outline);
    DrawRectangle(x, y + 1, w, h - 2, outline);
    DrawRectangle(x + 1, y + 1, w - 2, h - 2, rim);
    DrawRectangle(x + 2, y + 2, w - 4, h - 4, fill);
}

static void tiledPattern(Rectangle r, Color base, Color dot, int kind)
{
    DrawRectangleRec(r, base);
    for (int ty = (int)r.y; ty < (int)(r.y + r.height); ty += 8) {
        for (int tx = (int)r.x; tx < (int)(r.x + r.width); tx += 8) {
            int col = (tx - (int)r.x) / 8, row = (ty - (int)r.y) / 8;
            if (kind == 0) {
                /* polka dot */
                if ((col + row) % 2 == 0) {
                    DrawRectangle(tx + 3, ty + 3, 2, 2, dot);
                }
            } else {
                /* checker */
                if ((col + row) % 2 == 0) {
                    DrawRectangle(tx, ty, 8, 8, dot);
                }
            }
        }
    }
}

static float clamp01(float v)
{
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

/* Height of a body on a 24h arc peaking at `noon`: 1 at the top, 0 on the horizon. */
static float elevation(float hour, float noon)
{
    return cosf((hour - noon) * PI / 12.0f);
}

/* Position on the arc across the right of the banner, rising left and setting right. */
static Vector2 arcPosition(Rectangle sky, float hour, float noon, float horizonY)
{
    float t = fmodf(hour - (noon - 6.0f) + 24.0f, 24.0f) / 12.0f;
    float x = sky.x + 96.0f + t * (sky.width - 106.0f);
    float y = horizonY - elevation(hour, noon) * (horizonY - sky.y - 6.0f);
    return (Vector2) { floorf(x), floorf(y) };
}

static void drawBanner(const WatchView* view)
{
    Rectangle b = BANNER;
    pixelBox(b, BORDER, PRIMARY, CREAM);
    Rectangle inner = Gui_Inset(b, 2.0f);
    BeginScissorMode((int)inner.x, (int)inner.y, (int)inner.width, (int)inner.height);

    /* Static landscape lit for the clock's time of day. */
    float hour = view->clockHours + view->clockMinutes / 60.0f;
    float sun = elevation(hour, 12.0f);
    float daylight = clamp01((sun + 0.3f) / 0.6f);
    float twilight = clamp01(1.0f - fabsf(sun) / 0.45f);

    const Color night = { 32, 40, 88, 255 };
    const Color day = { 144, 200, 232, 255 };
    const Color dusk = { 240, 152, 96, 255 };
    Color skyTop = ColorLerp(night, day, daylight);
    Color skyBottom = ColorLerp(skyTop, dusk, twilight * 0.8f);
    DrawRectangleGradientV((int)inner.x, (int)inner.y, (int)inner.width, (int)inner.height, skyTop, skyBottom);

    /* Stars fade in after sunset */
    static const unsigned char STARS[][2] = { { 104, 3 }, { 120, 9 }, { 137, 4 }, { 151, 12 }, { 166, 6 }, { 182, 3 }, { 194, 10 }, { 112, 15 }, { 175, 14 } };
    float starAlpha = 1.0f - clamp01(daylight * 2.0f);
    for (size_t i = 0; starAlpha > 0.0f && i < sizeof(STARS) / sizeof(STARS[0]); i++) {
        DrawRectangle((int)inner.x + STARS[i][0], (int)inner.y + STARS[i][1], 1, 1, Fade(CREAM, starAlpha));
    }

    float horizonY = inner.y + inner.height - 4.0f;
    if (sun > -0.1f) {
        Vector2 p = arcPosition(inner, hour, 12.0f, horizonY);
        Color sunColor = ColorLerp(CREAM, (Color) { 248, 184, 88, 255 }, twilight);
        DrawCircle((int)p.x, (int)p.y, 4.0f, sunColor);
    }
    if (elevation(hour, 0.0f) > -0.1f) {
        Vector2 p = arcPosition(inner, hour, 0.0f, horizonY);
        DrawCircle((int)p.x, (int)p.y, 3.0f, CREAM);
        DrawCircle((int)p.x + 2, (int)p.y - 1, 3.0f, skyTop);
    }

    const Color shade = { 24, 32, 64, 255 };
    Color hill = ColorLerp(KBD_GREEN, shade, (1.0f - daylight) * 0.6f);
    Color hillFar = ColorLerp(PRIMARY, shade, (1.0f - daylight) * 0.6f);
    for (int x = 0; x < (int)inner.width; x++) {
        int far = (int)(6 + 4 * sinf(x * 0.09f + 1.0f));
        int near = (int)(3 + 3 * sinf(x * 0.05f + 3.0f));
        DrawRectangle((int)inner.x + x, (int)(inner.y + inner.height) - far - 6, 1, far + 6, hillFar);
        DrawRectangle((int)inner.x + x, (int)(inner.y + inner.height) - near - 2, 1, near + 2, hill);
    }
    EndScissorMode();

    const char* title = Gui_ModeName(view->mode);
    float w = pixelTextWidth(title, 1) + 12.0f;
    Rectangle tag = { b.x + 8.0f, b.y + 8.0f, floorf(w), 16.0f };
    pixelBox(tag, BORDER, CREAM, WHITE);
    pixelText(title, tag.x + 6.0f, tag.y + 4.0f, 1, BORDER);
}

/* The default font is proportional; centre each digit in a fixed cell. */
static void digitCells(const char* digits, float x, float y, float cell, int scale, Color color, Color shadow)
{
    for (int i = 0; digits[i] != '\0'; i++) {
        char glyph[2] = { digits[i], '\0' };
        centeredShadowText(glyph, (Rectangle) { x + i * cell, y, cell, 7.0f * scale }, scale, color, shadow);
    }
}

static void drawBox(const WatchView* view)
{
    /* Wallpaper container with bi-colour frame */
    DrawRectangleRec(BOX, BORDER);
    DrawRectangleRec(Gui_Inset(BOX, 1.0f), DARK);
    DrawRectangleRec(Gui_Inset(BOX, 3.0f), PRIMARY);
    tiledPattern(Gui_Inset(BOX, 4.0f), PRIMARY, (Color) { 72, 168, 152, 255 }, 0);

    /* Data window: metallic card holding a blue scanline screen */
    Rectangle card = { BOX.x + 12.0f, BOX.y + 12.0f, BOX.width - 24.0f, 64.0f };
    DrawRectangle((int)card.x + 2, (int)card.y + 2, (int)card.width, (int)card.height, DARK);
    pixelBox(card, BORDER, METAL, WHITE);
    DrawRectangle((int)card.x + 2, (int)(card.y + card.height) - 4, (int)card.width - 4, 2, METAL_DARK);

    Rectangle screen = { card.x + 6.0f, card.y + 6.0f, card.width - 12.0f, card.height - 14.0f };
    DrawRectangleRec(screen, BORDER);
    Rectangle glass = Gui_Inset(screen, 1.0f);
    Color screenColor = LCD_BLUE;
    if (view->alarmState == ALARM_EXPIRED && fmodf(view->time, 0.5f) < 0.25f) {
        screenColor = KBD_ORANGE;
    }
    DrawRectangleRec(glass, screenColor);

    char hh[4], mm[4], tail[8];
    snprintf(hh, sizeof(hh), "%02d", view->hours);
    snprintf(mm, sizeof(mm), "%02d", view->minutes);
    if (view->mode == STOPWATCH_MODE) {
        snprintf(tail, sizeof(tail), "%02d.%d", view->seconds, view->tenths);
    } else if (view->mode == CLOCK_MODE) {
        snprintf(tail, sizeof(tail), "%02d", view->seconds);
    } else {
        snprintf(tail, sizeof(tail), "--");
    }

    int big = 3;
    float cell = pixelTextWidth("0", big) + big;
    float colonW = pixelTextWidth(":", big) + 2.0f * big;
    float wBig = 4.0f * cell + colonW;
    float wTail = pixelTextWidth(tail, 1);
    float x = glass.x + floorf((glass.width - wBig - 4.0f - wTail) / 2.0f);
    float y = glass.y + 9.0f;
    Color ink = CREAM, inkShadow = DARK;

    if (!Gui_BlinkOff(view, true)) {
        digitCells(hh, x, y, cell, big, ink, inkShadow);
    }
    bool colonOn = !(view->mode == CLOCK_MODE && view->tenths >= 5);
    if (colonOn) {
        centeredShadowText(":", (Rectangle) { x + 2.0f * cell, y, colonW, 7.0f * big }, big, ink, inkShadow);
    }
    if (!Gui_BlinkOff(view, false)) {
        digitCells(mm, x + 2.0f * cell + colonW, y, cell, big, ink, inkShadow);
    }
    shadowText(tail, x + wBig + 4.0f, y + 14.0f, 1, ink, inkShadow);

    /* Scanlines */
    for (int sy = (int)glass.y + 1; sy < (int)(glass.y + glass.height); sy += 2) {
        DrawRectangle((int)glass.x, sy, (int)glass.width, 1, Fade(DARK, 0.18f));
    }

    /* Status strip */
    char status[32];
    snprintf(status, sizeof(status), "ALARM %02d:%02d %s", view->alarmHours, view->alarmMinutes, Gui_AlarmStateName(view->alarmState));
    Rectangle strip = { BOX.x + 12.0f, BOX.y + BOX.height - 22.0f, BOX.width - 24.0f, 14.0f };
    pixelBox(strip, BORDER, CREAM, WHITE);
    pixelText(status, strip.x + 6.0f, strip.y + 4.0f, 1, BORDER);
    if (view->mode == STOPWATCH_MODE) {
        const char* run = view->stopwatchRunning ? "RUN" : "STOP";
        pixelText(run, strip.x + strip.width - 6.0f - pixelTextWidth(run, 1), strip.y + 4.0f, 1, view->stopwatchRunning ? PRIMARY : KBD_ORANGE);
    }
}

static bool modeActive(enum GuiButton button, const WatchView* view)
{
    return (button == GB_CLOCK && view->mode == CLOCK_MODE)
        || (button == GB_ALARM && view->mode == ALARM_MODE)
        || (button == GB_STOPWATCH && view->mode == STOPWATCH_MODE)
        || (button == GB_TIMESET && view->mode == TIMESET_MODE);
}

static const char* const HAND[9] = {
    ".XXXX........",
    "XWWWWXXXXXXX.",
    "XWWWWWWWWWWWX",
    "XYWWWXXXXXXX.",
    "XYWWWWWWX....",
    "XYWWWXXX.....",
    "XYWWWWWWX....",
    ".XYWWWXX.....",
    "..XXXX.......",
};

/* White/yellow glove cursor pointing right (or left when mirrored). */
static void drawHand(float x, float y, bool mirror)
{
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 13; col++) {
            char p = HAND[row][col];
            if (p == '.') {
                continue;
            }
            Color c = p == 'X' ? BORDER : (p == 'Y' ? (Color) { 248, 216, 88, 255 } : WHITE);
            int px = mirror ? (int)x + 12 - col : (int)x + col;
            DrawRectangle(px, (int)y + row, 1, 1, c);
        }
    }
}

/* Party slot: teal container with a tiny pixel badge. */
static void drawSlot(enum GuiButton button, const WatchView* view, const GuiInput* input)
{
    Rectangle r = canvasButton(button);
    bool active = modeActive(button, view);
    bool pressed = input->pressed[button];
    if (pressed) {
        r.y += 1.0f;
    } else {
        DrawRectangle((int)r.x + 1, (int)r.y + 2, (int)r.width, (int)r.height, DARK);
    }
    Color fill = active ? (Color) { 88, 192, 168, 255 } : PRIMARY;
    if (input->hovered[button] && !active) {
        fill = (Color) { 72, 176, 152, 255 };
    }
    pixelBox(r, BORDER, fill, active ? CREAM : (Color) { 96, 184, 168, 255 });

    if (active) {
        /* pointing hand takes the badge's place on the active slot */
        float bob = (fmodf(view->time, 0.6f) < 0.3f) ? 0.0f : 1.0f;
        drawHand(r.x + 3.0f + bob, r.y + 7.0f, false);
    } else {
        Rectangle ball = { r.x + 5.0f, r.y + 7.0f, 10.0f, 10.0f };
        pixelBox(ball, BORDER, METAL, WHITE);
        DrawRectangle((int)ball.x + 1, (int)ball.y + 5, 8, 1, BORDER);
        DrawRectangle((int)ball.x + 4, (int)ball.y + 4, 2, 2, WHITE);
    }

    shadowText(Gui_ButtonLabel(button, view), r.x + 20.0f, r.y + 9.0f, 1, CREAM, DARK);
}

/* Keyboard-matrix key with inner highlight and hard pixel shadow. */
static void drawKey(enum GuiButton button, const WatchView* view, Color color, bool hovered, bool pressed, const char* caption)
{
    Rectangle r = canvasButton(button);
    if (pressed) {
        r.x += 1.0f;
        r.y += 2.0f;
    } else {
        DrawRectangle((int)r.x + 1, (int)r.y + 2, (int)r.width, (int)r.height, SHADOW);
    }
    Color face = hovered ? ColorBrightness(color, 0.15f) : color;
    pixelBox(r, BORDER, face, pressed ? face : ColorBrightness(color, 0.45f));
    DrawRectangle((int)r.x + 2, (int)(r.y + r.height) - 4, (int)r.width - 4, 2, ColorBrightness(color, -0.25f));
    Vector2 c = { r.x + 20.0f, floorf(r.y + r.height / 2.0f) - 1.0f };
    Gui_AdjustIcon(button, view, (Vector2) { c.x + 1.0f, c.y + 1.0f }, 7.0f, 3.0f, BORDER);
    Gui_AdjustIcon(button, view, c, 7.0f, 3.0f, WHITE);
    centeredShadowText(caption, (Rectangle) { r.x + 32.0f, r.y - 1.0f, r.width - 40.0f, r.height }, 1, WHITE, BORDER);
}

static void drawKeys(const WatchView* view, const GuiInput* input)
{
    const char* plusCaption = view->mode == STOPWATCH_MODE ? (view->stopwatchRunning ? "STOP" : "START") : "UP";
    const char* minusCaption = view->mode == STOPWATCH_MODE ? "RESET" : "DOWN";
    drawKey(GB_PLUS, view, KBD_BLUE, input->hovered[GB_PLUS], input->pressed[GB_PLUS], plusCaption);
    drawKey(GB_MINUS, view, KBD_ORANGE, input->hovered[GB_MINUS], input->pressed[GB_MINUS], minusCaption);
}

static void drawHint(const WatchView* view)
{
    /* Cream lined-paper text box with double-line border */
    pixelBox(HINT, BORDER, CREAM, CREAM);
    DrawRectangleLinesEx(Gui_Inset(HINT, 3.0f), 1.0f, PRIMARY);
    for (int y = (int)HINT.y + 7; y < (int)(HINT.y + HINT.height) - 4; y += 4) {
        DrawRectangle((int)HINT.x + 5, y, (int)HINT.width - 10, 1, (Color) { 232, 232, 200, 255 });
    }
    centeredShadowText(Gui_Hint(view), HINT, 1, BORDER, SHADOW);
}

static void drawThemeButton(const WatchView* view, const GuiInput* input)
{
    Rectangle r = canvasButton(GB_THEME);
    bool pressed = input->pressed[GB_THEME] || input->menuOpen;
    if (pressed) {
        r.y += 1.0f;
    } else {
        DrawRectangleRounded((Rectangle) { r.x + 1.0f, r.y + 2.0f, r.width, r.height }, 1.0f, 8, SHADOW);
    }
    Color face = input->hovered[GB_THEME] ? (Color) { 120, 120, 136, 255 } : METAL_DARK;
    DrawRectangleRounded(r, 1.0f, 8, BORDER);
    DrawRectangleRounded(Gui_Inset(r, 1.0f), 1.0f, 8, face);
    DrawRectangle((int)r.x + 8, (int)r.y + 3, (int)r.width - 16, 1, Fade(WHITE, pressed ? 0.2f : 0.6f));
    centeredShadowText(Gui_ButtonLabel(GB_THEME, view), r, 1, WHITE, BORDER);
}

static void drawMenu(const GuiInput* input, float time)
{
    int count = input->themeCount;
    Rectangle first = canvasMenuItem(0, count);
    Rectangle panel = { first.x - 16.0f, first.y - 8.0f, first.width + 24.0f, 18.0f * count + 16.0f };
    DrawRectangle((int)panel.x + 2, (int)panel.y + 2, (int)panel.width, (int)panel.height, Fade(BORDER, 0.5f));
    pixelBox(panel, BORDER, CREAM, PRIMARY);
    DrawRectangleLinesEx(Gui_Inset(panel, 4.0f), 1.0f, DARK);

    for (int i = 0; i < count; i++) {
        Rectangle item = canvasMenuItem(i, count);
        if (i == input->menuHovered) {
            DrawRectangleRec((Rectangle) { item.x - 2.0f, item.y + 1.0f, item.width - 4.0f, item.height - 2.0f }, (Color) { 216, 232, 200, 255 });
        }
        shadowText(input->themeNames[i], item.x + 4.0f, item.y + 6.0f, 1, BORDER, SHADOW);
        if (i == input->currentTheme) {
            pixelText("*", item.x + item.width - 14.0f, item.y + 6.0f, 1, PRIMARY);
        }
    }
    int pointed = input->menuHovered >= 0 ? input->menuHovered : input->currentTheme;
    Rectangle item = canvasMenuItem(pointed, count);
    float bob = (fmodf(time, 0.6f) < 0.3f) ? 0.0f : 1.0f;
    drawHand(item.x - 14.0f + bob, item.y + 5.0f, false);
}

static void draw(const WatchView* view, const GuiLayout* layout, const GuiInput* input)
{
    (void)layout;
    if (!canvasLoaded) {
        canvas = LoadRenderTexture(CANVAS_W, CANVAS_H);
        SetTextureFilter(canvas.texture, TEXTURE_FILTER_POINT);
        canvasLoaded = true;
    }

    BeginTextureMode(canvas);
    tiledPattern((Rectangle) { 0, 0, CANVAS_W, CANVAS_H }, CREAM, (Color) { 240, 240, 208, 255 }, 1);

    drawBanner(view);
    drawBox(view);
    for (int b = GB_CLOCK; b <= GB_TIMESET; b++) {
        drawSlot((enum GuiButton)b, view, input);
    }
    if (Gui_ButtonAvailable(GB_PLUS, view->mode)) {
        drawKeys(view, input);
    }
    drawHint(view);
    drawThemeButton(view, input);

    if (input->menuOpen) {
        drawMenu(input, view->time);
    }
    EndTextureMode();

    ClearBackground(BLACK);
    Rectangle src = { 0, 0, (float)canvas.texture.width, -(float)canvas.texture.height };
    Rectangle dst = { 0, 0, CANVAS_W * PIXEL_SCALE, CANVAS_H * PIXEL_SCALE };
    DrawTexturePro(canvas.texture, src, dst, (Vector2) { 0, 0 }, 0.0f, WHITE);
}

static void unload(void)
{
    if (canvasLoaded) {
        UnloadRenderTexture(canvas);
        canvasLoaded = false;
    }
}

const Theme ThemeEmerald = {
    "Emerald Box",
    layout,
    draw,
    unload,
};
