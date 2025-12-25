
# Mupen64plus quickstart guide

## Configuration file

`Mk64t` uses its own configuration file, located in:
- Linux: `$HOME/.config/mk64t/mupen64plus/config/`
- Windows: 

### Useful settings

There are many ways to customize your `mupen64plus` experience. Here are some of the main configuration settings.
```
[Core]

# Display mupen64plus configurations on screen on startup
OnScreenDisplay = True

```

Go to the `[CoreEvents]` section to change keyboard shortcuts. The keys are mapped to ASCII character decimal values. Here is a [complete list](https://www.libsdl.org/release/SDL-1.2.15/include/SDL_keysym.h) from the `SDL` library website.
```
[CoreEvents]

# Mupen64Plus CoreEvents config parameter set version number.  Please don't change this version number.
Version = 1
# SDL keysym for stopping the emulator
Kbd Mapping Stop = 27
# SDL keysym for switching between fullscreen/windowed modes
Kbd Mapping Fullscreen = 0
# SDL keysym for saving the emulator state
Kbd Mapping Save State = 286
# SDL keysym for loading the emulator state
Kbd Mapping Load State = 288
# ...
```
