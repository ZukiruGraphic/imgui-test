#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/log.h>
#include <android/input.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"
static ANativeWindow* win = nullptr; static bool g_run = false; static bool g_init = false; static pthread_t g_th; static pthread_mutex_t g_mu = PTHREAD_MUTEX_INITIALIZER; static float g_value = 0.5f;
static AInputQueue* g_input = nullptr; static pthread_mutex_t g_input_mu = PTHREAD_MUTEX_INITIALIZER;
static void slog(const char* s) { FILE* f = fopen("/storage/emulated/0/Download/imgui_debug.txt", "a"); if (f) { fprintf(f, "%s\n", s); fclose(f); } }
static void drain_input() {
  pthread_mutex_lock(&g_input_mu);
  AInputQueue* q = g_input;
  pthread_mutex_unlock(&g_input_mu);
  if (!q) return;
  AInputEvent* ev = nullptr;
  while (AInputQueue_hasEvents(q) > 0) {
    if (AInputQueue_getEvent(q, &ev) < 0) break;
    if (AInputQueue_preDispatchEvent(q, ev)) continue;
    int32_t handled = 0;
    if (g_init) handled = ImGui_ImplAndroid_HandleInputEvent(ev);
    AInputQueue_finishEvent(q, ev, handled);
  }
}
static void* render_loop(void* p) {
  EGLDisplay dpy = EGL_NO_DISPLAY; EGLSurface surf = EGL_NO_SURFACE; EGLContext ctx = EGL_NO_CONTEXT;
  while (1) {
    pthread_mutex_lock(&g_mu);
    ANativeWindow* w = win; bool run = g_run;
    pthread_mutex_unlock(&g_mu);
    if (w == nullptr || !run) {
      if (dpy != EGL_NO_DISPLAY) {
        slog("shutdown start");
        if (g_init) { ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplAndroid_Shutdown(); ImGui::DestroyContext(); g_init = false; slog("imgui shutdown ok"); }
        eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (surf != EGL_NO_SURFACE) eglDestroySurface(dpy, surf);
        if (ctx != EGL_NO_CONTEXT) eglDestroyContext(dpy, ctx);
        eglTerminate(dpy);
        dpy = EGL_NO_DISPLAY; surf = EGL_NO_SURFACE; ctx = EGL_NO_CONTEXT;
        slog("egl shutdown ok");
      }
      usleep(16000); continue;
    }
    if (dpy == EGL_NO_DISPLAY) {
      dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY); eglInitialize(dpy, 0, 0);
      const EGLint a[] = {EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8, EGL_DEPTH_SIZE, 24, EGL_NONE};
      EGLConfig c; EGLint n; eglChooseConfig(dpy, a, &c, 1, &n);
      const EGLint ca[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
      ctx = eglCreateContext(dpy, c, EGL_NO_CONTEXT, ca);
      surf = eglCreateWindowSurface(dpy, c, (EGLNativeWindowType)w, nullptr);
      eglMakeCurrent(dpy, surf, surf, ctx);
      slog("egl init ok");
      if (!g_init) {
        IMGUI_CHECKVERSION(); ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        int ww = ANativeWindow_getWidth(w); float s = ww / 800.0f; if (s < 1.0f) s = 1.0f; if (s > 3.0f) s = 3.0f;
        io.FontGlobalScale = s; io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen; ImGui::StyleColorsDark(); ImGui::GetStyle().ScaleAllSizes(s);
        ImGui_ImplAndroid_Init(w); ImGui_ImplOpenGL3_Init("#version 300 es");
        g_init = true; slog("imgui init ok");
      }
    }
    if (!g_init || dpy == EGL_NO_DISPLAY) { usleep(16000); continue; }
    drain_input();
    int wi = ANativeWindow_getWidth(w); int hi = ANativeWindow_getHeight(w);
    if (wi <= 0 || hi <= 0) { usleep(16000); continue; }
    ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplAndroid_NewFrame(); ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0)); ImGui::SetNextWindowSize(ImVec2((float)wi, (float)hi));
    ImGui::Begin("Hello Termux", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::Text("ImGui jalan di Android! APK");
    ImGui::Text("Layar: %dx%d", wi, hi);
    ImGui::SliderFloat("Value", &g_value, 0.0f, 1.0f);
    if (ImGui::Button("Klik Saya")) { slog("button clicked"); }
    ImGui::End();
    ImGui::Render();
    glViewport(0, 0, wi, hi);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (eglSwapBuffers(dpy, surf) == EGL_FALSE) { slog("swap failed"); }
    usleep(16000);
  }
  return nullptr;
}
static void onWindowCreated(ANativeActivity* a, ANativeWindow* w) { slog("onWindowCreated"); ANativeWindow_acquire(w); pthread_mutex_lock(&g_mu); win = w; g_run = true; pthread_mutex_unlock(&g_mu); }
static void onWindowDestroyed(ANativeActivity* a, ANativeWindow* w) { slog("onWindowDestroyed"); pthread_mutex_lock(&g_mu); win = nullptr; pthread_mutex_unlock(&g_mu); ANativeWindow_release(w); }
static void onStart(ANativeActivity* a) { slog("onStart"); pthread_mutex_lock(&g_mu); g_run = true; pthread_mutex_unlock(&g_mu); }
static void onStop(ANativeActivity* a) { slog("onStop"); pthread_mutex_lock(&g_mu); g_run = false; pthread_mutex_unlock(&g_mu); }
static void onPause(ANativeActivity* a) { slog("onPause"); pthread_mutex_lock(&g_mu); g_run = false; pthread_mutex_unlock(&g_mu); }
static void onResume(ANativeActivity* a) { slog("onResume"); pthread_mutex_lock(&g_mu); g_run = true; pthread_mutex_unlock(&g_mu); }
static void onInputCreated(ANativeActivity* a, AInputQueue* q) { slog("onInputCreated"); pthread_mutex_lock(&g_input_mu); g_input = q; pthread_mutex_unlock(&g_input_mu); }
static void onInputDestroyed(ANativeActivity* a, AInputQueue* q) { slog("onInputDestroyed"); pthread_mutex_lock(&g_input_mu); if (g_input == q) g_input = nullptr; pthread_mutex_unlock(&g_input_mu); }
extern "C" void ANativeActivity_onCreate(ANativeActivity* a, void* saved, size_t len) {
  remove("/storage/emulated/0/Download/imgui_debug.txt");
  slog("onCreate");
  a->callbacks->onNativeWindowCreated = onWindowCreated;
  a->callbacks->onNativeWindowDestroyed = onWindowDestroyed;
  a->callbacks->onStart = onStart;
  a->callbacks->onStop = onStop;
  a->callbacks->onPause = onPause;
  a->callbacks->onResume = onResume;
  a->callbacks->onInputQueueCreated = onInputCreated;
  a->callbacks->onInputQueueDestroyed = onInputDestroyed;
  pthread_create(&g_th, nullptr, render_loop, nullptr);
  slog("onCreate ok");
}
