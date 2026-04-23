# SDL3 Examples

Simple SDL3 example programs using the SDL3 library installed at SDL3_DIR = `/opt/homebrew/opt/sdl3`.

## Source Files

- `hello.c` - Basic SDL3 window example
- `hello-callbacks.c` - SDL3 example using callback API

## Build

```bash
make main         # Build hello (default)
make callbacks    # Build hello-callbacks
make              # Same as make main
```

## Run

```bash
make run              # Run hello
make run-callbacks    # Run hello-callbacks
```

## Clean

```bash
make clean
```

## Build Output

Compiled binaries are placed in the `Build/` directory.
