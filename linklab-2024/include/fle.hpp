#pragma once

#ifndef FLE_HPP
#define FLE_HPP

#include "nlohmann/json.hpp"
#include <cstdint>
#include <fstream>
#include <map>
#include <string>
#include <vector>

using json = nlohmann::ordered_json;

// Relocation types
enum class RelocationType {
    R_X86_64_32, // 32-bit absolute addressing
    R_X86_64_PC32, // 32-bit PC-relative addressing
    R_X86_64_64, // 64-bit absolute addressing
    R_X86_64_32S, // 32-bit signed absolute addressing
};

// Relocation entry
struct Relocation {
    RelocationType type;
    size_t offset; // Relocation position
    std::string symbol; // Symbol to relocate
    int64_t addend; // Relocation addend
};

// Symbol types
enum class SymbolType {
    LOCAL, // Local symbol (🏷️)
    WEAK, // Weak global symbol (📎)
    GLOBAL, // Strong global symbol (📤)
    UNDEFINED // Undefined symbol
};

// Symbol entry
struct Symbol {
    SymbolType type;
    std::string section; // Section containing the symbol
    size_t offset; // Offset within section
    size_t size; // Symbol size
    std::string name; // Symbol name
};

struct FLESection {
    std::string name;
    std::vector<uint8_t> data; // Section data (stored as bytes)
    std::vector<Relocation> relocs; // Relocation table for this section
    bool has_symbols; // Whether section contains symbols
};

enum class PHF { // Program Header Flags
    X = 1, // Executable
    W = 2, // Writable
    R = 4 // Readable
};

enum class SHF { // Section Header Flags
    ALLOC = 1, // Needs memory allocation at runtime
    WRITE = 2, // Writable
    EXEC = 4, // Executable
    NOBITS = 8, // Takes no space in file (like BSS)
};

struct SectionHeader {
    std::string name; // Section name
    uint32_t type; // Section type
    uint32_t flags; // Section flags
    uint64_t addr; // Virtual address
    uint64_t offset; // File offset
    uint64_t size; // Section size
};

struct ProgramHeader {
    std::string name; // Segment name
    uint64_t vaddr; // Virtual address (64-bit)
    uint64_t size; // Segment size
    uint32_t flags; // Permissions
};

struct FLEObject {
    std::string name; // Object name
    std::string type; // ".obj" or ".exe"
    std::map<std::string, FLESection> sections; // Section name -> section data
    std::vector<Symbol> symbols; // Global symbol table
    std::vector<ProgramHeader> phdrs; // Program headers (for .exe)
    std::vector<SectionHeader> shdrs; // Section headers
    size_t entry = 0; // Entry point (for .exe)
};

class FLEWriter {
public:
    void set_type(std::string_view type)
    {
        result["type"] = type;
    }

    void begin_section(std::string_view name)
    {
        current_section = name;
        current_lines.clear();
    }
    void end_section()
    {
        result[current_section] = current_lines;
        current_section.clear();
        current_lines.clear();
    }

    void write_line(std::string line)
    {
        if (current_section.empty()) {
            throw std::runtime_error("FLEWriter: begin_section must be called before write_line");
        }
        current_lines.push_back(line);
    }

    void write_to_file(const std::string& filename)
    {
        std::ofstream out(filename);
        out << result.dump(4) << std::endl;
    }

    void write_program_headers(const std::vector<ProgramHeader>& phdrs)
    {
        json phdrs_json = json::array();
        for (const auto& phdr : phdrs) {
            json phdr_json;
            phdr_json["name"] = phdr.name;
            phdr_json["vaddr"] = phdr.vaddr;
            phdr_json["size"] = phdr.size;
            phdr_json["flags"] = phdr.flags;
            phdrs_json.push_back(phdr_json);
        }
        result["phdrs"] = phdrs_json;
    }

    void write_entry(size_t entry)
    {
        result["entry"] = entry;
    }

    void write_section_headers(const std::vector<SectionHeader>& shdrs)
    {
        json shdrs_json = json::array();
        for (const auto& shdr : shdrs) {
            json shdr_json;
            shdr_json["name"] = shdr.name;
            shdr_json["type"] = shdr.type;
            shdr_json["flags"] = shdr.flags;
            shdr_json["addr"] = shdr.addr;
            shdr_json["offset"] = shdr.offset;
            shdr_json["size"] = shdr.size;
            shdrs_json.push_back(shdr_json);
        }
        result["shdrs"] = shdrs_json;
    }

private:
    std::string current_section;
    json result;
    std::vector<std::string> current_lines;
};

// Core functions that we provide
FLEObject load_fle(const std::string& filename); // Load FLE file into memory
void FLE_cc(const std::vector<std::string>& args); // Compile source files to FLE

// Functions for students to implement
/**
 * Display the contents of an FLE object file
 * @param obj The FLE object to display
 *
 * Expected output format:
 * Section .text:
 * 0000: 55 48 89 e5 48 83 ec 10  90 48 8b 45 f8 48 89 c7
 * Labels:
 *   _start: 0x0000
 * Relocations:
 *   0x0010: helper_func - 📍
 */
void FLE_objdump(const FLEObject& obj, FLEWriter& writer);

/**
 * Display the symbol table of an FLE object
 * @param obj The FLE object to analyze
 *
 * Expected output format:
 * 0000000000000000 T _start
 * 0000000000000020 t helper_func
 * 0000000000001000 D data_var
 */
void FLE_nm(const FLEObject& obj);

/**
 * Execute an FLE executable file
 * @param obj The FLE executable object
 * @throws runtime_error if the file is not executable or _start symbol is not found
 */
void FLE_exec(const FLEObject& obj);

/**
 * Link multiple FLE objects into an executable
 * @param objects Vector of FLE objects to link
 * @return A new FLE object of type ".exe"
 *
 * The linker should:
 * 1. Merge all sections with the same name
 * 2. Resolve symbols according to their binding:
 *    - Multiple strong symbols with same name: error
 *    - Strong and weak symbols: use strong
 *    - Multiple weak symbols: use first one
 * 3. Process relocations
 */
FLEObject FLE_ld(const std::vector<FLEObject>& objects);

/**
 * Read FLE object file
 * @param obj The FLE object to read
 */
void FLE_readfle(const FLEObject& obj);

/**
 * Disassemble data from specified section
 * @param obj The FLE object
 * @param section_name Name of section to disassemble
 *
 * Example output format:
 * 0000: 55                    push    rbp
 * 0001: 48 89 e5             mov     rbp, rsp
 * 0004: 48 83 ec 10          sub     rsp, 0x10
 */
void FLE_disasm(const FLEObject& obj, const std::string& section_name);

#endif