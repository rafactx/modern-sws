# Modern Playlist UI - Testing Guide

## Prerequisites

Before you can test the Modern Playlist UI, you need to compile the SWS extension. Here's what you need:

### Required Software

1. **CMake** - Build system generator
2. **PHP** - Required for generating ReaScript files
3. **Perl** - Required for generating Python bindings (optional)
4. **Xcode Command Line Tools** - C++ compiler for macOS

### Installation on macOS

```bash
# Install Homebrew if you don't have it
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake php

# Install Xcode Command Line Tools (if not already installed)
xcode-select --install
```

## Building the Extension

### Step 1: Create Build Directory

```bash
# Navigate to the SWS repository root
cd /path/to/sws

# Create a build directory
mkdir -p build
cd build
```

### Step 2: Configure with CMake

```bash
# Configure the build (macOS)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Alternative: Configure for debug build (includes debug logging)
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

**Note**: Use `Release` for production testing, `Debug` if you need to see debug output.

### Step 3: Compile

```bash
# Build the extension (use all CPU cores)
cmake --build . --config Release -j$(sysctl -n hw.ncpu)

# This will take a few minutes on first build
```

### Step 4: Install to REAPER

```bash
# Install to REAPER's UserPlugins directory
cmake --install .

# Default location on macOS:
# ~/Library/Application Support/REAPER/UserPlugins/
```

## Testing the Modern Playlist UI

### 1. Launch REAPER

```bash
# Open REAPER
open -a REAPER
```

### 2. Verify SWS Extension is Loaded

1. In REAPER, go to **Extensions** menu
2. You should see **SWS Extension** submenu
3. Check **Extensions > SWS > About SWS** to verify version

### 3. Open Region Playlist

1. Go to **Extensions > SWS > Region Playlist**
2. The Region Playlist window should open

### 4. Create Test Regions

To test the Modern Playlist UI, you need some regions:

1. Create a new project in REAPER
2. Add some markers/regions:
   - Press `Shift+M` to add a marker at current position
   - Or use **Insert > Region from time selection**
3. Create at least 5-10 regions for testing

Example regions:
- Region 1: "Intro" (0:00 - 0:10)
- Region 2: "Verse 1" (0:10 - 0:30)
- Region 3: "Chorus" (0:30 - 0:50)
- Region 4: "Verse 2" (0:50 - 1:10)
- Region 5: "Outro" (1:10 - 1:30)

### 5. Add Regions to Playlist

1. In the Region Playlist window, click **Add**
2. Select regions to add to the playlist
3. You should now see the regions listed with the Modern UI

### 6. Test Modern UI Features

#### Visual Features to Test:

✅ **Item Appearance**
- [ ] Rounded corners (4px radius)
- [ ] Proper spacing between items (4px)
- [ ] Clear fonts (12pt name, 14pt number, 11pt time)
- [ ] Time displayed in MM:SS format

✅ **Status Indicators**
- [ ] Play icon (▶) appears on currently playing region
- [ ] Next icon (⏭) appears on next region
- [ ] Loop badge shows count (e.g., "x3") or infinity (∞)

✅ **Interactive Features**
- [ ] Hover effect when mouse over items
- [ ] Selection highlight when clicking items
- [ ] Drag and drop to reorder items
- [ ] Double-click to play region

✅ **Playback Testing**
- [ ] Click **Play** button to start playlist
- [ ] Current item highlights with blue background
- [ ] Next item highlights with gray background
- [ ] Auto-scroll keeps current item visible
- [ ] Progress updates smoothly

✅ **Monitoring Mode**
- [ ] Click **Monitor** button
- [ ] Large fonts (24pt current, 20pt next)
- [ ] Progress bar shows position
- [ ] High contrast colors
- [ ] Readable from distance

✅ **Theme Testing**
- [ ] Switch REAPER theme (Options > Themes)
- [ ] Modern UI adapts to dark/light theme
- [ ] Colors remain readable
- [ ] Contrast ratios maintained

✅ **Performance Testing**
- [ ] Add 100+ regions to playlist
- [ ] Scroll should be smooth (>30 FPS)
- [ ] No flickering during updates
- [ ] Hover effects respond quickly

### 7. Test Loop Functionality

1. Right-click a region in the playlist
2. Set loop count (e.g., 3 times)
3. Play the playlist
4. Verify loop badge appears ("x3")
5. Test infinite loop (set count to -1)
6. Verify infinity symbol (∞) appears

### 8. Test Customization

#### Custom Colors (Optional)

1. Close REAPER
2. Edit `~/Library/Application Support/REAPER/reaper.ini`
3. Add custom theme section:

```ini
[sws_playlist_theme_dark]
background=0x1E1E1E
text=0xE0E0E0
current_bg=0x2D5F8D
accent_blue=0x0078D7
```

4. Reopen REAPER
5. Verify custom colors are applied

#### Toggle Modern UI

1. Right-click in playlist area
2. Select "Modern UI" to toggle on/off
3. Verify it switches between modern and classic rendering

## Troubleshooting

### Build Errors

**Error: "cmake: command not found"**
```bash
brew install cmake
```

**Error: "PHP not found"**
```bash
brew install php
```

**Error: "No CMAKE_CXX_COMPILER could be found"**
```bash
xcode-select --install
```

**Error: "TagLib not found"**
```bash
# The build should download TagLib automatically
# If it fails, try:
cd build
rm -rf *
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Runtime Issues

**Extension doesn't load in REAPER**
- Check REAPER version (6.0 or later required)
- Verify file is in correct location:
  - macOS: `~/Library/Application Support/REAPER/UserPlugins/`
  - Windows: `%APPDATA%\REAPER\UserPlugins\`
  - Linux: `~/.config/REAPER/UserPlugins/`

**Modern UI doesn't appear**
- Verify you're using the Region Playlist (not Marker List)
- Check if Modern UI is enabled (right-click menu)
- Try toggling Modern UI off and on

**Performance issues**
- Reduce number of items in playlist
- Close other resource-intensive plugins
- Try Release build instead of Debug build

**Colors look wrong**
- Check REAPER theme is properly loaded
- Verify custom colors in reaper.ini are valid hex
- Try deleting custom theme section to use defaults

### Debug Build

If you need to see debug output:

```bash
# Build in Debug mode
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
cmake --install .

# On macOS, view debug output:
log stream --predicate 'process == "REAPER"' --level debug
```

## Performance Benchmarks

Expected performance on modern hardware:

- **Frame Rate**: >30 FPS with 100+ items
- **Update Latency**: <100ms for visual updates
- **Scroll Latency**: <16ms per frame (60 FPS)
- **Memory Usage**: ~1-2 MB base + ~100 bytes per item

Test with Activity Monitor (macOS) or Task Manager (Windows) to verify.

## Automated Testing

Currently, the Modern Playlist UI doesn't have automated tests. Manual testing is required.

Future improvements could include:
- Unit tests for rendering logic
- Integration tests for playlist operations
- Performance benchmarks
- Visual regression tests

## Reporting Issues

If you find bugs or issues:

1. **Check existing issues**: https://github.com/reaper-oss/sws/issues
2. **Gather information**:
   - REAPER version
   - SWS version
   - Operating system and version
   - Steps to reproduce
   - Screenshots if applicable
3. **Create detailed bug report**:
   - What you expected to happen
   - What actually happened
   - Debug output (if available)

## Quick Test Checklist

Use this checklist for quick verification:

- [ ] Extension compiles without errors
- [ ] Extension loads in REAPER
- [ ] Region Playlist window opens
- [ ] Modern UI is enabled by default
- [ ] Items display with rounded corners
- [ ] Status icons appear correctly
- [ ] Hover effects work
- [ ] Playback highlights current item
- [ ] Monitoring mode displays large fonts
- [ ] Performance is smooth with 50+ items
- [ ] Theme switching works
- [ ] No crashes or errors

## Next Steps

After successful testing:

1. **Test on different platforms** (Windows, Linux if available)
2. **Test with different REAPER themes**
3. **Test with large playlists** (100+ items)
4. **Test in live performance scenario**
5. **Gather user feedback**
6. **Document any issues found**

## Additional Resources

- **SWS Extension Forum**: https://forum.cockos.com/forumdisplay.php?f=6
- **REAPER Forum**: https://forum.cockos.com/
- **SWS GitHub**: https://github.com/reaper-oss/sws
- **User Guide**: See USER_GUIDE.md in this directory

## Contact

For questions or help with testing:
- SWS Extension forum
- REAPER Discord community
- GitHub issues
