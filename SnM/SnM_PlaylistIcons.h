/******************************************************************************
/ SnM_PlaylistIcons.h
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
 * PLAYLIST ICON MANAGER
 *
 * Manages programmatically generated icons for the modern playlist UI.
 * No external image files are required - all icons are generated using
 * LICE drawing primitives.
 *
 * KEY FEATURES:
 * - Programmatic icon generation (no file dependencies)
 * - Icon caching for performance (Task 9.1)
 * - Multiple sizes supported (16x16, 24x24, 32x32, etc.)
 * - Color customization support
 * - Graceful fallback to simple shapes if generation fails
 *
 * ICON TYPES:
 * - ICON_PLAY: Triangle pointing right (play symbol)
 * - ICON_NEXT: Double triangle (skip forward symbol)
 * - ICON_WARNING: Exclamation mark in triangle
 * - ICON_LOOP_INFINITE: Infinity symbol (∞)
 * - ICON_SYNC_LOSS: Red X in circle
 *
 * CACHING:
 * Icons are cached by type and size. Cache is cleared when:
 * - Theme changes (to regenerate with new colors)
 * - Explicitly requested via ClearCache()
 *
 * PERFORMANCE:
 * - Icon caching eliminates repeated generation (Task 9.1)
 * - Fast lookup by cache key (type << 16 | size)
 * - Minimal memory overhead (only generated sizes are cached)
 *
 * REQUIREMENTS:
 * Satisfies requirements 2.1-2.5 from the specification.
 ******************************************************************************/

#ifndef _SNM_PLAYLISTICONS_H_
#define _SNM_PLAYLISTICONS_H_

#include "../WDL/lice/lice.h"
#include "../WDL/ptrlist.h"
#include "../WDL/assocarray.h"

///////////////////////////////////////////////////////////////////////////////
// PlaylistIconManager - Manages icons for playlist UI
///////////////////////////////////////////////////////////////////////////////

/**
 * PlaylistIconManager
 *
 * Singleton manager for programmatically generated icons.
 * Provides icon generation, caching, and drawing services.
 *
 * SINGLETON PATTERN:
 * Use GetInstance() to access the single shared instance.
 * Call DestroyInstance() on shutdown to clean up resources.
 *
 * THREAD SAFETY:
 * Not thread-safe. All methods must be called from the main UI thread.
 *
 * ERROR HANDLING:
 * - Returns NULL if icon generation fails
 * - Draws simple placeholder if icon is unavailable
 * - Validates all parameters (type, size, pointers)
 * - Debug logging for troubleshooting
 */
class PlaylistIconManager {
public:
    /**
     * IconType
     *
     * Enumeration of available icon types.
     * Each icon is generated programmatically using LICE primitives.
     *
     * REQUIREMENTS:
     * - ICON_PLAY: Req 2.1 (16x16 minimum)
     * - ICON_NEXT: Req 2.2 (16x16 minimum)
     * - ICON_LOOP_INFINITE: Req 2.3 (14x14 minimum)
     * - ICON_WARNING: Req 2.5 (red color)
     * - ICON_SYNC_LOSS: Req 2.5 (red color)
     */
    enum IconType {
        ICON_PLAY,          // Triangle pointing right (play symbol)
        ICON_NEXT,          // Double triangle (skip forward symbol)
        ICON_WARNING,       // Exclamation mark in triangle
        ICON_LOOP_INFINITE, // Infinity symbol (∞)
        ICON_SYNC_LOSS,     // Red X in circle
        ICON_TYPE_COUNT     // Total number of icon types (not a valid type)
    };

    /**
     * GetInstance - Returns the singleton instance
     * @return Pointer to the singleton instance (never NULL after first call)
     *
     * Creates the instance on first call. Thread-safe for single-threaded use.
     */
    static PlaylistIconManager* GetInstance();

    /**
     * DestroyInstance - Destroys the singleton instance
     *
     * Cleans up all cached icons and frees memory.
     * Call this on plugin shutdown to prevent memory leaks.
     */
    static void DestroyInstance();

    /**
     * GetIcon - Retrieves or generates an icon
     * @param type Icon type to get
     * @param size Icon size in pixels (8-128 range, default 16)
     * @return Pointer to icon bitmap, or NULL if generation failed
     *
     * Returns cached icon if available, otherwise generates and caches it.
     * Validates type and size parameters.
     *
     * PERFORMANCE: Cached icons are returned immediately (Task 9.1)
     * REQUIREMENTS: Satisfies Req 2.1, 2.2, 2.3, 2.5
     */
    LICE_IBitmap* GetIcon(IconType type, int size = 16);

    /**
     * DrawIcon - Draws an icon to a bitmap
     * @param dest Destination bitmap (must not be NULL)
     * @param type Icon type to draw
     * @param x X coordinate
     * @param y Y coordinate
     * @param size Icon size in pixels (8-128 range, default 16)
     * @param color Optional color tint (-1 for original colors)
     *
     * Convenience method that gets the icon and blits it to the destination.
     * Falls back to simple placeholder if icon is unavailable.
     *
     * REQUIREMENTS: Satisfies Req 2.1, 2.2, 2.3, 2.5
     */
    void DrawIcon(LICE_IBitmap* dest, IconType type, int x, int y, int size = 16, int color = -1);

    /**
     * ClearCache - Clears all cached icons
     *
     * Deletes all cached icon bitmaps and frees memory.
     * Called when theme changes to regenerate icons with new colors.
     *
     * PERFORMANCE: Allows icons to be regenerated with updated colors (Task 9.1)
     */
    void ClearCache();

private:
    /**
     * Constructor (private for singleton pattern)
     * Initializes empty icon cache.
     */
    PlaylistIconManager();

    /**
     * Destructor (private for singleton pattern)
     * Clears icon cache and frees all bitmaps.
     */
    ~PlaylistIconManager();

    // Prevent copying (singleton pattern)
    PlaylistIconManager(const PlaylistIconManager&);
    PlaylistIconManager& operator=(const PlaylistIconManager&);

    // Icon generation

    /**
     * GenerateIcon - Generates an icon bitmap
     * @param type Icon type to generate
     * @param size Icon size in pixels
     * @return Pointer to generated bitmap, or NULL if generation failed
     *
     * Creates a new bitmap and calls the appropriate generation method.
     * Handles bitmap creation failures gracefully.
     */
    LICE_IBitmap* GenerateIcon(IconType type, int size);

    /**
     * GeneratePlayIcon - Generates play icon (triangle)
     * @param bm Target bitmap
     * @param size Icon size in pixels
     * @param color Icon color
     *
     * Draws a right-pointing triangle with 20% padding.
     * Includes outline for better visibility at small sizes.
     *
     * REQUIREMENTS: Satisfies Req 2.1
     */
    void GeneratePlayIcon(LICE_IBitmap* bm, int size, int color);

    /**
     * GenerateNextIcon - Generates next icon (double triangle)
     * @param bm Target bitmap
     * @param size Icon size in pixels
     * @param color Icon color
     *
     * Draws two right-pointing triangles with small gap between them.
     * Represents "skip forward" or "next" action.
     *
     * REQUIREMENTS: Satisfies Req 2.2
     */
    void GenerateNextIcon(LICE_IBitmap* bm, int size, int color);

    /**
     * GenerateWarningIcon - Generates warning icon (exclamation in triangle)
     * @param bm Target bitmap
     * @param size Icon size in pixels
     * @param color Icon color
     *
     * Draws a triangle outline with exclamation mark inside.
     * Line and dot scale with icon size.
     *
     * REQUIREMENTS: Satisfies Req 2.5
     */
    void GenerateWarningIcon(LICE_IBitmap* bm, int size, int color);

    /**
     * GenerateLoopInfiniteIcon - Generates infinity symbol
     * @param bm Target bitmap
     * @param size Icon size in pixels
     * @param color Icon color
     *
     * Draws infinity symbol (∞) using two circles with connecting lines.
     * Thicker outline for larger sizes (>= 24px).
     *
     * REQUIREMENTS: Satisfies Req 2.3
     */
    void GenerateLoopInfiniteIcon(LICE_IBitmap* bm, int size, int color);

    /**
     * GenerateSyncLossIcon - Generates sync loss icon (red X in circle)
     * @param bm Target bitmap
     * @param size Icon size in pixels
     * @param color Icon color (typically red)
     *
     * Draws a red circle with white X inside.
     * Thicker lines for larger sizes (>= 24px).
     *
     * REQUIREMENTS: Satisfies Req 2.5
     */
    void GenerateSyncLossIcon(LICE_IBitmap* bm, int size, int color);

    /**
     * GetCacheKey - Generates cache key from type and size
     * @param type Icon type
     * @param size Icon size
     * @return Cache key (type << 16 | size)
     *
     * Combines type and size into a single integer key for fast lookup.
     */
    int GetCacheKey(IconType type, int size) const;

    /**
     * CacheEntry
     *
     * Represents a cached icon bitmap.
     * - key: Cache key (type << 16 | size)
     * - bitmap: Generated icon bitmap
     */
    struct CacheEntry {
        int key;                // Cache key for lookup
        LICE_IBitmap* bitmap;   // Cached icon bitmap
    };

    WDL_PtrList<CacheEntry> m_iconCache;  // Icon cache (Task 9.1)

    static PlaylistIconManager* s_instance;  // Singleton instance
};

#endif // _SNM_PLAYLISTICONS_H_
