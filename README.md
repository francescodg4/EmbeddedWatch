# EmbeddedWatch

A small C implementation of a digital watch, built as a hierarchical state
machine. It models the classic four-mode digital watch — Clock, Alarm,
Stopwatch, and Time-set — as a set of cooperating state machines driven by
signals (button presses and clock ticks).

The code has no dynamic memory allocation and no platform dependencies,
making it suitable to embed in resource-constrained targets; the current
repository provides the core logic and a unit test suite, without a
hardware or platform integration layer.

## Design

Each mode is its own state machine:

- `ClockCounter` — a tenths-of-a-second counter with day rollover; the
  shared primitive behind every timekeeping component.
- `EWatchClock` — the running time-of-day clock.
- `EWatchStopwatch` — start/stop/reset stopwatch.
- `EWatchTimeset` — hours/minutes editing, used both to set the clock and
  to set the alarm expiration time.
- `EWatchAlarm` — alarm configuration and expiration, driven by clock
  ticks from `EWatchClock`.
- `EWatch` — the top-level state machine that switches between modes and
  routes button/tick signals to the active submachine.

## Building

Requirements: CMake 3.16+, a C/C++ compiler, and network access on first
configure (CMake's `FetchContent` pulls in Catch2 for the test suite).

```sh
cmake -S . -B build
cmake --build build
```

## Running the tests

```sh
cmake --build build --target run-test
./build/test/run-test
```

## Desktop GUI

An optional [raylib](https://www.raylib.com/) front end drives the watch
state machine at 10 Hz and renders it with one of three themes, chosen at
runtime with the **Select theme** button:

- **Modern Glass** � frosted glass display, bento-grid mode cells and
  tactile clay buttons (`skills/Graphics.md`).
- **Winamp Modern** � metal frame, cobalt seven-segment LCD, spectrum
  analyzer and transport deck (`skills/WINAMP.Graphics.md`).
- **Emerald Box** � GBA-style pixel-grid storage box, rendered at half
  resolution and upscaled 2x (`skills/Emerald.Graphics.md`).

The GUI is off by default; enabling it fetches raylib on first configure:

```sh
cmake -S . -B build -DBUILD_GUI=ON
cmake --build build --target ewatch-gui
./build/gui/ewatch-gui
```

Keyboard shortcuts: `1`-`4` (or `C`/`A`/`S`/`E`) switch mode, `+`/`-` (or
the arrow keys) adjust, and `T` opens the theme menu.

## License

Distributed under the MIT License. See [LICENSE](LICENSE) for details.
