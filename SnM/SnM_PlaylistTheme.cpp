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
    if (!s_instance) {
        s_instance = new PlaylistTheme();
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
}

void PlaylistTheme::LoadDefaultDarkTheme()
{
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
}

void PlaylistTheme::LoadDefaultLightTheme()
{
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
    // Clean up existing fonts
    CleanupFonts();

    // Create fonts with proper sizes
    // Font sizes: 12pt (itemName), 14pt (itemNumber), 11pt (itemTime), 24pt (monitorLarge), 20pt (monitorMedium)

    // Helper lambda to create a font
    auto createFont = [](int height) -> LICE_CachedFont* {
        LICE_CachedFont* font = new LICE_CachedFont();
        LOGFONT lf = {
            height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
#ifdef _WIN32
            "Segoe UI"
#else
            "Arial"
#endif
        };

        font->SetFromHFont(
            CreateFontIndirect(&lf),
            LICE_FONT_FLAG_OWNS_HFONT | (g_SNM_ClearType ? LICE_FONT_FLAG_FORCE_NATIVE : 0)
        );
        font->SetBkMode(TRANSPARENT);
        return font;
    };

    // Create fonts with specified sizes
    m_fonts.itemName = createFont(12);       // 12pt
    m_fonts.itemNumber = createFont(14);     // 14pt
    m_fonts.itemTime = createFont(11);       // 11pt
    m_fonts.monitorLarge = createFont(24);   // 24pt
    m_fonts.monitorMedium = createFont(20);  // 20pt
}

void PlaylistTheme::CleanupFonts()
{
    // Delete fonts if they exist
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
