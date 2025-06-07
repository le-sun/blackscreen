# Black Screen Application

A lightweight Windows system tray application that provides an instant full-screen black overlay for OLED monitor, privacy, or screen breaks.

## Features

- 🖥️ **Full-screen black overlay** - Covers entire screen instantly
- ⌨️ **Global hotkey** - Press F14 from anywhere to toggle
- 🔧 **System tray integration** - Runs quietly in background
- 🎯 **Multiple exit methods** - ESC key, F14, or tray menu

## Quick Start

0. Run **blackscreen.exe** to have it running in the background
1. Press **F14** to show/hide the black screen
2. Press **ESC** or **F14** to hide the black screen
3. Right-click the system tray icon for menu options
4. Select "Exit" from tray menu to quit

## Compilation

### Prerequisites

- **MinGW-w64** cross-compiler for Windows (For Linux based systems)
- **windres** (Windows Resource Compiler)

### Build Instructions

```bash
# 1. Compile the resource file
x86_64-w64-mingw32-windres resource.rc -o resource.o

# 2. Compile the main application
x86_64-w64-mingw32-gcc app.c resource.o -o blackscreen.exe -lgdi32 -luser32 -lshell32 -mwindows -O2
```

### Required Files

- `app.c` - Main application source
- `resource.rc` - Resource definition file
- `resource.h` - Resource header file
- `icon.ico` - Custom icon file (optional)

## Usage

### Starting the Application

1. Run `blackscreen.exe`
2. The application will start minimized to system tray
3. Look for the icon in your system tray (bottom-right corner)

### Controls

| Action | Method |
|--------|--------|
| **Toggle Black Screen** | Press F14 key |
| **Hide Black Screen** | Press ESC or F14 key (when visible) |
| **Show Context Menu** | Right-click tray icon |
| **Exit Application** | Select "Exit" from tray menu |

### System Tray Menu

Right-click the tray icon to access:
- **Toggle Black Screen** - Same as F14 hotkey
- **Exit** - Quit the application

## Customization

### Changing the Hotkey

Edit `app.c` and modify this line:
```c
RegisterHotKey(hwnd, HOTKEY_ID, 0, VK_F14);
```

Common alternatives:
- `VK_F13` - F13 key
- `VK_PAUSE` - Pause/Break key
- `MOD_CONTROL | MOD_ALT, 'B'` - Ctrl+Alt+B

### Custom Icon

Replace `icon.ico` with your own icon file and rebuild.

### Tooltip Text

Edit the tooltip in `app.c`:
```c
strcpy(nid.szTip, "Your Custom Text Here");
```

**Tip**: Add `blackscreen.exe` to your Windows startup folder to have it available immediately when you boot your computer. 