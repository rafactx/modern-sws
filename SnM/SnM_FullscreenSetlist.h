/******************************************************************************
/ SnM_FullscreenSetlist.h
/
/ Copyright (c) 2025 and later SWS Extension Authors
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

#ifndef _SNM_FULLSCREENSETLIST_H_
#define _SNM_FULLSCREENSETLIST_H_

#include "SnM.h"

// Forward declarations
class FullscreenSetlistWindow;
class NowPlayingPanel;
class SetlistView;
class SetlistItemRenderer;
class TransportPanel;
class KeyboardController;
class RegionPlaylist;

///////////////////////////////////////////////////////////////////////////////
// FullscreenTheme - Theme system for fullscreen setlist
///////////////////////////////////////////////////////////////////////////////

class FullscreenTheme {
public:
    // Color definitions for dark theme optimized for live shows
    struct Colors {
        int background;          // Very dark (< 10% luminance)
        int text;                // Light gray (90% luminance)
        int textDimmed;          // Medium gray (60% luminance)
        int currentItemBg;       // Accent color (40-60% luminance)
        int currentItemText;     // White
        int nextItemBg;          // Subtle highlight
        int nextItemText;        // Light gray
        int selectedBorder;      // Bright accent
        int progressBarFill;     // Accent color
        int progressBarBg;       // Dark gray
        int buttonNormal;        // Medium gray
        int buttonHover;         // Lighter gray
        int buttonPressed;       // Accent color
        int buttonText;          // White
    };

    // Font definitions for large, readable text
    struct Fonts {
        LICE_CachedFont* songNameLarge;    // 32pt - setlist items
        LICE_CachedFont* songNameHuge;     // 40pt - now playing
        LICE_CachedFont* numberLarge;      // 28pt - song numbers
        LICE_CachedFont* timeMedium;       // 24pt - now playing time
        LICE_CachedFont* timeSmall;        // 20pt - item duration
        LICE_CachedFont* infoText;         // 18pt - general info
    };

    // Singleton access
    static FullscreenTheme* GetInstance();

    // Accessors
    const Colors& GetColors() const { return m_colors; }
    const Fonts& GetFonts() const { return m_fonts; }

    // Initialize theme
    void LoadTheme();

    // Cleanup
    ~FullscreenTheme();

private:
    FullscreenTheme();
    FullscreenTheme(const FullscreenTheme&) = delete;
    FullscreenTheme& operator=(const FullscreenTheme&) = delete;

    Colors m_colors;
    Fonts m_fonts;

    void InitializeColors();
    void InitializeFonts();
    void CleanupFonts();
};

// Initialize fullscreen setlist system
int FullscreenSetlistInit();

// Cleanup fullscreen setlist system
void FullscreenSetlistExit();

// Open/toggle fullscreen setlist window
void OpenFullscreenSetlist(COMMAND_T*);

// Check if fullscreen setlist is displayed
int IsFullscreenSetlistDisplayed(COMMAND_T*);

#endif // _SNM_FULLSCREENSETLIST_H_
