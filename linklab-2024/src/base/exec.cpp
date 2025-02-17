#include "fle.hpp"
#include "string_utils.hpp"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>

void FLE_exec(const FLEObject& obj)
{
    if (obj.type != ".exe") {
        throw std::runtime_error("File is not an executable FLE.");
    }

    // Map each section
    for (const auto& phdr : obj.phdrs) {
        if (phdr.size == 0) {
            std::cerr << "Warning: section " << phdr.name << " has size 0, skipping." << std::endl;
            continue;
        }

        void* addr = mmap((void*)phdr.vaddr, phdr.size,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0);
        // ! We need to set the permissions after copying the data

        if (addr == MAP_FAILED) {
            throw std::runtime_error(std::string("mmap failed: ") + strerror(errno));
        }

        // First, copy the section data
        auto it = obj.sections.find(phdr.name);
        if (it == obj.sections.end()) {
            throw std::runtime_error("Section not found: " + phdr.name);
        }

        // BSS段不需要复制数据，因为mmap已经返回零初始化的内存
        if (phdr.name != ".bss" && !starts_with(phdr.name, ".bss.")) {
            memcpy(addr, it->second.data.data(), phdr.size);
        }

        // Then, set the final permissions
        mprotect(addr, phdr.size,
            (phdr.flags & static_cast<uint32_t>(PHF::R) ? PROT_READ : 0)
                | (phdr.flags & static_cast<uint32_t>(PHF::W) ? PROT_WRITE : 0)
                | (phdr.flags & static_cast<uint32_t>(PHF::X) ? PROT_EXEC : 0));
    }

    using FuncType = int (*)();
    FuncType func = reinterpret_cast<FuncType>(obj.entry);
    func();

    // Should not reach here, since `func` is NoReturn.
    assert(false);
}