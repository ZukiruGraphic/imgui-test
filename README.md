# imgui-test

ImGui + SDL2 + GLES3 on Termux (Android).
Preview with Termux:X11, build native as APK.

## Features
- Adaptive scale `dispW / 800` (clamped 1.0 - 3.0)
- Default `StyleColorsDark()` (semi-transparent button is ImGui default)
- APK: NativeActivity + EGL + ImGui_ImplAndroid, touch working
- Clean lifecycle: `onPause` -> shutdown -> `onStart` again without crash

## X11 Preview
```bash
cd ~/imgui-test
cmake --build build -j$(nproc)
export DISPLAY=:0
export LIBGL_ALWAYS_SOFTWARE=1
export MESA_LOADER_DRIVER_OVERRIDE=swrast
export SDL_VIDEODRIVER=x11
./build/app &
