#include <Helpers/LinuxModules.hpp>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <thread>
#include <utility>

namespace
{
    bool direct = false;
    std::atomic<bool> passed{true};
    struct VisitState
    {
        size_t count = 0;
        size_t id = 0;
        bool stop = false;
    };

    template <size_t Id> int visit(dl_phdr_info* info, size_t, void* opaque)
    {
        auto& state = *static_cast<VisitState*>(opaque);
        if (!info || !info->dlpi_phdr || !info->dlpi_phnum) passed = false;
        ++state.count;
        state.id = Id;
        // Full enumeration must reach Box64's native modules to use the bridge.
        return state.stop && state.count == 2 ? static_cast<int>(Id + 1) : 0;
    }

    template <size_t Id> void check()
    {
        VisitState state;
        int result = direct ? dl_iterate_phdr(visit<Id>, &state)
                            : RC::for_each_elf_module(visit<Id>, &state);
        if (state.count < 2 || state.id != Id || result != 0) passed = false;
        state = {0, 0, true};
        result = direct ? dl_iterate_phdr(visit<Id>, &state)
                        : RC::for_each_elf_module(visit<Id>, &state);
        if (state.count != 2 || result != Id + 1) passed = false;
    }

    template <size_t... Ids> void check_all(std::index_sequence<Ids...>)
    {
        (check<Ids>(), ...);
    }
}

int main(int argc, char** argv)
{
    direct = argc > 1 && std::strcmp(argv[1], "--direct") == 0;
    check_all(std::make_index_sequence<32>{});
    if (!direct)
    {
        size_t nested = 0;
        RC::for_each_elf_module([](dl_phdr_info*, size_t, void* data) {
            check<40>();
            ++*static_cast<size_t*>(data);
            return 1;
        }, &nested);
        if (nested != 1) passed = false;
        std::thread first([] { check_all(std::make_index_sequence<32>{}); });
        std::thread second([] { check_all(std::make_index_sequence<32>{}); });
        first.join();
        second.join();
    }
    std::puts(passed ? "ELF enumeration PASS" : "ELF enumeration FAIL");
    return passed ? 0 : 1;
}
