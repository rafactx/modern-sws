/******************************************************************************
/ SnM_ModernPlaylistUI.h
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

#ifndef _SNM_MODERNPLAYLISTUI_H_
#define _SNM_MODERNPLAYLISTUI_H_

#include "SnM_RegionPlaylist.h"
#include "SnM_PlaylistTheme.h"
#include "SnM_PlaylistIcons.h"

// Forward declarations
class PlaylistTheme;
class PlaylistIconManager;

// External global variables from SnM_RegionPlaylist.cpp
extern int g_playPlaylist;      // -1: stopped, playlist id otherwise
extern bool g_unsync;           // true when switching to a position that is not part of the playlist
extern int g_playCur;           // index of the item being played, -1 means "not playing yet"
extern int g_playNext;          // index of the next item to be played, -1 means "the end"
extern int g_rgnLoop;           // region loop count: 0 not looping, <0 infinite loop, n>0 looping n times
extern SWSProjConfig<RegionPlaylists> g_pls;

///////////////////////////////////////////////////////////////////////////////
// ModernPlaylistItemRenderer - Renders individual playlist items
///////////////////////////////////////////////////////////////////////////////

class ModernPlaylistItemRenderer {
public:
    // Visual state of an item
    struct ItemVisualState {
        bool isPlaying;
        bool isNext;
        bool isSelected;
        bool isHovered;
        bool hasInfiniteLoop;
        int loopCount;
        bool isSyncLoss;

        ItemVisualState()
            : isPlaying(false)
            , isNext(false)
            , isSelected(false)
            , isHovered(false)
            , hasInfiniteLoop(false)
            , loopCount(1)
            , isSyncLoss(false)
        {}

        // Validation
        bool IsValid() const {
            return loopCount >= 0; // Loop count should never be negative
        }

        bool HasAnyHighlight() const {
            return isPlaying || isNext || isSelected || isHovered;
        }

        bool NeedsStatusIcon() const {
            return isPlaying || isNext || isSyncLoss;
        }
    };

    // Data for rendering an item
    struct ItemData {
        int regionNumber;
        WDL_FastString regionName;
        double startTime;
        double endTime;
        double duration;

        ItemData()
            : regionNumber(0)
            , startTime(0.0)
            , endTime(0.0)
            , duration(0.0)
        {}

        // Validation
        bool IsValid() const {
            return regionNumber > 0 &&
                   duration >= 0.0 &&
                   endTime >= startTime &&
                   regionName.GetLength() > 0;
        }

        bool HasValidTiming() const {
            return startTime >= 0.0 && endTime >= startTime;
        }

        bool IsShortRegion() const {
            return duration < 0.5; // Less than 0.5 seconds
        }
    };

    ModernPlaylistItemRenderer();
    ~ModernPlaylistItemRenderer();

    // Main drawing method
    void DrawItem(
        LICE_IBitmap* drawbm,
        const RECT& itemRect,
        const ItemData& data,
        const ItemVisualState& state,
        const PlaylistTheme* theme
    );

private:
    void DrawBackground(LICE_IBitmap* bm, const RECT& r, const ItemVisualState& state, const PlaylistTheme* theme);
    void DrawStatusIcon(LICE_IBitmap* bm, const RECT& r, const ItemVisualState& state);
    void DrawRegionNumber(LICE_IBitmap* bm, const RECT& r, int number, const PlaylistTheme* theme);
    void DrawRegionName(LICE_IBitmap* bm, const RECT& r, const char* name, const PlaylistTheme* theme);
    void DrawTimeInfo(LICE_IBitmap* bm, const RECT& r, const ItemData& data, const PlaylistTheme* theme);
    void DrawLoopBadge(LICE_IBitmap* bm, const RECT& r, int count, bool infinite, const PlaylistTheme* theme);
    void DrawRoundedRect(LICE_IBitmap* bm, const RECT& r, int radius, int color);
};

///////////////////////////////////////////////////////////////////////////////
// ModernRegionPlaylistView - Modern list view for playlist items
///////////////////////////////////////////////////////////////////////////////

class ModernRegionPlaylistView : public RegionPlaylistView {
public:
    ModernRegionPlaylistView(HWND hwndList, HWND hwndEdit);
    virtual ~ModernRegionPlaylistView();

    // Configuration
    void SetItemHeight(int height);
    int GetItemHeight() const { return m_itemHeight; }
    void EnableModernRendering(bool enable);
    bool IsModernRenderingEnabled() const { return m_modernRenderingEnabled; }

    // Hover tracking
    void OnMouseMove(int x, int y);
    int GetHoveredItemIndex() const { return m_hoveredItem; }

    // Auto-scroll
    void AutoScrollToCurrentItem();

protected:
    // Override base class methods
    void GetItemText(SWS_ListItem* item, int iCol, char* str, int iStrMax);

    // Custom rendering
    void OnItemPaint(LICE_IBitmap* drawbm, SWS_ListItem* item, const RECT& itemRect);

private:
    void UpdateTheme();
    ModernPlaylistItemRenderer::ItemVisualState GetItemState(SWS_ListItem* item, int index);
    ModernPlaylistItemRenderer::ItemData GetItemData(RgnPlaylistItem* item);

    ModernPlaylistItemRenderer m_renderer;
    PlaylistTheme* m_theme;
    int m_itemHeight;
    int m_hoveredItem;
    bool m_modernRenderingEnabled;
};

///////////////////////////////////////////////////////////////////////////////
// ModernMonitoringView - Enhanced monitoring display
///////////////////////////////////////////////////////////////////////////////

class ModernMonitoringView : public SNM_FiveMonitors {
public:
    ModernMonitoringView();
    virtual ~ModernMonitoringView();

    // Override base class
    void OnPaint(LICE_IBitmap* drawbm, int origin_x, int origin_y, RECT* cliprect, int rscale);

    // Data setters
    void SetProgress(double current, double total);
    void SetCurrentRegion(const char* name, int number);
    void SetNextRegion(const char* name, int number);
    void SetPlaylistInfo(const char* playlistName, int playlistNumber);

private:
    struct ProgressInfo {
        double current;
        double total;
        double percentage;

        ProgressInfo() : current(0.0), total(0.0), percentage(0.0) {}
    };

    void DrawProgressBar(LICE_IBitmap* bm, const RECT& r);
    void DrawLargeText(LICE_IBitmap* bm, const RECT& r, const char* text, LICE_CachedFont* font, int color);

    ProgressInfo m_progress;
    WDL_FastString m_currentName;
    WDL_FastString m_nextName;
    WDL_FastString m_playlistName;
    int m_currentNumber;
    int m_nextNumber;
    int m_playlistNumber;
    PlaylistTheme* m_theme;
};

#endif // _SNM_MODERNPLAYLISTUI_H_
