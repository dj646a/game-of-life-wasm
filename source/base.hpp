#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <cstdarg>
#include <cstddef>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <SDL2/SDL.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <stb/stb_image.h>
#include <stb/stb_rect_pack.h>
#include <stb/stb_truetype.h>

#define array_size(array) (sizeof(array) / sizeof(array[0]))

// Only checks once on the first call.
// Therefore attaching to the process after it has already launched will return false.
static inline bool debugger_is_attached()
{
    static bool have_checked = false;
    static bool result = false;

    if (!have_checked)
    {
        {
            char process_status[1024] = {};
            FILE* file = fopen("/proc/self/status", "rb");
            fread(process_status, array_size(process_status)-1, 1, file);

            const char* needle   = "TracerPid:\t";
            const char* haystack = process_status;
            const char* offset   = strstr(haystack, needle) + sizeof("TracerPid:\t" ) - 1;

            // TracerPid is zero if no debugger is attached.
            result = *offset != '0';
            have_checked = true;
            fclose(file);
        }
    }

    return result;
}

#define thred_local  __thread
#define debugger __asm__ volatile ("int3;")
#define crash *(volatile uintptr_t*) 0 = 0;
#define unreachable   __builtin_unreachable()

static thread_local bool assert_result;
#define assert_with_message(condition, ...)                                                                                   \
    do                                                                                                                        \
    {                                                                                                                         \
        assert_result = condition;                                                                                            \
        if (!(condition))                                                                                                     \
        {                                                                                                                     \
            enum { message_buffer_capacity = 1024 };                                                                          \
            char message_buffer[message_buffer_capacity] = {0};                                                               \
            snprintf(message_buffer, message_buffer_capacity, __VA_ARGS__);                                                   \
            fprintf(stderr, "[ASSERT]: %s:%d %c %s\n", __FILE__, __LINE__, message_buffer[0] ? '-' : ' ', message_buffer);    \
            if (debugger_is_attached())                                                                                       \
                debugger;                                                                                                     \
            else                                                                                                              \
                crash;                                                                                                        \
                                                                                                                              \
        }                                                                                                                     \
    } while(0)                                                                                                                \

#define assert(condition) assert_with_message(condition, "")
#define invalid_code_path assert_with_message(false, "INVALID_CODE_PATH"); unreachable;
#define not_implemented   assert_with_message(false, "NOT_IMPLEMENTED");   unreachable;
