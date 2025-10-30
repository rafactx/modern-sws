/******************************************************************************
/ SnM_FullscreenSetlist.cpp
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

#include "stdafx.h"

#include "SnM.h"
#include "SnM_FullscreenSetlist.h"
#include "SnM_RegionPlaylist.h"
#include "SnM_Dlg.h"

///////////////////////////////////////////////////////////////////////////////
// Global variables
///////////////////////////////////////////////////////////////////////////////

static FullscreenSetlistWindow* g_fullscreenSetlistWnd = NULL;
static FullscreenTheme* g_fullscreenTheme = NULL;

///////////////////////////////////////////////////////////////////////////////
// FullscreenTheme - Theme system implementation
///////////////////////////////////////////////////////////////////////////////

FullscreenTheme::FullscreenTheme() {
    // Initialize all font pointers to NULL
    m_fonts.songNameLarge = NULL;
    m_fonts.songNameHuge = NULL;
    m_fonts.numberLarge = NULL;
    m_fonts.timeMedium = NULL;
    m_fonts.timeSmall = NULL;
    m_fonts.infoText = NULL;
}

FullscreenTheme::~FullscreenTheme() {
    CleanupFonts();
}

FullscreenTheme* FullscreenTheme::GetInstance() {
    if (!g_fullscreenTheme) {
        g_fullscreenTheme = new FullscreenTheme();
        g_fullscreenTheme->LoadTheme();
    }
    return g_fullscreenTheme;
}

void FullscreenTheme::LoadTheme() {
    InitializeColors();
    InitializeFonts();
}

void FullscreenTheme::InitializeColors() {
    // Background: Very dark (< 10% luminance) - RGB(15, 15, 15) ≈ 6% luminance
    m_colors.background = LICE_RGBA(15, 15, 15, 255);

    // Text: Light gray (90% luminance) - RGB(230, 230, 230) ≈ 90% luminance
    m_colors.text = LICE_RGBA(230, 230, 230, 255);

    // Text dimmed: Medium gray (60% luminance) - RGB(153, 153, 153) ≈ 60% luminance
    m_colors.textDimmed = LICE_RGBA(153, 153, 153, 255);

    // Current item background: Accent color (40-60% luminance) - RGB(80, 120, 180) ≈ 47% luminance
    m_colors.currentItemBg = LICE_RGBA(80, 120, 180, 255);

    // Current item text: White for maximum contrast
    m_colors.currentItemText = LICE_RGBA(255, 255, 255, 255);

    // Next item background: Subtle highlight - RGB(35, 35, 40) ≈ 14% luminance
    m_colors.nextItemBg = LICE_RGBA(35, 35, 40, 255);

    // Next item text: Light gray
    m_colors.nextItemText = LICE_RGBA(200, 200, 200, 255);

    // Selected border: Bright accent - RGB(100, 150, 220) ≈ 59% luminance
    m_colors.selectedBorder = LICE_RGBA(100, 150, 220, 255);

    // Progress bar fill: Accent color matching current item
    m_colors.progressBarFill = LICE_RGBA(80, 120, 180, 255);

    // Progress bar background: Dark gray
    m_colors.progressBarBg = LICE_RGBA(40, 40, 40, 255);

    // Button normal: Medium gray
    m_colors.buttonNormal = LICE_RGBA(60, 60, 60, 255);

    // Button hover: Lighter gray
    m_colors.buttonHover = LICE_RGBA(90, 90, 90, 255);

    // Button pressed: Accent color
    m_colors.buttonPressed = LICE_RGBA(80, 120, 180, 255);

    // Button text: White
    m_colors.buttonText = LICE_RGBA(255, 255, 255, 255);
}

void FullscreenTheme::InitializeFonts() {
    // Clean up any existing fonts first
    CleanupFonts();

    // Create fonts with specified sizes
    // Note: Font height in LOGFONT is negative for character height (positive for cell height)

    // 32pt font for song names in setlist
    m_fonts.songNameLarge = new LICE_CachedFont();
    LOGFONT lf32 = {-32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, SNM_FONT_NAME};
#ifndef _SNM_SWELL_ISSUES
    m_fonts.songNameLarge->SetFromHFont(CreateFontIndirect(&lf32),
        LICE_FONT_FLAG_OWNS_HFONT | (g_SNM_ClearType ? LICE_FONT_FLAG_FORCE_NATIVE : 0));
#else
    m_fonts.songNameLarge->SetFromHFont(CreateFontIndirect(&lf32), LICE_FONT_FLAG_OWNS_HFONT);
#endif

    // 40pt font for now playing display
    m_fonts.songNameHuge = new LICE_CachedFont();
    LOGFONT lf40 = {-40, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, SNM_FONT_NAME};
#ifndef _SNM_SWELL_ISSUES
    m_fonts.songNameHuge->SetFromHFont(CreateFontIndirect(&lf40),
        LICE_FONT_FLAG_OWNS_HFONT | (g_SNM_ClearType ? LICE_FONT_FLAG_FORCE_NATIVE : 0));
#else
    m_fonts.songNameHuge->SetFromHFont(CreateFontIndirect(&lf40), LICE_FONT_FLAG_OWNS_HFONT);
#endif

    // 28pt font for song numbers
    m_fonts.numberLarge = new LICE_CachedFont();
    LOGFONT lf28 = {-28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, SNM_FONT_NAME};
#ifndef _SNM_SWELL_ISSUES
    m_fonts.numberLarge->SetFromHFont(CreateFontIndirect(&lf28),
        LICE_FONT_FLAG_OWNS_HFONT | (g_SNM_ClearType ? LICE_FONT_FLAG_FORCE_NATIVE : 0));
#else
    m_fonts.numberLarge->SetFromHFont(CreateFontIndirect(&lf28), LICE_FONT_FLAG_OWNS_HFONT);
#endif

    // 24pt font for time display
    m_fonts.timeMedium = new LICE_CachedFont();
    LOGFONT lf24 = {-24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, SNM_FONT_NAME};
#ifndef _SNM_SWELL_ISSUES
    m_fonts.timeMedium->SetFromHFont(CreateFontIndirect(&lf24),
        LICE_FONT_FLAG_OWNS_HFONT | (g_SNM_ClearType ? LICE_FONT_FLAG_FORCE_NATIVE : 0));
#else
    m_fonts.timeMedium->SetFromHFont(CreateFontIndirect(&lf24), LICE_FONT_FLAG_OWNS_HFONT);
#endif

    // 20pt font for duration display
    m_fonts.timeSmall = new LICE_CachedFont();
    LOGFONT lf20 = {-20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, SNM_FONT_NAME};
#ifndef _SNM_SWELL_ISSUES
    m_fonts.timeSmall->SetFromHFont(CreateFontIndirect(&lf20),
        LICE_FONT_FLAG_OWNS_HFONT | (g_SNM_ClearType ? LICE_FONT_FLAG_FORCE_NATIVE : 0));
#else
    m_fonts.timeSmall->SetFromHFont(CreateFontIndirect(&lf20), LICE_FONT_FLAG_OWNS_HFONT);
#endif

    // 18pt font for info text
    m_fonts.infoText = new LICE_CachedFont();
    LOGFONT lf18 = {-18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, SNM_FONT_NAME};
#ifndef _SNM_SWELL_ISSUES
    m_fonts.infoText->SetFromHFont(CreateFontIndirect(&lf18),
        LICE_FONT_FLAG_OWNS_HFONT | (g_SNM_ClearType ? LICE_FONT_FLAG_FORCE_NATIVE : 0));
#else
    m_fonts.infoText->SetFromHFont(CreateFontIndirect(&lf18), LICE_FONT_FLAG_OWNS_HFONT);
#endif
}

void FullscreenTheme::CleanupFonts() {
    if (m_fonts.songNameLarge) {
        delete m_fonts.songNameLarge;
        m_fonts.songNameLarge = NULL;
    }
    if (m_fonts.songNameHuge) {
        delete m_fonts.songNameHuge;
        m_fonts.songNameHuge = NULL;
    }
    if (m_fonts.numberLarge) {
        delete m_fonts.numberLarge;
        m_fonts.numberLarge = NULL;
    }
    if (m_fonts.timeMedium) {
        delete m_fonts.timeMedium;
        m_fonts.timeMedium = NULL;
    }
    if (m_fonts.timeSmall) {
        delete m_fonts.timeSmall;
        m_fonts.timeSmall = NULL;
    }
    if (m_fonts.infoText) {
        delete m_fonts.infoText;
        m_fonts.infoText = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
// FullscreenSetlistWindow - Main window class (stub)
///////////////////////////////////////////////////////////////////////////////

class FullscreenSetlistWindow {
public:
    FullscreenSetlistWindow() {}
    ~FullscreenSetlistWindow() {}

    void Show() {}
    void Hide() {}
    bool IsVisible() const { return false; }
};

///////////////////////////////////////////////////////////////////////////////
// Initialization and cleanup
///////////////////////////////////////////////////////////////////////////////

int FullscreenSetlistInit() {
    // Initialization will be implemented in future tasks
    return 1;
}

void FullscreenSetlistExit() {
    // Cleanup window
    if (g_fullscreenSetlistWnd) {
        delete g_fullscreenSetlistWnd;
        g_fullscreenSetlistWnd = NULL;
    }

    // Cleanup theme
    if (g_fullscreenTheme) {
        delete g_fullscreenTheme;
        g_fullscreenTheme = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Command handlers
///////////////////////////////////////////////////////////////////////////////

void OpenFullscreenSetlist(COMMAND_T*) {
    if (!g_fullscreenSetlistWnd) {
        g_fullscreenSetlistWnd = new FullscreenSetlistWindow();
    }

    if (g_fullscreenSetlistWnd) {
        if (g_fullscreenSetlistWnd->IsVisible()) {
            g_fullscreenSetlistWnd->Hide();
        } else {
            g_fullscreenSetlistWnd->Show();
        }
    }
}

int IsFullscreenSetlistDisplayed(COMMAND_T*) {
    return (g_fullscreenSetlistWnd && g_fullscreenSetlistWnd->IsVisible()) ? 1 : 0;
}
