/******************************************************************************
/ SnM_ModernPlaylistView.h
/
/ Modern playlist view components that require full class definitions
/ Separated to avoid circular dependencies
/
******************************************************************************/

#ifndef _SNM_MODERNPLAYLISTVIEW_H_
#define _SNM_MODERNPLAYLISTVIEW_H_

#include "SnM_RegionPlaylist.h"
#include "SnM_ModernPlaylistUI.h"
#include "SnM_VWnd.h"

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

    // Theme management
    void UpdateTheme();

protected:
    void GetItemText(SWS_ListItem* item, int iCol, char* str, int iStrMax);
    void OnItemPaint(LICE_IBitmap* drawbm, SWS_ListItem* item, const RECT& itemRect);
    void OnBeginDrag(SWS_ListItem* item);
    void OnDrag();
    void OnEndDrag();

private:
    ModernPlaylistItemRenderer::ItemVisualState GetItemState(SWS_ListItem* item, int index);
    ModernPlaylistItemRenderer::ItemData GetItemData(RgnPlaylistItem* item);

    void DrawDragGhost(LICE_IBitmap* drawbm);
    void ClearDragGhost();
    int GetDropTargetIndex();

    ModernPlaylistItemRenderer m_renderer;
    PlaylistTheme* m_theme;
    int m_itemHeight;
    int m_hoveredItem;
    bool m_modernRenderingEnabled;

    bool m_isDragging;
    int m_draggedItemIndex;
    int m_dropTargetIndex;
    LICE_IBitmap* m_dragGhostBitmap;
    POINT m_dragStartPos;
    POINT m_dragCurrentPos;

    WDL_TypedBuf<bool> m_dirtyItems;
    bool m_fullRepaintNeeded;
    int m_lastPlayingItem;
    int m_lastNextItem;

    LICE_IBitmap* m_offscreenBuffer;
    int m_bufferWidth;
    int m_bufferHeight;

    void MarkItemDirty(int index);
    void MarkAllItemsDirty();
    void ClearDirtyFlags();
    bool IsItemDirty(int index) const;

    void EnsureOffscreenBuffer(int width, int height);
    void ReleaseOffscreenBuffer();
};

///////////////////////////////////////////////////////////////////////////////
// ModernMonitoringView - Enhanced monitoring display
///////////////////////////////////////////////////////////////////////////////

class ModernMonitoringView : public SNM_FiveMonitors {
public:
    ModernMonitoringView();
    virtual ~ModernMonitoringView();

    void OnPaint(LICE_IBitmap* drawbm, int origin_x, int origin_y, RECT* cliprect, int rscale);

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
    void EnsureOffscreenBuffer(int width, int height);
    void ReleaseOffscreenBuffer();

    ProgressInfo m_progress;
    WDL_FastString m_currentName;
    WDL_FastString m_nextName;
    WDL_FastString m_playlistName;
    int m_currentNumber;
    int m_nextNumber;
    int m_playlistNumber;
    PlaylistTheme* m_theme;

    LICE_IBitmap* m_offscreenBuffer;
    int m_bufferWidth;
    int m_bufferHeight;
};

#endif // _SNM_MODERNPLAYLISTVIEW_H_
