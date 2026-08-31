# Two-Pass Assembler in C

A modular, cross-platform **Two-Pass Assembler** written in ANSI C.  
The assembler processes assembly source files (`.as`), parses syntax, expands macros, and translates the assembly instructions into binary machine code, outputting memory images and symbol linkage tables.

---

## 📌 Project Overview

The assembler translates assembly code into machine code using a multi-stage architecture:

1. **Pre-assembler (Macro Expansion):** Scans the source file, identifies macro definitions, and expands macro calls into the intermediate `.am` file.
2. **First Pass:** Analyzes instructions, validates syntax, encodes instruction/data images, and populates the **Symbol Table** with labels, addresses, and symbol attributes.
3. **Second Pass:** Resolves label operand references using the Symbol Table, completes the binary encoding, and generates output files if no syntax/semantic errors are detected.

---

## 📁 Output Files

If the assembly process finishes without errors, the assembler generates up to four output files per source:

| Extension | Description |
| :--- | :--- |
| `.am` | Source code after **Macro Expansion** (pre-assembler stage). |
| `.ob` | The final **Object File** containing machine code and memory addresses. |
| `.ent` | **Entry Symbols** table (symbol names and their defined memory addresses). Generated only if `.entry` directives exist. |
| `.ext` | **External References** table (external symbols and the memory addresses where they are referenced). Generated only if `.extern` directives are used. |

---

## 🏗 Project Structure

The project follows a clean, modular architecture separating compilation phases, data structures, and utilities:

* `main.c` – Program entry point and file processing pipeline.
* `preprocessor.c` / `preprocessor.h` – Macro definition, parsing, and expansion (`.as` -> `.am`).
* `first_pass.c` / `first_pass.h` – First pass logic, parsing, and instruction encoding.
* `second_pass.c` / `second_pass.h` – Second pass logic, address resolution, and file generation.
* `symbol_table.c` / `symbol_table.h` – Dynamic symbol table ADT and lookup operations.
* `utils.c` / `utils.h` – Helper functions (string manipulation, memory handling, error reporting).
* `global.c` / `global.h` – Global constants, type definitions, and initialization structures.
* `Makefile` – Build automation script.

---

## 🚀 Getting Started

### Prerequisites
* A C compiler (e.g., `gcc` with ANSI C support)
* `make` utility

### Compilation
To compile the project with standard strict flags (`-Wall -ansi -pedantic`), run:
`make`

To clean build artifacts (`.o` files and executables):
`make clean`

---

## 💻 Usage

Run the compiled `assembler` executable followed by one or more input file names (including the `.as` extension):
`./assembler file1.as file2.as file3.as`

### Example
Given an input file named `input1.as`:
`./assembler input1.as`

If successful, the following files will be created:
* `input1.am`
* `input1.ob`
* `input1.ent` (if entry labels exist)
* `input1.ext` (if extern labels exist)

---

## 🛠 Key Features & Implementation Details

* **Dynamic Data Structures:** Dynamic memory allocation for symbol tables and macro registries to handle variable-sized inputs without fixed buffers.
* **Robust Error Handling:** Validates syntax, operand counts, and invalid addressing modes per line with descriptive error messages.
* **Strict Memory Management:** All dynamically allocated memory is cleanly freed upon completion or error recovery.
* **Modular Codebase:** High cohesion and loose coupling between compiler passes.
