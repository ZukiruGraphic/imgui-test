# imgui-test

ImGui + SDL2 + GLES3 on Termux (Android).
Preview with Termux:X11, build native as APK.

## Features
- Adaptive scale dispW / 800 (clamped 1.0 - 3.0)
- Default StyleColorsDark() (semi-transparent button is ImGui default)
- APK: NativeActivity + EGL + ImGui_ImplAndroid, touch working
- Clean lifecycle: onPause -> shutdown -> onStart again without crash

## Supported Devices
- CPU: arm64-v8a (64-bit) only
- Android: minimum 24 (Android 7.0 Nougat) and up, target 34 (Android 14)
- GPU: must support OpenGL ES 3.0 + EGL
- 32-bit (armeabi-v7a) is not supported yet and not tested, because there is no 32-bit device available for testing
- Vulkan-capable devices also work, because they also support OpenGL ES 3.0. This app uses OpenGL ES 3.0, not Vulkan.

## X11 Preview
```bash
cd ~/imgui-test
cmake --build build -j$(nproc)
export DISPLAY=:0
export LIBGL_ALWAYS_SOFTWARE=1
export MESA_LOADER_DRIVER_OVERRIDE=swrast
export SDL_VIDEODRIVER=x11
./build/app &
```

## Build APK
```bash
cd ~/imgui-test/apk/app/src/main/cpp
cmake --build build -j$(nproc)
patchelf --replace-needed libEGL.so.1 libEGL.so build/libimguitest.so
cd ~/imgui-test/apk
cp app/src/main/cpp/build/libimguitest.so lib/arm64-v8a/libimguitest.so
aapt package -f -M app/src/main/AndroidManifest.xml -I sdk/android.jar -F out/base.apk --min-sdk-version 24 --target-sdk-version 34 --version-code 1 --version-name 1.0
cp out/base.apk out/unsigned.apk
zip -r -0 out/unsigned.apk lib/
apksigner sign --ks debug.p12 --ks-type PKCS12 --ks-pass pass:android --key-pass pass:android --ks-key-alias debug --out out/imguitest.apk out/unsigned.apk
cp out/imguitest.apk ~/storage/downloads/imguitest.apk
```

## Release
v1.0.0 - imguitest-base.apk - base build with touch + default theme.

## Fixed bugs
- Pale colors on X11 = old binary still running, not sRGB. pkill before running the new build.
- Semi-transparent button = ImGui default, alpha 0.40.
- Tap caused force close = AInputQueue was not polled, Android treated it as ANR.
- Force close on open = libEGL.so.1 does not exist on phone, must patchelf to libEGL.so.
- Build failed = ImGuiConfigFlags_NavEnableTouch does not exist in this ImGui version.
