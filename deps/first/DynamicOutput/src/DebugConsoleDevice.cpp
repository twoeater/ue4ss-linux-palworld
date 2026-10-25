#include <chrono>
#include <locale>
#include <cstdio>
#include <cstdlib>
#include <atomic>

#include <DynamicOutput/DebugConsoleDevice.hpp>
#include <DynamicOutput/Output.hpp>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#ifdef TEXT
#undef TEXT
#endif
#endif

namespace RC::Output
{
    auto DebugConsoleDevice::set_windows_console_out_mode_if_needed() const -> void
    {
        if (m_windows_console_mode_set)
        {
            return;
        }
#ifdef _WIN32
        HANDLE current_console_out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (current_console_out_handle != INVALID_HANDLE_VALUE)
        {
            DWORD current_console_out_mode = 0;
            GetConsoleMode(current_console_out_handle, &current_console_out_mode);
            SetConsoleMode(current_console_out_handle, current_console_out_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
#endif
        m_windows_console_mode_set = true;
    }

    auto DebugConsoleDevice::has_optional_arg() const -> bool
    {
        return true;
    }

    auto DebugConsoleDevice::receive(File::StringViewType fmt) const -> void
    {
        receive_with_optional_arg(fmt, Color::NoColor);
    }

    auto DebugConsoleDevice::receive_with_optional_arg(File::StringViewType fmt, [[maybe_unused]] int32_t optional_arg) const -> void
    {
        static std::atomic<unsigned> receives{0};
        const char* debug = std::getenv("UE4SS_DEBUG");
        const bool trace = debug && std::atoi(debug) > 0 && receives.fetch_add(1) < 8;
        if (trace) fprintf(stderr, "[UE4SS] Console write: entering (%zu UTF-16 code units)...\n", fmt.size());
        set_windows_console_out_mode_if_needed();

#if ENABLE_OUTPUT_DEVICE_DEBUG_MODE
        RC_DEVICE_PRINT_FUNC(fmt, optional_arg, "DebugConsoleDevice received: ")
#else
        RC_DEVICE_PRINT_FUNC(fmt, optional_arg, "")
        if (trace) fprintf(stderr, "[UE4SS] Console write: complete.\n");
#endif
    }
} // namespace RC::Output
