/*
 * Modern theme: adaptive visual physics (skills/Graphics.md).
 * Dark canvas with drifting ambient radiance, a frosted glass display,
 * bento-grid mode cells and tactile clay buttons that compress on press.
 */
#include "Gui.h"

#include <math.h>
#include <stdio.h>

#define MARGIN 24.0f
#define GAP 16.0f

static const Color BG_TOP = { 9, 12, 24, 255 };
static const Color BG_BOTTOM = { 22, 14, 38, 255 };
static const Color CLAY = { 28, 33, 52, 255 };
static const Color CLAY_LIGHT = { 44, 50, 74, 255 };
static const Color TEXT_MAIN = { 240, 244, 255, 255 };
static const Color TEXT_DIM = { 160, 170, 200, 255 };

static Color accentFor(const WatchView* view)
{
    if (view->alarmState == ALARM_EXPIRED) {
        float pulse = 0.5f + 0.5f * sinf(view->time * 10.0f);
        return ColorLerp((Color) { 248, 113, 113, 255 }, (Color) { 251, 191, 36, 255 }, pulse);
    }
    switch (view->mode) {
    case ALARM_MODE:
        return (Color) { 251, 191, 36, 255 };
    case STOPWATCH_MODE:
        return (Color) { 52, 211, 153, 255 };
    case TIMESET_MODE:
        return (Color) { 167, 139, 250, 255 };
    default:
        return (Color) { 56, 189, 248, 255 };
    }
}

static Color modeAccent(enum GuiButton button)
{
    switch (button) {
    case GB_ALARM:
        return (Color) { 251, 191, 36, 255 };
    case GB_STOPWATCH:
        return (Color) { 52, 211, 153, 255 };
    case GB_TIMESET:
        return (Color) { 167, 139, 250, 255 };
    default:
        return (Color) { 56, 189, 248, 255 };
    }
}

static Rectangle displayRect(void)
{
    return (Rectangle) { MARGIN, 76.0f, GUI_SCREEN_WIDTH - 2.0f * MARGIN, 230.0f };
}

static void layout(GuiLayout* out, int themeCount)
{
    float cellW = (GUI_SCREEN_WIDTH - 2.0f * MARGIN - GAP) / 2.0f;
    float cellH = 86.0f;
    float gridY = 322.0f;

    out->buttons[GB_CLOCK] = (Rectangle) { MARGIN, gridY, cellW, cellH };
    out->buttons[GB_ALARM] = (Rectangle) { MARGIN + cellW + GAP, gridY, cellW, cellH };
    out->buttons[GB_STOPWATCH] = (Rectangle) { MARGIN, gridY + cellH + GAP, cellW, cellH };
    out->buttons[GB_TIMESET] = (Rectangle) { MARGIN + cellW + GAP, gridY + cellH + GAP, cellW, cellH };

    float padY = gridY + 2.0f * (cellH + GAP);
    out->buttons[GB_MINUS] = (Rectangle) { MARGIN, padY, cellW, cellH };
    out->buttons[GB_PLUS] = (Rectangle) { MARGIN + cellW + GAP, padY, cellW, cellH };

    out->buttons[GB_THEME] = (Rectangle) { GUI_SCREEN_WIDTH - MARGIN - 144.0f, 20.0f, 144.0f, 38.0f };

    for (int i = 0; i < themeCount && i < GUI_MAX_THEMES; i++) {
        out->menuItems[i] = (Rectangle) { GUI_SCREEN_WIDTH - MARGIN - 212.0f + 8.0f, 74.0f + 8.0f + 46.0f * i, 196.0f, 42.0f };
    }
}

static void drawBackground(const WatchView* view, Color accent)
{
    DrawRectangleGradientV(0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT, BG_TOP, BG_BOTTOM);

    /* Ambient radiance: slow-drifting soft light sources. */
    float t = view->time * 0.25f;
    BeginBlendMode(BLEND_ADDITIVE);
    DrawCircleGradient((int)(110 + 60 * sinf(t)), (int)(170 + 40 * cosf(t * 1.3f)), 230.0f, Fade(accent, 0.22f), Fade(accent, 0.0f));
    DrawCircleGradient((int)(340 + 50 * cosf(t * 0.8f)), (int)(470 + 60 * sinf(t * 1.1f)), 260.0f, Fade((Color) { 139, 92, 246, 255 }, 0.16f), Fade((Color) { 139, 92, 246, 255 }, 0.0f));
    EndBlendMode();
}

static void glassPanel(Rectangle r, float radius, float opacity)
{
    float round = Gui_Roundness(r, radius);
    DrawRectangleRounded((Rectangle) { r.x, r.y + 10.0f, r.width, r.height }, round, 16, Fade(BLACK, 0.25f));
    DrawRectangleRounded(r, round, 16, Fade((Color) { 180, 195, 255, 255 }, opacity));
    /* Directional light: a faint sheen across the top edge. */
    Rectangle sheen = { r.x + radius, r.y + 1.0f, r.width - 2.0f * radius, r.height * 0.35f };
    DrawRectangleGradientV((int)sheen.x, (int)sheen.y, (int)sheen.width, (int)sheen.height, Fade(WHITE, 0.06f), Fade(WHITE, 0.0f));
    DrawRectangleRoundedLinesEx(r, round, 16, 1.0f, Fade(WHITE, 0.15f));
}

static void glowText(Font font, const char* text, Vector2 pos, float size, Color color, Color glow, float alpha)
{
    BeginBlendMode(BLEND_ADDITIVE);
    for (int i = 0; i < 12; i++) {
        float a = (float)i * (2.0f * PI / 12.0f);
        Vector2 p = { pos.x + 4.0f * cosf(a), pos.y + 4.0f * sinf(a) };
        DrawTextEx(font, text, p, size, 0.0f, Fade(glow, 0.05f * alpha));
    }
    EndBlendMode();
    DrawTextEx(font, text, pos, size, 0.0f, Fade(color, alpha));
}

static void drawDisplay(const WatchView* view, Color accent)
{
    const GuiFonts* fonts = Gui_Fonts();
    Rectangle card = displayRect();
    glassPanel(card, 24.0f, 0.07f);

    /* Mode pill */
    const char* modeName = Gui_ModeName(view->mode);
    Vector2 modeSize = MeasureTextEx(fonts->uiBold, modeName, 15.0f, 1.0f);
    Rectangle pill = { card.x + 20.0f, card.y + 18.0f, modeSize.x + 34.0f, 28.0f };
    DrawRectangleRounded(pill, 1.0f, 12, Fade(accent, 0.16f));
    DrawRectangleRoundedLinesEx(pill, 1.0f, 12, 1.0f, Fade(accent, 0.5f));
    DrawCircleV((Vector2) { pill.x + 14.0f, pill.y + 14.0f }, 4.0f, accent);
    Gui_TextLeft(fonts->uiBold, modeName, (Vector2) { pill.x + 24.0f, pill.y + 14.0f - modeSize.y / 2.0f }, 15.0f, 1.0f, TEXT_MAIN);

    /* Alarm status */
    char alarmText[32];
    snprintf(alarmText, sizeof(alarmText), "ALARM %s  %02d:%02d", Gui_AlarmStateName(view->alarmState), view->alarmHours, view->alarmMinutes);
    Vector2 alarmSize = MeasureTextEx(fonts->ui, alarmText, 15.0f, 0.5f);
    Color alarmColor = view->alarmState == ALARM_OFF ? TEXT_DIM : (Color) { 251, 191, 36, 255 };
    if (view->alarmState == ALARM_EXPIRED) {
        alarmColor = accent;
    }
    Gui_TextLeft(fonts->ui, alarmText, (Vector2) { card.x + card.width - 20.0f - alarmSize.x, pill.y + 14.0f - alarmSize.y / 2.0f }, 15.0f, 0.5f, alarmColor);

    /* Time readout */
    char hh[4], mm[4], ss[8];
    snprintf(hh, sizeof(hh), "%02d", view->hours);
    snprintf(mm, sizeof(mm), "%02d", view->minutes);
    if (view->mode == STOPWATCH_MODE) {
        snprintf(ss, sizeof(ss), "%02d.%d", view->seconds, view->tenths);
    } else {
        snprintf(ss, sizeof(ss), "%02d", view->seconds);
    }

    float big = 104.0f;
    float small = 34.0f;
    Font f = fonts->light;
    float wH = MeasureTextEx(f, hh, big, 0.0f).x;
    float wC = MeasureTextEx(f, ":", big, 0.0f).x;
    float wM = MeasureTextEx(f, mm, big, 0.0f).x;
    float wS = MeasureTextEx(f, ss, small, 0.0f).x;
    float total = wH + wC + wM + 10.0f + wS;
    float x = card.x + (card.width - total) / 2.0f;
    float y = card.y + 58.0f;

    bool secondsShown = view->mode == CLOCK_MODE || view->mode == STOPWATCH_MODE;
    float colonAlpha = (view->mode == CLOCK_MODE && view->tenths >= 5) ? 0.35f : 1.0f;

    glowText(f, hh, (Vector2) { x, y }, big, TEXT_MAIN, accent, Gui_BlinkOff(view, true) ? 0.15f : 1.0f);
    glowText(f, ":", (Vector2) { x + wH, y - 6.0f }, big, TEXT_MAIN, accent, colonAlpha);
    glowText(f, mm, (Vector2) { x + wH + wC, y }, big, TEXT_MAIN, accent, Gui_BlinkOff(view, false) ? 0.15f : 1.0f);
    DrawTextEx(f, ss, (Vector2) { x + wH + wC + wM + 10.0f, y + big - small - 18.0f }, small, 0.0f, Fade(accent, secondsShown ? 1.0f : 0.3f));

    /* Hint */
    Gui_TextCentered(fonts->ui, Gui_Hint(view), (Rectangle) { card.x, card.y + card.height - 64.0f, card.width, 20.0f }, 15.0f, 0.5f, TEXT_DIM);

    /* Progress track with a radiant leading edge */
    float fraction;
    switch (view->mode) {
    case ALARM_MODE:
        fraction = (view->hours * 60 + view->minutes) / 1440.0f;
        break;
    case TIMESET_MODE:
        fraction = view->editingHours ? view->hours / 23.0f : view->minutes / 59.0f;
        break;
    default:
        fraction = (view->seconds + view->tenths / 10.0f) / 60.0f;
        break;
    }
    Rectangle track = { card.x + 24.0f, card.y + card.height - 30.0f, card.width - 48.0f, 6.0f };
    DrawRectangleRounded(track, 1.0f, 8, Fade(WHITE, 0.08f));
    Rectangle fill = track;
    fill.width = fmaxf(track.height, track.width * fraction);
    DrawRectangleRounded(fill, 1.0f, 8, accent);
    Vector2 head = { fill.x + fill.width - 3.0f, track.y + 3.0f };
    BeginBlendMode(BLEND_ADDITIVE);
    DrawCircleGradient((int)head.x, (int)head.y, 18.0f, Fade(accent, 0.6f), Fade(accent, 0.0f));
    EndBlendMode();
    DrawCircleV(head, 5.0f, TEXT_MAIN);
}

/* Soft volumetric surface with inner highlight/shade passes. */
static Rectangle clay(Rectangle r, float radius, bool hovered, bool pressed, Color base)
{
    if (pressed) {
        r = (Rectangle) { r.x + r.width * 0.01f, r.y + r.height * 0.01f, r.width * 0.98f, r.height * 0.98f };
    }
    float round = Gui_Roundness(r, radius);

    if (!pressed) {
        DrawRectangleRounded((Rectangle) { r.x + 2.0f, r.y + 12.0f, r.width - 4.0f, r.height }, round, 16, Fade(BLACK, 0.22f));
        DrawRectangleRounded((Rectangle) { r.x, r.y + 5.0f, r.width, r.height }, round, 16, Fade(BLACK, 0.3f));
    }

    Color face = hovered ? ColorLerp(base, CLAY_LIGHT, 0.5f) : base;
    if (pressed) {
        DrawRectangleRounded(r, round, 16, ColorBrightness(face, -0.1f));
    } else {
        DrawRectangleRounded(r, round, 16, ColorBrightness(face, 0.35f));
        DrawRectangleRounded((Rectangle) { r.x, r.y + 3.0f, r.width, r.height - 3.0f }, round, 16, ColorBrightness(face, -0.25f));
        DrawRectangleRounded((Rectangle) { r.x, r.y + 2.0f, r.width, r.height - 4.0f }, round, 16, face);
    }
    return r;
}

static void radiance(Rectangle r, float radius, Color glow, float strength)
{
    BeginBlendMode(BLEND_ADDITIVE);
    for (int i = 1; i <= 6; i++) {
        float grow = 3.0f * i;
        Rectangle g = { r.x - grow, r.y - grow, r.width + 2.0f * grow, r.height + 2.0f * grow };
        DrawRectangleRounded(g, Gui_Roundness(g, radius + grow), 16, Fade(glow, strength * 0.05f * (7 - i) / 6.0f));
    }
    EndBlendMode();
}

static void drawIcon(enum GuiButton button, Vector2 c, Color color)
{
    switch (button) {
    case GB_CLOCK:
        DrawRing(c, 8.0f, 10.0f, 0.0f, 360.0f, 32, color);
        DrawLineEx(c, (Vector2) { c.x, c.y - 6.0f }, 2.0f, color);
        DrawLineEx(c, (Vector2) { c.x + 5.0f, c.y }, 2.0f, color);
        break;
    case GB_ALARM:
        DrawCircleSector((Vector2) { c.x, c.y + 3.0f }, 9.0f, 180.0f, 360.0f, 24, color);
        DrawRectangleRounded((Rectangle) { c.x - 11.0f, c.y + 2.0f, 22.0f, 4.0f }, 1.0f, 6, color);
        DrawCircleV((Vector2) { c.x, c.y + 8.5f }, 2.5f, color);
        break;
    case GB_STOPWATCH:
        DrawRing((Vector2) { c.x, c.y + 2.0f }, 7.0f, 9.0f, 0.0f, 360.0f, 32, color);
        DrawRectangleRec((Rectangle) { c.x - 3.0f, c.y - 11.0f, 6.0f, 3.0f }, color);
        DrawLineEx((Vector2) { c.x, c.y + 2.0f }, (Vector2) { c.x + 3.0f, c.y - 2.0f }, 2.0f, color);
        break;
    case GB_TIMESET:
        for (int i = 0; i < 3; i++) {
            float y = c.y - 7.0f + 7.0f * i;
            DrawLineEx((Vector2) { c.x - 10.0f, y }, (Vector2) { c.x + 10.0f, y }, 2.0f, Fade(color, 0.5f));
            DrawCircleV((Vector2) { c.x - 6.0f + 6.0f * ((i * 2) % 3), y }, 3.5f, color);
        }
        break;
    default:
        break;
    }
}

static void drawModeCell(enum GuiButton button, const WatchView* view, const GuiLayout* layout, const GuiInput* input)
{
    const GuiFonts* fonts = Gui_Fonts();
    Rectangle r = layout->buttons[button];
    Color accent = modeAccent(button);
    bool active = (button == GB_CLOCK && view->mode == CLOCK_MODE)
        || (button == GB_ALARM && view->mode == ALARM_MODE)
        || (button == GB_STOPWATCH && view->mode == STOPWATCH_MODE)
        || (button == GB_TIMESET && view->mode == TIMESET_MODE);

    if (active) {
        radiance(r, 20.0f, accent, 1.0f);
    }
    r = clay(r, 20.0f, input->hovered[button], input->pressed[button], active ? ColorLerp(CLAY, accent, 0.18f) : CLAY);
    if (active) {
        DrawRectangleRoundedLinesEx(r, Gui_Roundness(r, 20.0f), 16, 1.0f, Fade(accent, 0.6f));
    }

    drawIcon(button, (Vector2) { r.x + 30.0f, r.y + 30.0f }, active ? accent : TEXT_DIM);

    char key[2] = { (char)('1' + button), '\0' };
    Gui_TextLeft(fonts->ui, key, (Vector2) { r.x + r.width - 24.0f, r.y + 14.0f }, 14.0f, 0.0f, Fade(TEXT_DIM, 0.5f));
    Gui_TextLeft(fonts->uiBold, Gui_ButtonLabel(button, view), (Vector2) { r.x + 20.0f, r.y + r.height - 34.0f }, 17.0f, 1.0f, active ? TEXT_MAIN : TEXT_DIM);
}

static void drawAdjustButton(enum GuiButton button, const WatchView* view, const GuiLayout* layout, const GuiInput* input, Color accent)
{
    const GuiFonts* fonts = Gui_Fonts();
    Rectangle r = clay(layout->buttons[button], 43.0f, input->hovered[button], input->pressed[button], CLAY);

    Vector2 c = { r.x + 48.0f, r.y + r.height / 2.0f };
    DrawCircleV(c, 22.0f, Fade(accent, 0.14f));
    DrawRectangleRounded((Rectangle) { c.x - 10.0f, c.y - 2.0f, 20.0f, 4.0f }, 1.0f, 6, accent);
    if (button == GB_PLUS) {
        DrawRectangleRounded((Rectangle) { c.x - 2.0f, c.y - 10.0f, 4.0f, 20.0f }, 1.0f, 6, accent);
    }

    const char* label;
    if (view->mode == STOPWATCH_MODE) {
        label = button == GB_PLUS ? (view->stopwatchRunning ? "Stop" : "Start") : "Reset";
    } else {
        label = button == GB_PLUS ? "Increase" : "Decrease";
    }
    Vector2 size = MeasureTextEx(fonts->ui, label, 19.0f, 0.5f);
    Gui_TextLeft(fonts->ui, label, (Vector2) { r.x + 84.0f, c.y - size.y / 2.0f }, 19.0f, 0.5f, TEXT_MAIN);
}

static void drawThemeButton(const GuiLayout* layout, const GuiInput* input)
{
    const GuiFonts* fonts = Gui_Fonts();
    Rectangle r = layout->buttons[GB_THEME];
    if (input->pressed[GB_THEME]) {
        r = Gui_Inset(r, 1.0f);
    }
    glassPanel(r, 19.0f, input->hovered[GB_THEME] || input->menuOpen ? 0.16f : 0.09f);

    DrawCircleV((Vector2) { r.x + 18.0f, r.y + 19.0f }, 4.0f, (Color) { 56, 189, 248, 255 });
    DrawCircleV((Vector2) { r.x + 26.0f, r.y + 15.0f }, 4.0f, (Color) { 167, 139, 250, 255 });
    DrawCircleV((Vector2) { r.x + 26.0f, r.y + 23.0f }, 4.0f, (Color) { 52, 211, 153, 255 });

    Vector2 size = MeasureTextEx(fonts->ui, "Select theme", 16.0f, 0.5f);
    Gui_TextLeft(fonts->ui, "Select theme", (Vector2) { r.x + 40.0f, r.y + (r.height - size.y) / 2.0f }, 16.0f, 0.5f, TEXT_MAIN);
}

static void drawMenu(const GuiLayout* layout, const GuiInput* input)
{
    const GuiFonts* fonts = Gui_Fonts();
    DrawRectangle(0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT, Fade(BLACK, 0.45f));

    Rectangle first = layout->menuItems[0];
    Rectangle panel = { first.x - 8.0f, first.y - 8.0f, first.width + 16.0f, 46.0f * input->themeCount + 12.0f };
    /* Solid-backed frosted layer, legible regardless of what is underneath. */
    DrawRectangleRounded((Rectangle) { panel.x, panel.y + 12.0f, panel.width, panel.height }, Gui_Roundness(panel, 20.0f), 16, Fade(BLACK, 0.35f));
    DrawRectangleRounded(panel, Gui_Roundness(panel, 20.0f), 16, (Color) { 24, 28, 46, 245 });
    DrawRectangleRoundedLinesEx(panel, Gui_Roundness(panel, 20.0f), 16, 1.0f, Fade(WHITE, 0.15f));

    for (int i = 0; i < input->themeCount; i++) {
        Rectangle item = layout->menuItems[i];
        bool current = i == input->currentTheme;
        if (i == input->menuHovered) {
            DrawRectangleRounded(item, Gui_Roundness(item, 14.0f), 12, Fade(WHITE, 0.08f));
        }
        if (current) {
            DrawCircleV((Vector2) { item.x + 20.0f, item.y + item.height / 2.0f }, 5.0f, (Color) { 56, 189, 248, 255 });
            BeginBlendMode(BLEND_ADDITIVE);
            DrawCircleGradient((int)(item.x + 20.0f), (int)(item.y + item.height / 2.0f), 16.0f, Fade((Color) { 56, 189, 248, 255 }, 0.5f), Fade(BLACK, 0.0f));
            EndBlendMode();
        } else {
            DrawRing((Vector2) { item.x + 20.0f, item.y + item.height / 2.0f }, 4.0f, 5.5f, 0.0f, 360.0f, 24, TEXT_DIM);
        }
        Vector2 size = MeasureTextEx(fonts->ui, input->themeNames[i], 17.0f, 0.5f);
        Gui_TextLeft(fonts->ui, input->themeNames[i], (Vector2) { item.x + 38.0f, item.y + (item.height - size.y) / 2.0f }, 17.0f, 0.5f, current ? TEXT_MAIN : TEXT_DIM);
    }
}

static void draw(const WatchView* view, const GuiLayout* layout, const GuiInput* input)
{
    const GuiFonts* fonts = Gui_Fonts();
    Color accent = accentFor(view);

    drawBackground(view, accent);
    Gui_TextLeft(fonts->uiBold, "EmbeddedWatch", (Vector2) { MARGIN, 26.0f }, 22.0f, 0.5f, TEXT_MAIN);

    drawDisplay(view, accent);
    drawModeCell(GB_CLOCK, view, layout, input);
    drawModeCell(GB_ALARM, view, layout, input);
    drawModeCell(GB_STOPWATCH, view, layout, input);
    drawModeCell(GB_TIMESET, view, layout, input);
    drawAdjustButton(GB_MINUS, view, layout, input, accent);
    drawAdjustButton(GB_PLUS, view, layout, input, accent);

    if (input->menuOpen) {
        drawMenu(layout, input);
    }
    drawThemeButton(layout, input);
}

const Theme ThemeModern = {
    "Modern Glass",
    layout,
    draw,
    NULL,
};
