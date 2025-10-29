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

#ifndef _SNM_PLAYLISTTHEME_H_
#define _SNM_PLAYLISTTHEME_H_

#include "../WDL/lice/lice.h"
#include "../WDL/wdltypes.h"

// Forward declarations
class LICE_CachedFont;

///////////////////////////////////////////////////////////////////////////////
// PlaylistTheme - Manages colors, fonts and visual styles for playlist UI
///////////////////////////////////////////////////////////////////////////////

class PlaylistTheme {
public:
    // Color scheme structure
    struct Colors {
        int background;
        int text;
        int currentItemBg;
        int currentItemText;
        int nextItemBg;
        int nextItemText;
        int selectedBg;
        int selectedText;
        int hoverBg;
        int border;
        int progressBar;
        int warningRed;
        int accentBlue;
    };

    // Font collection structure
    struct Fonts {
        LICE_CachedFont* itemName;      // 12pt
        LICE_CachedFont* itemNumber;    // 14pt
        LICE_CachedFont* itemTime;      // 11pt
        LICE_CachedFont* monitorLarge;  // 24pt
        LICE_CachedFont* monitorMedium; // 20pt
    };

    // Font size tracking for cache invalidation
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

    // Singleton access
    static PlaylistTheme* GetInstance();

    // Theme management
    void LoadTheme(bool isDark);
    void UpdateTheme();

    // Accessors
    const Colors& GetColors() const { return m_colors; }
    const Fonts& GetFonts() const { return m_fonts; }
    bool IsDark() const { return m_isDark; }

    // Color utilities
    int GetHoverColor(int baseColor) const;
    int BlendColors(int color1, int color2, float ratio) const;
    bool ValidateContrastRatio(int fg, int bg, float minRatio) const;

private:
    PlaylistTheme();
    ~PlaylistTheme();

    // Prevent copying
    PlaylistTheme(const PlaylistTheme&);
    PlaylistTheme& operator=(const PlaylistTheme&);

    void LoadDefaultDarkTheme();
    void LoadDefaultLightTheme();
    void LoadCustomThemeFromINI();
    void InitializeFonts();
    void InitializeFontsWithSizes(const FontSizes& sizes);
    void CleanupFonts();

    Colors m_colors;
    Fonts m_fonts;
    FontSizes m_fontSizes;
    bool m_isDark;

    static PlaylistTheme* s_instance;
};

#endif // _SNM_PLAYLISTTHEME_H_
