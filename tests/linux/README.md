# Linux ELF enumeration regression

`elf_enumeration.cpp` checks 32 distinct callbacks, user data, early termination,
nested enumeration, and simultaneous enumerations. On Box64, direct use of a
different callback for every scanner exhausts its native callback bridge slots.
The shared `RC::for_each_elf_module` dispatcher uses only one slot.

```sh
x86_64-linux-gnu-g++ -std=c++20 -O2 -pthread \
  -I deps/first/Helpers/include tests/linux/elf_enumeration.cpp \
  deps/first/Helpers/src/LinuxModules.cpp -ldl -o /tmp/elf_enumeration
box64 /tmp/elf_enumeration
# Expected: ELF enumeration PASS (exit 0).
```

Run with the same Box64 version as the server. If Box64 is only installed in
Docker, bind-mount the executable into a temporary container and use
`/usr/local/bin/box64` as its entrypoint. `--direct` reproduces the old behavior;
on the affected Box64 v0.4.3 it reports no free `dl_iterate_phdr` callback slot
and exits with SIGSEGV (139). This is a standalone probe, not a game process.

For live engine and Lua validation, also run the `Box64Smoke` mod under
`tests/lua-mods/`.
