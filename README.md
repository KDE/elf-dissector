# ELF Dissector

Static analysis tool for ELF libraries and executables.

## Main Use-Cases

ELF Dissector is useful if you need to do one or more of the following tasks:

* Inspecting forward and backward dependencies, on library and symbol level.
* Identifying load-time performance bottlenecks such as expensive static constructors or excessive relocations.
* Size profiling of ELF files.

## Features

* ELF structure browser.
* Tree map visualization of the size of the various parts of an ELF file.
* Relocation heatmap.
* Built-in disassmbler for x86 and AArch64.
* Browser for data type memory layouts extracted from DWARF debug information.
* Forward and backward dependency viewer.

## Esoteric/experimental features

ELF Dissector also doubles as a research platform for more experimental work around ELF files and loading/dynamic linking. Some of this is only available in CLI tools. Use at your own risk.

* Identifying sub-optimal struct packing.
* Optimizing dependency order by lookup hit rate probability.
* Identifying duplicate vtable or typeinfo emission.
* Loading/dynamic linking profiler.
* Identifying unused symbols in a set of ELF files.
