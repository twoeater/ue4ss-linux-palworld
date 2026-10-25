#pragma once

#ifdef __linux__
#include <link.h>

namespace RC
{
    using ElfModuleCallback = int (*)(dl_phdr_info*, size_t, void*);

    // Keep one native callback address for Box64's finite callback bridge table.
    auto for_each_elf_module(ElfModuleCallback callback, void* data) -> int;
}
#endif
