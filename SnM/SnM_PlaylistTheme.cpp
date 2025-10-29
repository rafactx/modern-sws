/******************************************************************************
/ SnM_PlaylistTheme.cpp
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

#include "stdafx.h"
#include "SnM_PlaylistTheme.h"
#include "SnM_PlaylistIcons.h"
#include "SnM_Dlg.h"

// Static instance
PlaylistTheme* PlaylistTheme::s_instance = NULL;

///////////////////////////////////////////////////////////////////////////////
// PlaylistTheme implementation
///////////////////////////////////////////////////////////////////////////////

PlaylistTheme::PlaylistTheme()
    : m_isDark(true)
{
    // Initialize fonts to NULL
    m_fonts.itemName = NULL;
    m_fonts.itemNumber = NULL;
    m_fonts.itemTime = NULL;
    m_fonts.monitorLarge = NULL;
    m_fonts.monitorMedium = NULL;

    // Load default theme
    LoadDefaultDarkTheme();
}

PlaylistTheme::~PlaylistTheme()
{
    CleanupFonts();
}

PlaylistTheme* PlaylistTheme::GetInstance()
{
    // TASK 11.3: Handle singleton creation failure
    if (!s_instance) {
        #ifdef _DEBUG
        try {
        #endif
            s_instance = new PlaylistTheme();

            // TASK 11.1: Check if instance was created successfully
            if (!s_instance) {
                #ifdef _DEBUG
                OutputDebugString("PlaylistTheme::GetInstance - Failed to create instance\n");
                #endif
            }
        #ifdef _DEBUG
        }
        catch (...) {
            OutputDebugString("PlaylistTheme::GetInstance - Exception during instance creation\n");
            s_instance = NULL;
        }
        #endif
    }
    return s_instance;
}

void PlaylistTheme::LoadTheme(bool isDark)
{
    m_isDark = isDark;

    if (isDark) {
        LoadDefaultDarkTheme();
    } else {
        LoadDefaultLightTheme();
    }

    // Try to load custom colors from INI
    LoadCustomThemeFromINI();

    // Initialize fonts
    InitializeFonts();
}

void PlaylistTheme::UpdateTheme()
{
    // TASK 11.3: Use try-catch for theme detection
    #ifdef _DEBUG
    try {
    #endif
        // Detect current REAPER theme
        // For now, we'll use a simple heuristic based on existing SNM theme functions
        int bg, txt;
        SNM_GetThemeListColors(&bg, &txt);

        // Determine if dark based on background luminance
        int r = (bg >> 16) & 0xFF;
        int g = (bg >> 8) & 0xFF;
        int b = bg & 0xFF;
        float luminance = (0.299f * r + 0.587f * g + 0.114f * b) / 255.0f;

        bool isDark = luminance < 0.5f;
        LoadTheme(isDark);

        // TASK 11.1: Check icon manager before clearing cache
        // Clear icon cache when theme changes to regenerate icons with new colors
        PlaylistIconManager* iconMgr = PlaylistIconManager::GetInstance();
        if (iconMgr) {
            iconMgr->ClearCache();
        } else {
            #ifdef _DEBUG
            OutputDebugString("PlaylistTheme::UpdateTheme - NULL icon manager\n");
            #endif
        }
    #ifdef _DEBUG
    }
    catch (...) {
        OutputDebugString("PlaylistTheme::UpdateTheme - Exception during theme update\n");
        // Continue with existing theme
    }
    #endif
}

void PlaylistTheme::LoadDefaultDarkTheme()
{
    // Base dark theme colors
    m_colors.background = LICE_RGBA(30, 30, 30, 255);
    m_colors.text = LICE_RGBA(224, 224, 224, 255);
    m_colors.currentItemBg = LICE_RGBA(45, 95, 141, 255);
    m_colors.currentItemText = LICE_RGBA(255, 255, 255, 255);
    m_colors.nextItemBg = LICE_RGBA(62, 62, 66, 255);
    m_colors.nextItemText = LICE_RGBA(224, 224, 224, 255);
    m_colors.selectedBg = LICE_RGBA(51, 51, 51, 255);
    m_colors.selectedText = LICE_RGBA(224, 224, 224, 255);
    m_colors.hoverBg = LICE_RGBA(45, 45, 48, 255);
    m_colors.border = LICE_RGBA(60, 60, 60, 255);
    m_colors.progressBar = LICE_RGBA(0, 120, 215, 255);
    m_colors.warningRed = LICE_RGBA(255, 0, 0, 255);
    m_colors.accentBlue = LICE_RGBA(0, 120, 215, 255);

    // PLATFORM-SPECIFIC COLOR ADJUSTMENTS (Tasks 10.1, 10.2, 10.3)

#ifdef _WIN32
    // Windows Dark Theme Adjustments (Task 10.1)
    // Windows 10/11 dark mode uses slightly different color palette
    // Adjust colors to match Windows system theme better
    m_colors.background = LICE_RGBA(32, 32, 32, 255);      // Slightly lighter for Windows
    m_colors.currentItemBg = LICE_RGBA(0, 120, 215, 255);  // Windows accent blue
    m_colors.selectedBg = LICE_RGBA(55, 55, 55, 255);      // Match Windows selection
    m_colors.hoverBg = LICE_RGBA(50, 50, 52, 255);         // Subtle hover for Windows

#elif defined(__APPLE__)
    // macOS Dark Theme Adjustments (Task 10.2)
    // macOS dark mode has different color characteristics
    // Adjust to match macOS system appearance
    m_colors.background = LICE_RGBA(28, 28, 30, 255);      // macOS dark background
    m_colors.currentItemBg = LICE_RGBA(10, 132, 255, 255); // macOS system blue
    m_colors.selectedBg = LICE_RGBA(48, 48, 51, 255);      // macOS selection color
    m_colors.hoverBg = LICE_RGBA(44, 44, 46, 255);         // macOS hover color
    m_colors.border = LICE_RGBA(58, 58, 60, 255);          // macOS separator color

#else
    // Linux Dark Theme Adjustments (Task 10.3)
    // Linux has various desktop environments (GNOME, KDE, etc.)
    // Use neutral colors that work well across different themes
    m_colors.background = LICE_RGBA(35, 35, 35, 255);      // Neutral dark background
    m_colors.currentItemBg = LICE_RGBA(52, 101, 164, 255); // GNOME-style blue
    m_colors.selectedBg = LICE_RGBA(60, 60, 60, 255);      // Neutral selection
    m_colors.hoverBg = LICE_RGBA(48, 48, 48, 255);         // Subtle hover
    m_colors.border = LICE_RGBA(70, 70, 70, 255);          // Visible border
#endif
}

void PlaylistTheme::LoadDefaultLightTheme()
{
    // Base light theme colors
    m_colors.background = LICE_RGBA(255, 255, 255, 255);
    m_colors.text = LICE_RGBA(30, 30, 30, 255);
    m_colors.currentItemBg = LICE_RGBA(0, 120, 215, 255);
    m_colors.currentItemText = LICE_RGBA(255, 255, 255, 255);
    m_colors.nextItemBg = LICE_RGBA(224, 224, 224, 255);
    m_colors.nextItemText = LICE_RGBA(30, 30, 30, 255);
    m_colors.selectedBg = LICE_RGBA(204, 204, 204, 255);
    m_colors.selectedText = LICE_RGBA(30, 30, 30, 255);
    m_colors.hoverBg = LICE_RGBA(240, 240, 240, 255);
    m_colors.border = LICE_RGBA(200, 200, 200, 255);
    m_colors.progressBar = LICE_RGBA(0, 120, 215, 255);
    m_colors.warningRed = LICE_RGBA(255, 0, 0, 255);
    m_colors.accentBlue = LICE_RGBA(0, 120, 215, 255);

    // PLATFORM-SPECIFIC COLOR ADJUSTMENTS (Tasks 10.1, 10.2, 10.3)

#ifdef _WIN32
    // Windows Light Theme Adjustments (Task 10.1)
    // Windows 10/11 light mode uses specific color palette
    m_colors.background = LICE_RGBA(255, 255, 255, 255);   // Pure white for Windows
    m_colors.currentItemBg = LICE_RGBA(0, 120, 215, 255);  // Windows accent blue
    m_colors.selectedBg = LICE_RGBA(230, 230, 230, 255);   // Windows selection
    m_colors.hoverBg = LICE_RGBA(243, 243, 243, 255);      // Subtle hover for Windows
    m_colors.border = LICE_RGBA(218, 218, 218, 255);       // Windows border color

#elif defined(__APPLE__)
    // macOS Light Theme Adjustments (Task 10.2)
    // macOS light mode has warmer tones
    m_colors.background = LICE_RGBA(255, 255, 255, 255);   // Pure white
    m_colors.currentItemBg = LICE_RGBA(10, 132, 255, 255); // macOS system blue
    m_colors.selectedBg = LICE_RGBA(220, 220, 220, 255);   // macOS selection
    m_colors.hoverBg = LICE_RGBA(245, 245, 245, 255);      // macOS hover
    m_colors.border = LICE_RGBA(210, 210, 210, 255);       // macOS separator
    m_colors.text = LICE_RGBA(0, 0, 0, 255);               // Pure black text for macOS

#else
    // Linux Light Theme Adjustments (Task 10.3)
    // Linux light themes vary, use neutral colors
    m_colors.background = LICE_RGBA(252, 252, 252, 255);   // Slightly off-white
    m_colors.currentItemBg = LICE_RGBA(52, 101, 164, 255); // GNOME-style blue
    m_colors.selectedBg = LICE_RGBA(215, 215, 215, 255);   // Neutral selection
    m_colors.hoverBg = LICE_RGBA(238, 238, 238, 255);      // Subtle hover
    m_colors.border = LICE_RGBA(190, 190, 190, 255);       // Visible border
#endif
}

void PlaylistTheme::LoadCustomThemeFromINI()
{
    // Helper function to read color from INI
    auto readColor = [](const char* section, const char* key, int defaultValue) -> int {
        char str[32];
        GetPrivateProfileString(section, key, "", str, sizeof(str), get_ini_file());

        if (str[0] == '\0') {
            return defaultValue; // Key not found, use default
        }

        // Parse hex color (format: 0xRRGGBB or RRGGBB)
        unsigned int rgb = 0;
        if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
            sscanf(str + 2, "%x", &rgb);
        } else {
            sscanf(str, "%x", &rgb);
        }

        // Convert to LICE_RGBA format
        int r = (rgb >> 16) & 0xFF;
        int g = (rgb >> 8) & 0xFF;
        int b = rgb & 0xFF;
        return LICE_RGBA(r, g, b, 255);
    };

    // Determine which section to read from based on theme
    const char* section = m_isDark ? "sws_playlist_theme_dark" : "sws_playlist_theme_light";

    // Read custom colors from INI, falling back to current defaults if not found
    m_colors.background = readColor(section, "background", m_colors.background);
    m_colors.text = readColor(section, "text", m_colors.text);
    m_colors.currentItemBg = readColor(section, "current_bg", m_colors.currentItemBg);
    m_colors.currentItemText = readColor(section, "current_text", m_colors.currentItemText);
    m_colors.nextItemBg = readColor(section, "next_bg", m_colors.nextItemBg);
    m_colors.nextItemText = readColor(section, "next_text", m_colors.nextItemText);
    m_colors.selectedBg = readColor(section, "selected_bg", m_colors.selectedBg);
    m_colors.selectedText = readColor(section, "selected_text", m_colors.selectedText);
    m_colors.hoverBg = readColor(section, "hover_bg", m_colors.hoverBg);
    m_colors.border = readColor(section, "border", m_colors.border);
    m_colors.progressBar = readColor(section, "progress_bar", m_colors.progressBar);
    m_colors.warningRed = readColor(section, "warning_red", m_colors.warningRed);
    m_colors.accentBlue = readColor(section, "accent_blue", m_colors.accentBlue);
}

void PlaylistTheme::InitializeFonts()
{
    // Use default font sizes
    FontSizes defaultSizes;
    InitializeFontsWithSizes(defaultSizes);
}

void PlaylistTheme::InitializeFontsWithSizes(const FontSizes& sizes)
{
    // TASK 11.4: Check if fonts are already cached with correct sizes
    // Check if font sizes have changed - if not, reuse cached fonts
    if (m_fonts.itemName != NULL && m_fontSizes == sizes) {
        // Fonts are already cached with the correct sizes, no need to recreate
        return;
    }

    // TASK 11.3: Use try-catch for font initialization
    #ifdef _DEBUG
    try {
    #endif
        // Clean up existing fonts only if sizes changed
        CleanupFonts();

        // Store new font sizes
        m_fontSizes = sizes;

    // Helper lambda to create a font with platform-specific adjustments
    auto createFont = [](int height) -> LICE_CachedFont* {
        LICE_CachedFont* font = new LICE_CachedFont();

#ifdef _WIN32
        // WINDOWS PLATFORM-SPECIFIC ADJUSTMENTS (Task 10.1)

        // Get DPI scaling factor for High DPI support
        HDC hdc = GetDC(NULL);
        int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(NULL, hdc);

        // Scale font height based on DPI (96 is standard DPI)
        // This ensures fonts render correctly on High DPI displays
        int scaledHeight = -MulDiv(height, dpiY, 72);

        LOGFONT lf = {
            scaledHeight,           // Height scaled for DPI
            0,                      // Width (0 = default)
            0,                      // Escapement
            0,                      // Orientation
            FW_NORMAL,              // Weight
            FALSE,                  // Italic
            FALSE,                  // Underline
            FALSE,                  // StrikeOut
            DEFAULT_CHARSET,        // CharSet
            OUT_DEFAULT_PRECIS,     // OutPrecision
            CLIP_DEFAULT_PRECIS,    // ClipPrecision
            CLEARTYPE_QUALITY,      // Quality - use CLEARTYPE_QUALITY for better rendering
            DEFAULT_PITCH,          // PitchAndFamily
            "Segoe UI"              // FaceName - modern Windows font
        };

        // Create font with ClearType support for smooth rendering
        // Force native rendering on Windows for better ClearType support
        font->SetFromHFont(
            CreateFontIndirect(&lf),
            LICE_FONT_FLAG_OWNS_HFONT | LICE_FONT_FLAG_FORCE_NATIVE
        );

#elif defined(__APPLE__)
        // MACOS PLATFORM-SPECIFIC ADJUSTMENTS (Task 10.2)
        // macOS uses Core Graphics via SWELL

        // Get scale factor for Retina display support
        // SWELL handles Retina scaling automatically, but we adjust font sizes
        float scaleFactor = 1.0f;

        // On Retina displays, SWELL reports 2x scale
        // We don't need to manually scale fonts as SWELL handles it,
        // but we use slightly different font metrics for better rendering
        int adjustedHeight = height;

        LOGFONT lf = {
            -adjustedHeight,        // Negative height for better scaling
            0,                      // Width (0 = default)
            0,                      // Escapement
            0,                      // Orientation
            FW_NORMAL,              // Weight
            FALSE,                  // Italic
            FALSE,                  // Underline
            FALSE,                  // StrikeOut
            DEFAULT_CHARSET,        // CharSet
            OUT_DEFAULT_PRECIS,     // OutPrecision
            CLIP_DEFAULT_PRECIS,    // ClipPrecision
            ANTIALIASED_QUALITY,    // Quality - use antialiasing on macOS
            DEFAULT_PITCH,          // PitchAndFamily
            "Helvetica Neue"        // FaceName - native macOS font
        };

        // Create font with antialiasing for smooth rendering on Retina displays
        font->SetFromHFont(
            CreateFontIndirect(&lf),
            LICE_FONT_FLAG_OWNS_HFONT
        );

#else
        // LINUX PLATFORM-SPECIFIC ADJUSTMENTS (Task 10.3)
        // Linux uses Cairo via SWELL

        // Cairo handles font rendering differently
        // Use negative height for better scaling
        int adjustedHeight = height;

        LOGFONT lf = {
            -adjustedHeight,        // Negative height for better scaling
            0,                      // Width (0 = default)
            0,                      // Escapement
            0,                      // Orientation
            FW_NORMAL,              // Weight
            FALSE,                  // Italic
            FALSE,                  // Underline
            FALSE,                  // StrikeOut
            DEFAULT_CHARSET,        // CharSet
            OUT_DEFAULT_PRECIS,     // OutPrecision
            CLIP_DEFAULT_PRECIS,    // ClipPrecision
            ANTIALIASED_QUALITY,    // Quality - use antialiasing on Linux
            DEFAULT_PITCH,          // PitchAndFamily
            "Liberation Sans"       // FaceName - common Linux font (fallback to system default)
        };

        // Create font with antialiasing for smooth rendering
        font->SetFromHFont(
            CreateFontIndirect(&lf),
            LICE_FONT_FLAG_OWNS_HFONT
        );
#endif

        font->SetBkMode(TRANSPARENT);
        return font;
    };

        // TASK 11.4: Create fonts with graceful degradation
        // Create fonts with specified sizes
        m_fonts.itemName = createFont(sizes.itemName);
        m_fonts.itemNumber = createFont(sizes.itemNumber);
        m_fonts.itemTime = createFont(sizes.itemTime);
        m_fonts.monitorLarge = createFont(sizes.monitorLarge);
        m_fonts.monitorMedium = createFont(sizes.monitorMedium);

        // TASK 11.1: Check if fonts were created successfully
        #ifdef _DEBUG
        if (!m_fonts.itemName) OutputDebugString("PlaylistTheme::InitializeFontsWithSizes - Failed to create itemName font\n");
        if (!m_fonts.itemNumber) OutputDebugString("PlaylistTheme::InitializeFontsWithSizes - Failed to create itemNumber font\n");
        if (!m_fonts.itemTime) OutputDebugString("PlaylistTheme::InitializeFontsWithSizes - Failed to create itemTime font\n");
        if (!m_fonts.monitorLarge) OutputDebugString("PlaylistTheme::InitializeFontsWithSizes - Failed to create monitorLarge font\n");
        if (!m_fonts.monitorMedium) OutputDebugString("PlaylistTheme::InitializeFontsWithSizes - Failed to create monitorMedium font\n");
        #endif
    #ifdef _DEBUG
    }
    catch (...) {
        OutputDebugString("PlaylistTheme::InitializeFontsWithSizes - Exception during font initialization\n");
        // Clean up any partially created fonts
        CleanupFonts();
    }
    #endif
}

void PlaylistTheme::CleanupFonts()
{
    // TASK 11.3: Safe cleanup with try-catch
    #ifdef _DEBUG
    try {
    #endif
        // TASK 11.1: Delete fonts if they exist
        if (m_fonts.itemName) {
            delete m_fonts.itemName;
            m_fonts.itemName = NULL;
        }
        if (m_fonts.itemNumber) {
            delete m_fonts.itemNumber;
            m_fonts.itemNumber = NULL;
        }
        if (m_fonts.itemTime) {
            delete m_fonts.itemTime;
            m_fonts.itemTime = NULL;
        }
        if (m_fonts.monitorLarge) {
            delete m_fonts.monitorLarge;
            m_fonts.monitorLarge = NULL;
        }
        if (m_fonts.monitorMedium) {
            delete m_fonts.monitorMedium;
            m_fonts.monitorMedium = NULL;
        }
    #ifdef _DEBUG
    }
    catch (...) {
        OutputDebugString("PlaylistTheme::CleanupFonts - Exception during font cleanup\n");
        // Set all pointers to NULL to prevent dangling pointers
        m_fonts.itemName = NULL;
        m_fonts.itemNumber = NULL;
        m_fonts.itemTime = NULL;
        m_fonts.monitorLarge = NULL;
        m_fonts.monitorMedium = NULL;
    }
    #endif
}

int PlaylistTheme::GetHoverColor(int baseColor) const
{
    // Extract RGBA components
    int r = LICE_GETR(baseColor);
    int g = LICE_GETG(baseColor);
    int b = LICE_GETB(baseColor);
    int a = LICE_GETA(baseColor);

    // Adjust brightness based on theme
    float factor = m_isDark ? 1.15f : 0.95f;

    r = (int)(r * factor);
    g = (int)(g * factor);
    b = (int)(b * factor);

    // Clamp values
    r = r > 255 ? 255 : r;
    g = g > 255 ? 255 : g;
    b = b > 255 ? 255 : b;

    return LICE_RGBA(r, g, b, a);
}

int PlaylistTheme::BlendColors(int color1, int color2, float ratio) const
{
    int r1 = LICE_GETR(color1);
    int g1 = LICE_GETG(color1);
    int b1 = LICE_GETB(color1);
    int a1 = LICE_GETA(color1);

    int r2 = LICE_GETR(color2);
    int g2 = LICE_GETG(color2);
    int b2 = LICE_GETB(color2);
    int a2 = LICE_GETA(color2);

    int r = (int)(r1 * (1.0f - ratio) + r2 * ratio);
    int g = (int)(g1 * (1.0f - ratio) + g2 * ratio);
    int b = (int)(b1 * (1.0f - ratio) + b2 * ratio);
    int a = (int)(a1 * (1.0f - ratio) + a2 * ratio);

    return LICE_RGBA(r, g, b, a);
}

bool PlaylistTheme::ValidateContrastRatio(int fg, int bg, float minRatio) const
{
    // Calculate relative luminance for foreground
    float r1 = LICE_GETR(fg) / 255.0f;
    float g1 = LICE_GETG(fg) / 255.0f;
    float b1 = LICE_GETB(fg) / 255.0f;

    r1 = (r1 <= 0.03928f) ? r1 / 12.92f : powf((r1 + 0.055f) / 1.055f, 2.4f);
    g1 = (g1 <= 0.03928f) ? g1 / 12.92f : powf((g1 + 0.055f) / 1.055f, 2.4f);
    b1 = (b1 <= 0.03928f) ? b1 / 12.92f : powf((b1 + 0.055f) / 1.055f, 2.4f);

    float L1 = 0.2126f * r1 + 0.7152f * g1 + 0.0722f * b1;

    // Calculate relative luminance for background
    float r2 = LICE_GETR(bg) / 255.0f;
    float g2 = LICE_GETG(bg) / 255.0f;
    float b2 = LICE_GETB(bg) / 255.0f;

    r2 = (r2 <= 0.03928f) ? r2 / 12.92f : powf((r2 + 0.055f) / 1.055f, 2.4f);
    g2 = (g2 <= 0.03928f) ? g2 / 12.92f : powf((g2 + 0.055f) / 1.055f, 2.4f);
    b2 = (b2 <= 0.03928f) ? b2 / 12.92f : powf((b2 + 0.055f) / 1.055f, 2.4f);

    float L2 = 0.2126f * r2 + 0.7152f * g2 + 0.0722f * b2;

    // Calculate contrast ratio
    float lighter = L1 > L2 ? L1 : L2;
    float darker = L1 > L2 ? L2 : L1;
    float ratio = (lighter + 0.05f) / (darker + 0.05f);

    return ratio >= minRatio;
}
