# CodeCave Finder

A simple tool to find code caves in PE files. Learning about Windows internals and PE structure. Uses native NT APIs and inline ASM because why not 😎

## Flow

- Scans PE files for empty spaces
- Shows cave locations and sizes in each section
- Uses inline ASM for byte checking
- NT APIs