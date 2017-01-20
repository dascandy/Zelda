#pragma once

#include <string>
#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include "MmapFile.h"

class Symbol;

class Section {
private:
  virtual size_t size();
  virtual void Write(uint8_t* target, std::unordered_map<Symbol*, size_t> symbols);
};

class Symbol {
public:
  virtual Section* section() = 0;
  virtual size_t offset() = 0;
  virtual std::string name() = 0;
};

class ObjectFile {
public:
  virtual size_t symbolcount() = 0;
  virtual Symbol* getSymbol(size_t index) = 0;
  virtual Symbol* getSymbol(const std::string& name) = 0;
};

ObjectFile* LoadElfFile(std::shared_ptr<MmapFile> file, size_t offset, size_t length);

template <typename Elf>
class ElfFile : public ObjectFile {
private:
  friend ObjectFile* LoadElfFile(std::shared_ptr<MmapFile> file, size_t offset, size_t length);
  ElfFile(std::shared_ptr<MmapFile> file, size_t offset, size_t length);
private:
  std::shared_ptr<MmapFile> file;
  uint8_t *ptr;
  struct ElfSymbol : Symbol {
    ElfSymbol(typename Elf::Symbol*, ElfFile<Elf>*);
    Section* section() override;
    size_t offset() override;
    std::string name() override;
    typename Elf::Symbol* sym;
    ElfFile<Elf>* file;
  };
  std::unordered_map<typename Elf::Symbol*, ElfSymbol> symbols;
public:
  size_t symbolcount() override;
  Symbol* getSymbol(size_t index) override;
  Symbol* getSymbol(const std::string& name) override;
  size_t sectioncount();
  size_t segmentcount();
  typename Elf::ElfHeader* header();
  typename Elf::SectionHeader* section(size_t index);
  typename Elf::SectionHeader* section(const std::string& name);
  typename Elf::ProgramHeader* segment(size_t index);
  typename Elf::Symbol* symbol(size_t index);
  typename Elf::Symbol* symbol(const std::string& name);
  uint8_t *get(typename Elf::SectionHeader*);
  const char* name(size_t offset);
  const char* symbolname(size_t offset);
};
/*
template <typename Elf>
class ElfExecutable {
public:
  ElfExecutable(const std::string& name);
  ~ElfExecutable();
  typename Elf::ElfHeader* header();
  typename Elf::ProgramHeader* add_phdr(size_t size, uint32_t vaddr, bool isBss);
  uint8_t *get(typename Elf::ProgramHeader*);
private:
  std::vector<uint8_t> storage;
  std::vector<typename Elf::ProgramHeader> phdrs;
  std::string name;
};
*/

