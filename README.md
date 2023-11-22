# asshdr

Recolorize ASS subtitle for HDR contents.

### Build Requirements

Package Manager ---- Conan version 2.0

- [pcre2](https://www.pcre.org/)

- [CLI11](https://github.com/CLIUtils/CLI11)

### How to build

```
conan install . -b missing -s build_type=Release -c tools.system.package_manager:mode=install
cmake --preset conan-default
cmake --build --preset conan-release -j
cmake --install build --prefix install
```

### How to use

```
Usage:     asshdr [options...] [<files>]
Examples:  asshdr <files>
           asshdr -i <files>
           asshdr -o <dir> -i <files>
           asshdr -b <num> -o <dir> -i <files>
Options:
  -i, --input,       <files>           Input .ass files
  -o, --output       <dir>             Output directory
                                      (Default: same directory as input)
  -b, --brightness   <num (0-1000)>    Target brightness for recoloring
                                      (Default: 203)
  -h, --help                           Get help info
 ```
