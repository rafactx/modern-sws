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

///////////////////////////////////////////////////////////////////////////////
// SetlistView - Scrollable list of setlist items
///////////////////////////////////////////////////////////////////////////////

class SetlistView {
public:
    // SetlistItem structure
    struct SetlistItem {
        int number;              // Song number in setlist
        char name[256];          // Song name
        double duration;         // Duration in seconds
        bool isPlaying;          // Currently playing
        bool isNext;             // Next to play
        bool isSelected;         // Selected by user
        int regionIndex;         // REAPER region index
    };

    SetlistView(RECT bounds);
    ~SetlistView();

    // Data management
    void SetItems(const WDL_TypedBuf<SetlistItem>& items);
    int GetItemCount() const { return m_items.GetSize(); }
    const SetlistItem* GetItem(int index) const;

    // Selection
    void SetSelectedIndex(int index);
    int GetSelectedIndex() const { return m_selectedIndex; }

    // Scrolling
    void ScrollToIndex(int index);
    void ScrollBy(int delta);
    void EnsureVisible(int index);
    int GetScrollOffset() const { return m_scrollOffset; }

    // Hit testing
    int GetItemAtPoint(int x, int y) const;

    // Layout
    void SetBounds(RECT bounds);
    RECT GetBounds() const { return m_bounds; }

    // Rendering
    void Draw(LICE_IBitmap* bm, FullscreenTheme* theme);

private:
    RECT m_bounds;
    WDL_TypedBuf<SetlistItem> m_items;
    int m_selectedIndex;
    int m_scrollOffset;          // Scroll position in pixels
    int m_itemHeight;            // Height of each item in pixels

    void CalculateLayout();
    void DrawScrollbar(LICE_IBitmap* bm, FullscreenTheme* theme);
};

///////////////////////////////////////////////////////////////////////////////
// KeyboardController - Handles keyboard input
///////////////////////////////////////////////////////////////////////////////

class KeyboardController {
public:
    // Action enum
    enum Action {
        ACTION_NONE = 0,
        ACTION_SELECT_NEXT,
        ACTION_SELECT_PREVIOUS,
        ACTION_PLAY_SELECTED,
        ACTION_TOGGLE_PLAY_PAUSE,
        ACTION_EXIT_FULLSCREEN,
        ACTION_PAGE_UP,
        ACTION_PAGE_DOWN,
        ACTION_JUMP_FIRST,
        ACTION_JUMP_LAST,
        ACTION_JUMP_TO_NUMBER,
        ACTION_PLAY_NEXT,
        ACTION_PLAY_PREVIOUS
    };

    KeyboardController();
    ~KeyboardController();

    // Key processing
    Action ProcessKey(WPARAM key, LPARAM lParam);
    Action MapKeyToAction(WPARAM key);

    // Number buffer for jump-to-number
    void SetNumberBuffer(int number) { m_numberBuffer = number; }
    int GetNumberBuffer() const { return m_numberBuffer; }
    void ClearNumberBuffer() { m_numberBuffer = 0; }

private:
    int m_numberBuffer;
};

///////////////////////////////////////////////////////////////////////////////
// TransportPanel - Transport controls at bottom
///////////////////////////////////////////////////////////////////////////////

class TransportPanel {
public:
    // Button enum
    enum Button {
        BTN_PREVIOUS = 0,
        BTN_PLAY_STOP,
        BTN_NEXT,
        BTN_COUNT
    };

    TransportPanel(RECT bounds);
    ~TransportPanel();

    // Layout
    void SetBounds(RECT bounds);
    RECT GetBounds() const { return m_bounds; }

    // State
    void SetIsPlaying(bool isPlaying) { m_isPlaying = isPlaying; }
    void SetTotalDuration(double duration) { m_totalDuration = duration; }
    void SetHoveredButton(Button btn) { m_hoveredButton = btn; }
    void SetPressedButton(Button btn) { m_pressedButton = btn; }

    // Hit testing
    Button GetButtonAtPoint(int x, int y) const;

    // Rendering
    void Draw(LICE_IBitmap* bm, FullscreenTheme* theme);

private:
    RECT m_bounds;
    bool m_isPlaying;
    double m_totalDuration;
    Button m_hoveredButton;
    Button m_pressedButton;
    RECT m_buttonRects[BTN_COUNT];

    void CalculateLayout();
    void DrawButton(LICE_IBitmap* bm, Button btn, FullscreenTheme* theme);
    void DrawTotalTime(LICE_IBitmap* bm, FullscreenTheme* theme);
};

///////////////////////////////////////////////////////////////////////////////
// NowPlayingPanel - Shows currently playing song info
///////////////////////////////////////////////////////////////////////////////

class NowPlayingPanel {
public:
    // NowPlayingInfo structure
    struct NowPlayingInfo {
        char songName[256];      // Current song name
        double currentTime;      // Current playback time in seconds
        double totalTime;        // Total song duration in seconds
        double progress;         // Progress 0.0-1.0
        bool isPlaying;          // Is currently playing
    };

    NowPlayingPanel(RECT bounds);
    ~NowPlayingPanel();

    // Data management
    void SetInfo(const NowPlayingInfo& info);
    const NowPlayingInfo& GetInfo() const { return m_info; }

    // Layout
    void SetBounds(RECT bounds);
    RECT GetBounds() const { return m_bounds; }

    // Rendering
    void Draw(LICE_IBitmap* bm, FullscreenTheme* theme);

private:
    RECT m_bounds;
    NowPlayingInfo m_info;

    void DrawPlayIcon(LICE_IBitmap* bm, FullscreenTheme* theme);
    void DrawSongName(LICE_IBitmap* bm, FullscreenTheme* theme);
    void DrawTimeInfo(LICE_IBitmap* bm, FullscreenTheme* theme);
    void DrawProgressBar(LICE_IBitmap* bm, FullscreenTheme* theme);
};

///////////////////////////////////////////////////////////////////////////////
// SetlistItemRenderer - Renders individual setlist items
///////////////////////////////////////////////////////////////////////////////

class SetlistItemRenderer {
public:
    static void DrawItem(
        LICE_IBitmap* bm,
        const SetlistView::SetlistItem& item,
        RECT itemRect,
        FullscreenTheme* theme
    );

private:
    static void DrawBackground(LICE_IBitmap* bm, RECT rect, const SetlistView::SetlistItem& item, FullscreenTheme* theme);
    static void DrawStatusIcon(LICE_IBitmap* bm, RECT rect, const SetlistView::SetlistItem& item, FullscreenTheme* theme);
    static void DrawNumber(LICE_IBitmap* bm, RECT rect, const SetlistView::SetlistItem& item, FullscreenTheme* theme);
    static void DrawSongName(LICE_IBitmap* bm, RECT rect, const SetlistView::SetlistItem& item, FullscreenTheme* theme);
    static void DrawDuration(LICE_IBitmap* bm, RECT rect, const SetlistView::SetlistItem& item, FullscreenTheme* theme);
};

///////////////////////////////////////////////////////////////////////////////
// FullscreenSetlistWindow - Main window class
///////////////////////////////////////////////////////////////////////////////

class FullscreenSetlistWindow {
public:
    FullscreenSetlistWindow();
    ~FullscreenSetlistWindow();

    // Window management
    void Show();
    void Hide();
    void ToggleFullscreen();
    bool IsFullscreen() const { return m_isFullscreen; }
    bool IsVisible() const { return m_hwnd && IsWindowVisible(m_hwnd); }

    // Setlist integration
    void LoadSetlist(int playlistIndex);
    void RefreshFromSWS();

    // Playback control
    void PlaySelected();
    void PlayNext();
    void PlayPrevious();
    void Stop();
    void TogglePlayPause();

    // Navigation
    void SelectNext();
    void SelectPrevious();
    void SelectItem(int index);
    void ScrollToItem(int index);
    void JumpToFirst();
    void JumpToLast();

private:
    HWND m_hwnd;
    NowPlayingPanel* m_nowPlayingPanel;
    SetlistView* m_setlistView;
    TransportPanel* m_transportPanel;
    KeyboardController* m_keyboardController;
    FullscreenTheme* m_theme;

    int m_currentPlaylistIndex;
    int m_selectedItemIndex;
    bool m_isFullscreen;
    RECT m_savedWindowRect;  // For restoring from fullscreen
    LONG m_savedWindowStyle;  // For restoring from fullscreen

    // Window procedure
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // Event handlers
    void OnPaint();
    void OnKeyDown(WPARAM key);
    void OnMouseMove(int x, int y);
    void OnMouseClick(int x, int y);
    void OnResize(int width, int height);

    // Layout
    void UpdateLayout();

    // Helper methods
    bool CreateMainWindow();
    void DestroyMainWindow();
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
