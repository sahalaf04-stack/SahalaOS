🖥️ SahalaOS v0.2

<p align="center">
  <strong>A Custom 32-bit Operating System Built from Scratch</strong>
</p><p align="center">
  Built using C, x86 Assembly, GRUB and QEMU.
</p>---

🌟 About

SahalaOS is a custom 32-bit x86 operating system developed from scratch as a low-level systems programming project.

SahalaOS v0.2 focuses on building the core operating-system environment, including:

- 🖥️ VGA text-mode display
- ⌨️ PS/2 keyboard input
- 🐚 Interactive command shell
- 🧠 Memory management
- ⚙️ Process management
- 💾 Persistent filesystem
- 💿 ATA disk support
- ⏱️ Timer support
- 🕐 RTC date/time support
- 🛡️ GDT and interrupt handling
- 🚀 GRUB-based booting
- 🧪 QEMU virtual-machine support

---

🚀 Current Version

SahalaOS v0.2

Architecture

CPU Architecture : x86 / i386
Mode             : 32-bit Protected Mode
Kernel Language  : C
Low-level Code   : x86 Assembly
Bootloader       : GRUB / Multiboot
Testing          : QEMU

---

🖥️ SahalaOS Shell

After booting, SahalaOS provides an interactive terminal:

========================================
           SAHALA OS v0.2
========================================

Welcome to SahalaOS!
Type 'help' to see available commands.

SahalaOS:/$

Available Commands

Command| Description
"help"| Display available commands
"about"| Display information about SahalaOS
"version"| Display OS version
"uname"| Display system information
"whoami"| Display current user
"clear"| Clear the screen
"pwd"| Display current directory
"ls"| List files
"mem"| Display memory information
"ps"| Display running processes

---

🏗️ Project Structure

SahalaOS/
│
├── boot/
│   └── boot.asm
│
├── include/
│   └── kernel.h
│
├── kernel/
│   ├── kernel.c
│   ├── kernel_entry.asm
│   ├── shell.c
│   ├── memory.c
│   ├── process.c
│   ├── persistent_fs.c
│   ├── filesystem.c
│   ├── ata.c
│   ├── gdt.c
│   ├── gdt.asm
│   ├── interrupts.c
│   ├── interrupts.asm
│   ├── rtc.c
│   └── timer.c
│
├── iso/
│   └── boot/
│       └── grub/
│
├── linker.ld
├── Makefile
└── README.md

---

⚙️ Technologies Used

- C — Kernel and system components
- x86 Assembly — Low-level CPU and boot code
- NASM — Assembly compilation
- GCC — C compilation
- GNU LD — Kernel linking
- GRUB — Bootloader
- xorriso — ISO generation
- QEMU — Operating-system virtualization

---

🔨 Building SahalaOS

1. Clone the repository

git clone https://github.com/sahalaf04-stack/SahalaOS.git
cd SahalaOS

2. Build the operating system

make clean
make

This generates:

kernel.elf
kernel.bin
SahalaOS.iso

3. Run using QEMU

make run

---

🧪 Running Manually

You can also launch the generated ISO directly:

qemu-system-i386 -cdrom SahalaOS.iso

---

🧠 System Architecture

The basic boot flow is:

        Computer / QEMU
               │
               ▼
             GRUB
               │
               ▼
        Multiboot Header
               │
               ▼
       kernel_entry.asm
               │
               ▼
          kernel_main()
               │
       ┌───────┴────────┐
       ▼                ▼
   Hardware          Kernel
   Setup             Services
       │                │
       ├── GDT          ├── Memory
       ├── Interrupts   ├── Processes
       ├── Timer        ├── Filesystem
       └── RTC          └── Shell
                              │
                              ▼
                       SahalaOS:/$

---

📌 Version 0.2 Goals

SahalaOS v0.2 is focused on creating a functional kernel environment rather than a full desktop operating system.

Implemented

- [x] 32-bit x86 kernel
- [x] GRUB boot process
- [x] VGA text output
- [x] Keyboard input
- [x] Interactive shell
- [x] Basic command parser
- [x] Memory management
- [x] Process management
- [x] Filesystem foundation
- [x] Persistent filesystem
- [x] ATA support
- [x] GDT
- [x] Interrupt handling
- [x] Timer
- [x] RTC
- [x] QEMU testing

---

🛣️ Future Development

Possible future versions may introduce:

- [ ] Improved shell
- [ ] More filesystem commands
- [ ] Better process scheduling
- [ ] User-mode applications
- [ ] System calls
- [ ] Improved memory management
- [ ] Multitasking
- [ ] Networking
- [ ] GUI
- [ ] 64-bit architecture

---

🎓 Project Purpose

SahalaOS was created to explore how operating systems work at a low level and to gain practical experience with:

- Operating-system architecture
- Computer architecture
- Memory management
- CPU modes
- Interrupts
- Hardware interaction
- Filesystems
- Assembly programming
- Kernel development

---

👩‍💻 Author

Sahala Fathima P A

Artificial Intelligence & Data Science Student

GitHub:
https://github.com/sahalaf04-stack

---

⭐ Contributing

This project is currently maintained as a personal operating-system development project.

Suggestions, bug reports and improvements are welcome through GitHub Issues and Pull Requests.

---

📜 License

This project is provided for educational and development purposes.

---

<p align="center">
  🚀 <strong>SahalaOS v0.2 — Built from Scratch</strong>
</p>
