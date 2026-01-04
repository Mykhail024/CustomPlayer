#if defined(__SANITIZE_THREAD__) || defined(__has_feature)
#if __has_feature(thread_sanitizer) || defined(__SANITIZE_THREAD__)
extern "C" const char *__tsan_default_options() { return "ignore_noninstrumented_modules=1"; }
#endif
#endif
