# wink

A TUI framework for [Blink](https://blinklang.org). Built with Blink. Don't blink.

**Status: pre-alpha — not yet functional.**

## What is this?

`wink` is a terminal UI framework for the Blink programming language, in the same spirit as
[Ratatui](https://ratatui.rs) (Rust) or [Bubbletea](https://github.com/charmbracelet/bubbletea) (Go).

It is also a primary dogfooding project for Blink itself — building it surfaces real language gaps,
exercises the stdlib, and results in something immediately useful for Blink tooling.

## Architecture (planned)

`wink` follows an **Elm-style TEA (The Elm Architecture)** loop:

```
init() -> Model
update(model, event) -> Model
view(model) -> Canvas
```

The runtime handles the event loop, diffed rendering, and cleanup.

### Planned layers

| Module | Description |
|--------|-------------|
| `wink.raw` | termios raw mode, non-blocking stdin |
| `wink.input` | key/mouse/paste event parser |
| `wink.signal` | SIGWINCH / SIGINT hooks |
| `wink.unicode` | Unicode display width (wcwidth) |
| `wink.app` | TEA event loop |
| `wink.view` | Cell buffer + diff renderer |
| `wink.layout` | Flex/constraint layout engine |
| `wink.widget` | Built-in widgets |
| `wink.event` | Event types |
| `wink.style` | Style builder ergonomics |

> The low-level primitives (`raw`, `input`, `signal`, `unicode`) live in `wink` for now.
> If they prove useful beyond TUIs, they can be promoted into `std.term` later.

### Planned widgets (v0.1 scope)

- `Text` — styled text block
- `Box` — bordered container
- `List` — scrollable item list with selection
- `Input` — single-line text input
- `Progress` — progress bar
- `Keybindings` — help bar showing bound keys

## What `wink` adds beyond `std.term`

`std.term` covers TTY detection, dimensions, basic ANSI colors, and cursor movement.
`wink` builds everything else needed for interactive TUIs:

- Raw mode / termios (`ICANON`, `ECHO` off, cbreak)
- Non-blocking stdin reads
- Key event parsing (arrows, F-keys, Ctrl/Alt/Shift modifiers)
- Mouse tracking (click, drag, scroll)
- Alternate screen buffer (`?1049h`)
- SIGWINCH (resize) handling
- Unicode display width (`wcwidth`) — CJK, emoji, combining marks
- Truecolor / 256-color support
- Cell buffer + diffed redraws (no flicker)
- Layout engine
- Widgets

## Non-goals for v0.1

- Images (Kitty graphics / Sixel / iTerm2)
- Animations / transitions
- Windows support
- Retained component tree (React-style) — TEA only for now

## Usage (aspirational)

```blink
import wink

struct Model {
    count: Int,
}

fn update(model: Model, event: wink.Event) -> Model {
    match event {
        wink.Key(k) if k == wink.KeyUp => Model { count: model.count + 1, ..model },
        wink.Key(k) if k == wink.KeyDown => Model { count: model.count - 1, ..model },
        _ => model,
    }
}

fn view(model: Model) -> wink.Canvas {
    wink.text("Count: {model.count}")
}

fn main() {
    wink.run(Model { count: 0 }, update, view)
}
```

## License

MIT
