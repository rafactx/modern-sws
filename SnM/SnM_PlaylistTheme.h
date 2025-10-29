/******************************************************************************
/ SnM_PlaylistTheme.h
/
/ Copyright (c) 2012 and later Jeffos
/
/
/ Permission is hereby granted, free of charge, to any person obtaining a copy
/ of this software and associated documentation files (the "Software"), to deal
/ in the Software without restriction, including without limitation the rights to
/ use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
/ of the Software, and to permit persons to whom the Software is furnished to
/ do so, subject to the following conditions:
/
/ The above copyright notice and this permission notice shall be included in all
/ copies or substantial portions of the Software.
/
/ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
/ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
/ OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
/ NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
/ HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
/ WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/ FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
/ OTHER DEALINGS IN THE SOFTWARE.
/
******************************************************************************/

/******************************************************************************
 * PLAYLIST THEME SYSTEM
 *
 * Manages colors, fonts, and visual styles for the modern playlist UI.
 * Provides centralized theme management with support for:
 * - Dark and light themes
 * - Platform-specific color adjustments (Windows, macOS, Linux)
 * - Custom theme colors from reaper.ini
 * - Font caching for performance
 * - Contrast ratio validation for accessibility
 *
 * THEME DETECTION:
 * Automatically detects REAPER's current theme (dark/light) by analyzing
 * the background color luminance. Themes with luminance < 0.5 are considered
 * dark, >= 0.5 are considered light.
 *
 * PLATFORM-SPECIFIC ADJUSTMENTS:
 * - Windows: Colors adjusted for Windows 10/11 dark/light modes
 * - macOS: Colors adjusted for macOS system appearance
 * - Linux: Neutral colors for compatibility with various desktop environments
 *
 * CUSTOM THEMES:
 * Users can customize colors via reaper.ini sections:
 * - [sws_playlist_theme_dark] for dark theme colors
 * - [sws_playlist_theme_light] for light theme colors
 *
 * Color format: 0xRRGGBB or RRGGBB (hexadecimal RGB)
 *
 * FONT CACHING:
 * Fonts are cached to avoid repeated creation. Cache is invalidated when:
 * - Font sizes change
 * - Theme is reloaded
 *
 * PERFORMANCE:
 * - Singleton pattern for efficient access
 * - Font caching reduces allocation overhead
 * - Color calculations are fast (no file I/O during rendering)
 *
 * REQUIREMENTS:
 * Satisfies requirements 5.1-5.5 from the specification.
 ******************************************************************************/

#ifndef _SNM_PLAYLISTTHEME_H_
#define _SNM_PLAYLISTTHEME_H_

#include "../WDL/lice/lice.h"
#include "../WDL/wdltypes.h"

// Forward declarations
class LICE_CachedFont;

///////////////////////////////////////////////////////////////////////////////
// PlaylistTheme - Manages colors, fonts and visual styles for playlist UI
///////////////////////////////////////////////////////////////////////////////

/**
 * PlaylistTheme
 *
 * Centralized theme management system for the modern playlist UI.
 * Provides colors, fonts, and utility functions for visual styling.
 *
 * SINGLETON PATTERN:
 * Use GetInstance() to access the single shared instance.
 *
 * THREAD SAFETY:
 * Not thread-safe. All methods must be called from the main UI thread.
 *
 * ERROR HANDLING:
 * - Graceful fallback to default colors if custom theme fails to load
 * - Graceful fallback to system font if custom fonts fail to load
 * - NULL checks on all font pointers before use
 * - Debug logging for troubleshooting
 */
class PlaylistTheme {
public:
    /**
     * Colors
     *
     * Complete color scheme for the playlist UI.
     * All colors are in LICE_RGBA format (32-bit RGBA).
     *
     * COLOR REQUIREMENTS:
     * - background: Luminance < 20% for dark, > 80% for light (Req 5.1, 5.2)
     * - currentItemBg: Contrast ratio >= 4.5:1 with text (Req 1.4)
     * - nextItemBg: Contrast ratio >= 3:1 with text (Req 1.5)
     * - Monitoring colors: Contrast ratio >= 7:1 (Req 4.4)
     */
    struct Colors {
        int background;         // Main background color
        int text;               // Primary text color
        int currentItemBg;      // Background for currently playing item (Req 1.4)
        int currentItemText;    // Text color for currently playing item
        int nextItemBg;         // Background for next item (Req 1.5)
        int nextItemText;       // Text color for next item
        int selectedBg;         // Background for selected items (Req 5.3)
        int selectedText;       // Text color for selected items
        int hoverBg;            // Background for hovered items (Req 6.1)
        int border;             // Border color for highlighted items (Req 5.5)
        int progressBar;        // Progress bar fill color (Req 4.5)
        int warningRed;         // Warning/error color (Req 2.5)
        int accentBlue;         // Accent color for badges and highlights (Req 5.3)
    };

    /**
     * Fonts
     *
     * Complete font collection for the playlist UI.
     * All fonts are LICE_CachedFont pointers (may be NULL if creation failed).
     *
     * FONT REQUIREMENTS:
     * - itemName: 12pt minimum (Req 1.3)
     * - itemNumber: 14pt minimum (Req 1.2)
     * - itemTime: 11pt minimum (Req 3.1)
     * - monitorLarge: 24pt minimum (Req 4.1)
     * - monitorMedium: 20pt minimum (Req 4.2, 4.3)
     *
     * PLATFORM-SPECIFIC FONTS:
     * - Windows: Segoe UI with ClearType rendering
     * - macOS: Helvetica Neue with Retina support
     * - Linux: Liberation Sans with Cairo rendering
     *
     * ALWAYS CHECK FOR NULL before using fonts!
     */
    struct Fonts {
        LICE_CachedFont* itemName;      // 12pt - Region name (Req 1.3)
        LICE_CachedFont* itemNumber;    // 14pt - Region number (Req 1.2)
        LICE_CachedFont* itemTime;      // 11pt - Time display (Req 3.1)
        LICE_CachedFont* monitorLarge;  // 24pt - Current region in monitoring (Req 4.1)
        LICE_CachedFont* monitorMedium; // 20pt - Next region in monitoring (Req 4.2, 4.3)
    };

    /**
     * FontSizes
     *
     * Tracks font sizes for cache invalidation.
     * When sizes change, fonts are recreated.
     */
    struct FontSizes {
        int itemName;
        int itemNumber;
        int itemTime;
        int monitorLarge;
        int monitorMedium;

        FontSizes() : itemName(12), itemNumber(14), itemTime(11),
                      monitorLarge(24), monitorMedium(20) {}

        bool operator==(const FontSizes& other) const {
            return itemName == other.itemName &&
                   itemNumber == other.itemNumber &&
                   itemTime == other.itemTime &&
                   monitorLarge == other.monitorLarge &&
                   monitorMedium == other.monitorMedium;
        }

        bool operator!=(const FontSizes& other) const {
            return !(*this == other);
        }
    };

    /**
     * GetInstance - Returns the singleton instance
     * @return Pointer to the singleton instance (never NULL after first call)
     *
     * Creates the instance on first call. Thread-safe for single-threaded use.
     * Handles creation failures gracefully (returns NULL on failure).
     */
    static PlaylistTheme* GetInstance();

    // Theme management

    /**
     * LoadTheme - Loads a specific theme (dark or light)
     * @param isDark True for dark theme, false for light theme
     *
     * Loads default colors for the specified theme, then attempts to load
     * custom colors from reaper.ini. Initializes fonts with default sizes.
     *
     * Platform-specific color adjustments are applied automatically.
     *
     * REQUIREMENTS: Satisfies Req 5.1, 5.2
     */
    void LoadTheme(bool isDark);

    /**
     * UpdateTheme - Detects and applies REAPER theme changes
     *
     * Detects current REAPER theme by analyzing background color luminance.
     * Reloads colors and fonts if theme has changed.
     * Clears icon cache to regenerate icons with new colors.
     *
     * Call this when REAPER's theme changes or on window activation.
     *
     * REQUIREMENTS: Satisfies Req 5.1, 5.2
     */
    void UpdateTheme();

    // Accessors

    /**
     * GetColors - Returns the current color scheme
     * @return Reference to Colors struct (always valid)
     */
    const Colors& GetColors() const { return m_colors; }

    /**
     * GetFonts - Returns the current font collection
     * @return Reference to Fonts struct (fonts may be NULL - always check!)
     */
    const Fonts& GetFonts() const { return m_fonts; }

    /**
     * IsDark - Checks if current theme is dark
     * @return True if dark theme is active
     */
    bool IsDark() const { return m_isDark; }

    // Color utilities

    /**
     * GetHoverColor - Calculates hover color from base color
     * @param baseColor Base color in LICE_RGBA format
     * @return Hover color (lighter for dark theme, darker for light theme)
     *
     * Adjusts brightness by 15% for dark themes, -5% for light themes.
     * Used to create hover effects without defining separate colors.
     *
     * REQUIREMENTS: Satisfies Req 1.4, 1.5, 4.4, 5.3
     */
    int GetHoverColor(int baseColor) const;

    /**
     * BlendColors - Blends two colors with a ratio
     * @param color1 First color in LICE_RGBA format
     * @param color2 Second color in LICE_RGBA format
     * @param ratio Blend ratio (0.0 = color1, 1.0 = color2)
     * @return Blended color in LICE_RGBA format
     *
     * Used for smooth color transitions and animations.
     */
    int BlendColors(int color1, int color2, float ratio) const;

    /**
     * ValidateContrastRatio - Checks if colors meet contrast requirements
     * @param fg Foreground color in LICE_RGBA format
     * @param bg Background color in LICE_RGBA format
     * @param minRatio Minimum contrast ratio (e.g., 4.5, 3.0, 7.0)
     * @return True if contrast ratio meets or exceeds minimum
     *
     * Uses WCAG 2.0 relative luminance formula to calculate contrast ratio.
     * Useful for validating accessibility requirements.
     *
     * REQUIREMENTS:
     * - Req 1.4: 4.5:1 for current item
     * - Req 1.5: 3:1 for next item
     * - Req 4.4: 7:1 for monitoring mode
     */
    bool ValidateContrastRatio(int fg, int bg, float minRatio) const;

private:
    /**
     * Constructor (private for singleton pattern)
     * Initializes with default dark theme and NULL fonts.
     */
    PlaylistTheme();

    /**
     * Destructor (private for singleton pattern)
     * Cleans up all allocated fonts.
     */
    ~PlaylistTheme();

    // Prevent copying (singleton pattern)
    PlaylistTheme(const PlaylistTheme&);
    PlaylistTheme& operator=(const PlaylistTheme&);

    /**
     * LoadDefaultDarkTheme - Loads default dark theme colors
     *
     * Sets colors appropriate for dark backgrounds:
     * - Dark background (luminance < 20%)
     * - Light text for contrast
     * - Platform-specific adjustments applied
     *
     * REQUIREMENTS: Satisfies Req 5.1
     */
    void LoadDefaultDarkTheme();

    /**
     * LoadDefaultLightTheme - Loads default light theme colors
     *
     * Sets colors appropriate for light backgrounds:
     * - Light background (luminance > 80%)
     * - Dark text for contrast
     * - Platform-specific adjustments applied
     *
     * REQUIREMENTS: Satisfies Req 5.2
     */
    void LoadDefaultLightTheme();

    /**
     * LoadCustomThemeFromINI - Loads custom colors from reaper.ini
     *
     * Reads custom colors from:
     * - [sws_playlist_theme_dark] for dark theme
     * - [sws_playlist_theme_light] for light theme
     *
     * Falls back to default colors if keys are not found.
     * Color format: 0xRRGGBB or RRGGBB (hexadecimal RGB)
     */
    void LoadCustomThemeFromINI();

    /**
     * InitializeFonts - Creates fonts with default sizes
     *
     * Creates all fonts with default sizes:
     * - itemName: 12pt
     * - itemNumber: 14pt
     * - itemTime: 11pt
     * - monitorLarge: 24pt
     * - monitorMedium: 20pt
     *
     * Platform-specific font creation with DPI scaling.
     * Reuses cached fonts if sizes haven't changed.
     */
    void InitializeFonts();

    /**
     * InitializeFontsWithSizes - Creates fonts with specific sizes
     * @param sizes Font sizes to use
     *
     * Creates all fonts with the specified sizes.
     * Reuses cached fonts if sizes haven't changed (performance optimization).
     * Handles font creation failures gracefully (fonts may be NULL).
     *
     * PLATFORM-SPECIFIC:
     * - Windows: DPI scaling, ClearType rendering, Segoe UI
     * - macOS: Retina support, antialiasing, Helvetica Neue
     * - Linux: Cairo rendering, antialiasing, Liberation Sans
     */
    void InitializeFontsWithSizes(const FontSizes& sizes);

    /**
     * CleanupFonts - Deletes all allocated fonts
     *
     * Safely deletes all fonts and sets pointers to NULL.
     * Called in destructor and when fonts need to be recreated.
     * Handles exceptions gracefully (debug builds only).
     */
    void CleanupFonts();

    // Member variables
    Colors m_colors;            // Current color scheme
    Fonts m_fonts;              // Current font collection (may contain NULLs)
    FontSizes m_fontSizes;      // Current font sizes (for cache invalidation)
    bool m_isDark;              // True if dark theme is active

    static PlaylistTheme* s_instance;  // Singleton instance
};

#endif // _SNM_PLAYLISTTHEME_H_
