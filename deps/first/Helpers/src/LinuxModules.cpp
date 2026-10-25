#ifdef __linux__
#include <Helpers/LinuxModules.hpp>

namespace RC
{
    namespace
    {
        struct Enumeration
        {
            ElfModuleCallback callback;
            void* data;
        };

        auto dispatch_module(dl_phdr_info* info, size_t size, void* opaque) -> int
        {
            auto& enumeration = *static_cast<Enumeration*>(opaque);
            return enumeration.callback(info, size, enumeration.data);
        }
    }

    auto for_each_elf_module(ElfModuleCallback callback, void* data) -> int
    {
        // dl_iterate_phdr is synchronous. A stack context keeps nested calls and
        // simultaneous enumerations independent without consuming new bridges.
        Enumeration enumeration{callback, data};
        return dl_iterate_phdr(dispatch_module, &enumeration);
    }
}
#endif
