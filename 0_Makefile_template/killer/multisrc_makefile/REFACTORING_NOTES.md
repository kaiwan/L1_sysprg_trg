# Makefile Refactoring: Single to Multi-Source Support

## Overview
The Makefile has been refactored to support building C projects with **multiple source files** instead of just a single source file.

## Key Changes

### 1. **Variable Names**
- **Old:** `FNAME_C := killer` (single file name)
- **New:** 
  - `PROG_NAME := killer` (executable name)
  - `SRCS := killer.c` (list of source files)

### 2. **Object File Generation**
- Automatically generates object file names from source files using Make pattern rules
- `OBJS := $(SRCS:.c=.o)` — converts `main.c helper.c` → `main.o helper.o`

### 3. **Compilation Rules**
Added implicit pattern rules for different build types:

```makefile
%.o:        %.c        # Production object files
%.dbg.o:    %.c        # Debug object files
%.asan.o:   %.c        # AddressSanitizer
%.ub.o:     %.c        # UndefinedBehavior Sanitizer
%.lsan.o:   %.c        # LeakSanitizer
%.msan.o:   %.c        # MemorySanitizer
%.tsan.o:   %.c        # ThreadSanitizer
%.gcov.o:   %.c        # Code coverage
```

Each rule compiles with the appropriate compiler flags.

### 4. **Object File Lists**
For each build variant, object file lists are automatically generated:

```makefile
OBJS_DBG   := $(SRCS:.c=.dbg.o)
OBJS_ASAN  := $(SRCS:.c=.asan.o)
OBJS_UB    := $(SRCS:.c=.ub.o)
# ... and so on
```

### 5. **Target Dependencies**
All targets now depend on the appropriate object files instead of source files:

- `prod: ${OBJS}` — compiles and links all production objects
- `debug: ${OBJS_DBG} ${OBJS_ASAN} ${OBJS_UB} ...` — creates all debug variants
- `prod_2part: ${OBJS}` — for the 2-part debug linking solution

### 6. **Linking**
Instead of compiling directly:

**Before:**
```makefile
${CC} ${CFLAGS} killer.c -o killer ${LDFLAGS}
```

**After:**
```makefile
${CC} ${CFLAGS} ${OBJS} -o ${PROG_NAME} ${LDFLAGS}
```

## Usage

### For a Single Source File (default)
```makefile
PROG_NAME := myapp
SRCS := main.c
```

### For Multiple Source Files
```makefile
PROG_NAME := myapp
SRCS := main.c helper.c utils.c logger.c
```

### For Source Files in Subdirectories
```makefile
PROG_NAME := myapp
SRCS := src/main.c src/helper.c lib/utils.c lib/logger.c
```

The Makefile automatically handles the compilation of all source files, generating object files in their respective directories.

## Benefits

1. **Modularity** — Organize code into multiple files by function/purpose
2. **Parallel Compilation** — Each source file compiles independently (faster on multi-core systems)
3. **Partial Rebuilds** — Only modified files are recompiled
4. **Clean Separation** — Object files use suffixes (`.dbg.o`, `.asan.o`, etc.) to separate build variants
5. **Backward Compatible** — Works with single or multiple source files

## Cleanup
The `clean` target now removes all object file variants:
```makefile
rm -vf *.o *.dbg.o *.asan.o *.ub.o *.lsan.o *.msan.o *.tsan.o *.gcov.o
```

## Notes

- Object files are kept in the same directory as source files for simplicity
- For complex multi-directory projects with many files, consider using dependency generation (`-M` flags) or a build system like CMake
- All existing targets (code-style, static analysis, dynamic analysis, coverage) continue to work unchanged
