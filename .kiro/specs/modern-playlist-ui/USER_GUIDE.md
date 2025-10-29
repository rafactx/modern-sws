# Modern Playlist UI - User Guide

## Overview

The Modern Playlist UI is an enhanced visual interface for the SWS Region Playlist feature in REAPER. It provides a modern, intuitive design specifically optimized for live performance scenarios where visual clarity and quick recognition are critical.

## Key Features

### Visual Enhancements
- **Modern Design**: Rounded corners, subtle shadows, and high-contrast colors
- **Clear Status Indicators**: Icons show playing, next, loop, and sync loss states at a glance
- **Color-Coded Items**: Different colors for playing, next, selected, and hovered items
- **Improved Readability**: Larger fonts (12pt-14pt) with proper spacing between items

### Performance Optimizations
- **Smooth Scrolling**: Maintains >30 FPS even with 100+ items in the playlist
- **Flicker-Free Updates**: Double buffering eliminates visual artifacts
- **Efficient Rendering**: Only changed items are redrawn for optimal performance

### Live Performance Features
- **Enhanced Monitoring Mode**: Large fonts (24pt/20pt) readable from a distance
- **Progress Bar**: Visual indication of position within the current region
- **Auto-Scroll**: Automatically keeps current and next items visible
- **High Contrast**: 7:1 contrast ratio in monitoring mode for maximum visibility

## Getting Started

### Enabling Modern UI

The Modern Playlist UI is enabled by default. To toggle it:

1. Open the Region Playlist window (Extensions > SWS > Region Playlist)
2. Right-click in the playlist area
3. Select "Modern UI" to toggle on/off

The preference is saved automatically and will persist across REAPER sessions.

### Basic Usage

#### Playlist View (Edition Mode)

The playlist view shows all regions in your playlist with the following information:

- **Status Icon** (left): Shows current state
  - ▶ Play icon: Currently playing region
  - ⏭ Next icon: Next region to play
  - ⚠ Warning icon: Sync loss detected

- **Region Number**: Large, bold number for quick identification

- **Region Name**: Truncated with ellipsis if too long

- **Duration**: Displayed in MM:SS format on the right

- **Loop Badge**: Shows loop count (e.g., "x3") or infinity symbol (∞) for infinite loops

#### Visual States

Items change appearance based on their state:

- **Playing**: Highlighted with accent color (blue) background
- **Next**: Secondary highlight (gray) background
- **Selected**: Selection highlight
- **Hovered**: Subtle highlight when mouse is over the item
- **Normal**: Default background color

#### Monitoring Mode

Switch to monitoring mode for large-screen display during live performances:

1. Click the "Monitor" button in the playlist window
2. The display shows:
   - **Top**: Playlist name and number
   - **Middle (Large)**: Current region with 24pt font
   - **Below**: Next region with 20pt font
   - **Bottom**: Progress bar showing position in current region

The monitoring mode uses high-contrast colors (7:1 ratio) for maximum visibility from a distance.

## Customization

### Theme Colors

The Modern Playlist UI automatically detects REAPER's theme (dark/light) and adjusts colors accordingly. You can customize colors by editing your `reaper.ini` file:

#### Dark Theme Colors

Add a `[sws_playlist_theme_dark]` section:

```ini
[sws_playlist_theme_dark]
background=0x1E1E1E
text=0xE0E0E0
current_bg=0x2D5F8D
current_text=0xFFFFFF
next_bg=0x3E3E42
next_text=0xE0E0E0
selected_bg=0x333333
selected_text=0xE0E0E0
hover_bg=0x2D2D30
border=0x3C3C3C
progress_bar=0x0078D7
warning_red=0xFF0000
accent_blue=0x0078D7
```

#### Light Theme Colors

Add a `[sws_playlist_theme_light]` section:

```ini
[sws_playlist_theme_light]
background=0xFFFFFF
text=0x1E1E1E
current_bg=0x0078D7
current_text=0xFFFFFF
next_bg=0xE0E0E0
next_text=0x1E1E1E
selected_bg=0xCCCCCC
selected_text=0x1E1E1E
hover_bg=0xF0F0F0
border=0xC8C8C8
progress_bar=0x0078D7
warning_red=0xFF0000
accent_blue=0x0078D7
```

**Color Format**: Use hexadecimal RGB format: `0xRRGGBB` or `RRGGBB`

After editing `reaper.ini`, restart REAPER or reload the theme to see changes.

### Item Height

You can adjust the height of playlist items (default: 40px):

1. Right-click in the playlist area
2. Select "Item Height" submenu
3. Choose from preset heights (32px, 40px, 48px, 56px, 64px)

Minimum height is 32px to ensure readability. Maximum is 200px.

## Keyboard Shortcuts

The Modern Playlist UI supports all standard Region Playlist keyboard shortcuts:

- **Space**: Play/Pause
- **Enter**: Play selected region
- **Up/Down Arrows**: Navigate regions
- **Delete**: Remove selected region
- **Ctrl+A**: Select all regions
- **Ctrl+C/V**: Copy/Paste regions

## Tips for Live Performance

### Optimal Setup

1. **Use Monitoring Mode**: Switch to monitoring mode for large-screen display
2. **Position Window**: Place on a secondary monitor visible from your performance position
3. **Adjust Size**: Make the window as large as possible for maximum visibility
4. **Test Distance**: Verify you can read the text from your performance position

### Best Practices

- **Keep Playlists Organized**: Use clear, descriptive region names
- **Use Loop Counts**: Set appropriate loop counts to avoid manual intervention
- **Test Sync**: Verify sync is working before the performance
- **Monitor Progress**: Watch the progress bar to anticipate transitions
- **Auto-Scroll**: Let the auto-scroll feature keep current items visible

### Troubleshooting

#### Sync Loss Warning

If you see the red warning icon (⚠):

1. Check that playback is at the correct position
2. Verify the playlist is still active
3. Click the region to resync if needed

#### Performance Issues

If scrolling is slow or choppy:

1. Reduce the number of items in the playlist (split into multiple playlists)
2. Close other resource-intensive plugins
3. Disable animations if needed (future feature)

#### Display Issues

If colors or fonts look wrong:

1. Check REAPER's theme is properly loaded
2. Verify custom theme colors in `reaper.ini` are valid
3. Try disabling and re-enabling Modern UI
4. Restart REAPER if issues persist

## Platform-Specific Notes

### Windows

- **High DPI Support**: Automatically scales fonts for High DPI displays
- **ClearType**: Uses ClearType font rendering for smooth text
- **Font**: Segoe UI (modern Windows system font)

### macOS

- **Retina Support**: Automatically handles 2x scaling for Retina displays
- **Antialiasing**: Uses Core Graphics for smooth rendering
- **Font**: Helvetica Neue (native macOS font)

### Linux

- **Window Managers**: Compatible with GNOME, KDE, XFCE, and others
- **Font Rendering**: Uses Cairo for high-quality text rendering
- **Font**: Liberation Sans (common Linux font with good fallback support)

## Accessibility

The Modern Playlist UI follows WCAG 2.0 accessibility guidelines:

- **Contrast Ratios**:
  - Current item: 4.5:1 minimum (Level AA)
  - Next item: 3:1 minimum
  - Monitoring mode: 7:1 minimum (Level AAA)

- **Font Sizes**:
  - Minimum 11pt for time display
  - Minimum 12pt for region names
  - Minimum 14pt for region numbers
  - 24pt/20pt for monitoring mode

- **Visual Indicators**: Icons supplement color coding for color-blind users

## Technical Details

### Performance Targets

The Modern Playlist UI is designed to meet these performance targets:

- **Frame Rate**: >30 FPS during scrolling with 100+ items
- **Update Latency**: <100ms for visual updates
- **Resize Latency**: <200ms for window resize reflow
- **Monitoring Updates**: ≤100ms intervals during playback

### System Requirements

- **REAPER**: Version 6.0 or later
- **SWS Extension**: Latest version
- **Operating System**:
  - Windows 7 or later
  - macOS 10.9 or later
  - Linux with GTK 2.0 or 3.0

### Memory Usage

- **Base**: ~1-2 MB for UI components
- **Per Item**: ~100 bytes per playlist item
- **Icons**: ~10 KB cached icons (all sizes)
- **Fonts**: ~50 KB cached fonts

## Frequently Asked Questions

### Q: Can I use the old playlist UI?

**A**: Yes! You can toggle Modern UI off in the right-click menu to use the classic interface.

### Q: Will my custom colors work with the Modern UI?

**A**: Yes, you can customize all colors via `reaper.ini` as described in the Customization section.

### Q: Does the Modern UI work with all REAPER themes?

**A**: Yes, it automatically detects dark/light themes and adjusts colors accordingly.

### Q: Can I adjust font sizes?

**A**: Font sizes are currently fixed to ensure readability. Future versions may add customization.

### Q: Does the Modern UI affect performance?

**A**: No, it's optimized to maintain >30 FPS even with 100+ items. It may actually perform better than the classic UI due to optimizations.

### Q: Can I use the Modern UI with multiple playlists?

**A**: Yes, each playlist window can independently enable/disable Modern UI.

### Q: Does the Modern UI support drag-and-drop?

**A**: Yes, you can drag items to reorder them. Visual feedback shows the drop target.

### Q: What happens if I have very long region names?

**A**: Long names are automatically truncated with ellipsis (...) to fit the available space.

### Q: Can I export my custom theme colors?

**A**: Yes, just copy the `[sws_playlist_theme_dark]` and `[sws_playlist_theme_light]` sections from your `reaper.ini` file.

### Q: Does the Modern UI work with ReaScript?

**A**: Yes, all existing ReaScript functions for Region Playlist continue to work normally.

## Support and Feedback

For issues, questions, or feature requests:

1. Check the SWS Extension forum: https://forum.cockos.com/forumdisplay.php?f=6
2. Report bugs on GitHub: https://github.com/reaper-oss/sws
3. Join the REAPER Discord community

## Version History

### Version 1.0 (Current)
- Initial release of Modern Playlist UI
- Dark/light theme support
- Enhanced monitoring mode
- Performance optimizations
- Platform-specific adjustments (Windows, macOS, Linux)
- Comprehensive error handling
- Double buffering for flicker-free rendering
- Dirty region tracking for efficient updates
- Icon caching for performance
- Font caching for performance

## Credits

The Modern Playlist UI was developed as part of the SWS Extension project.

- **Original SWS Extension**: Jeffos and the SWS team
- **Modern Playlist UI**: Developed following the SWS Extension architecture
- **Testing**: Community contributors

## License

The Modern Playlist UI is part of the SWS Extension and is licensed under the MIT License.

See the LICENSE file in the SWS Extension repository for full details.
