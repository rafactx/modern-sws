/******************************************************************************
/ SnM_ModernPlaylistUI.cpp
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
#include "SnM_ModernPlaylistUI.h"

///////////////////////////////////////////////////////////////////////////////
// ModernPlaylistItemRenderer implementation
///////////////////////////////////////////////////////////////////////////////

ModernPlaylistItemRenderer::ModernPlaylistItemRenderer()
{
}

ModernPlaylistItemRenderer::~ModernPlaylistItemRenderer()
{
}

void ModernPlaylistItemRenderer::DrawItem(
    LICE_IBitmap* drawbm,
    const RECT& itemRect,
    const ItemData& data,
    const ItemVisualState& state,
    const PlaylistTheme* theme)
{
    if (!drawbm || !theme) {
        return;
    }

    // TODO: Implement full item rendering
    // This will be implemented in later tasks
}

void ModernPlaylistItemRenderer::DrawBackground(LICE_IBitmap* bm, const RECT& r, const ItemVisualState& state, const PlaylistTheme* theme)
{
    // TODO: Implement background drawing
}

void ModernPlaylistItemRenderer::DrawStatusIcon(LICE_IBitmap* bm, const RECT& r, const ItemVisualState& state)
{
    // TODO: Implement status icon drawing
}

void ModernPlaylistItemRenderer::DrawRegionNumber(LICE_IBitmap* bm, const RECT& r, int number, const PlaylistTheme* theme)
{
    // TODO: Implement region number drawing
}

void ModernPlaylistItemRenderer::DrawRegionName(LICE_IBitmap* bm, const RECT& r, const char* name, const PlaylistTheme* theme)
{
    // TODO: Implement region name drawing
}

void ModernPlaylistItemRenderer::DrawTimeInfo(LICE_IBitmap* bm, const RECT& r, const ItemData& data, const PlaylistTheme* theme)
{
    // TODO: Implement time info drawing
}

void ModernPlaylistItemRenderer::DrawLoopBadge(LICE_IBitmap* bm, const RECT& r, int count, bool infinite, const PlaylistTheme* theme)
{
    // TODO: Implement loop badge drawing
}

void ModernPlaylistItemRenderer::DrawRoundedRect(LICE_IBitmap* bm, const RECT& r, int radius, int color)
{
    // TODO: Implement rounded rectangle drawing
}

///////////////////////////////////////////////////////////////////////////////
// ModernRegionPlaylistView implementation
///////////////////////////////////////////////////////////////////////////////

ModernRegionPlaylistView::ModernRegionPlaylistView(HWND hwndList, HWND hwndEdit)
    : RegionPlaylistView(hwndList, hwndEdit)
    , m_theme(NULL)
    , m_itemHeight(32)
    , m_hoveredItem(-1)
    , m_modernRenderingEnabled(false)
{
    m_theme = PlaylistTheme::GetInstance();
    if (m_theme) {
        m_theme->UpdateTheme();
    }
}

ModernRegionPlaylistView::~ModernRegionPlaylistView()
{
}

void ModernRegionPlaylistView::SetItemHeight(int height)
{
    m_itemHeight = height;
}

void ModernRegionPlaylistView::EnableModernRendering(bool enable)
{
    m_modernRenderingEnabled = enable;
}

void ModernRegionPlaylistView::OnMouseMove(int x, int y)
{
    // TODO: Implement hover tracking
}

void ModernRegionPlaylistView::GetItemText(SWS_ListItem* item, int iCol, char* str, int iStrMax)
{
    // Call base class implementation
    RegionPlaylistView::GetItemText(item, iCol, str, iStrMax);
}

void ModernRegionPlaylistView::OnItemPaint(LICE_IBitmap* drawbm, SWS_ListItem* item, const RECT& itemRect)
{
    if (!drawbm || !item || !m_modernRenderingEnabled) {
        // Fallback to default rendering
        return;
    }

    // TODO: Implement modern item painting
    // This will be implemented in later tasks
}

void ModernRegionPlaylistView::UpdateTheme()
{
    if (m_theme) {
        m_theme->UpdateTheme();
    }
}

ModernPlaylistItemRenderer::ItemVisualState ModernRegionPlaylistView::GetItemState(SWS_ListItem* item, int index)
{
    ModernPlaylistItemRenderer::ItemVisualState state;

    // TODO: Implement state detection
    // This will be implemented in later tasks

    return state;
}

ModernPlaylistItemRenderer::ItemData ModernRegionPlaylistView::GetItemData(RgnPlaylistItem* item)
{
    ModernPlaylistItemRenderer::ItemData data;

    if (!item) {
        return data;
    }

    // TODO: Implement data extraction
    // This will be implemented in later tasks

    return data;
}

///////////////////////////////////////////////////////////////////////////////
// ModernMonitoringView implementation
///////////////////////////////////////////////////////////////////////////////

ModernMonitoringView::ModernMonitoringView()
    : SNM_FiveMonitors()
    , m_currentNumber(0)
    , m_nextNumber(0)
    , m_theme(NULL)
{
    m_theme = PlaylistTheme::GetInstance();
}

ModernMonitoringView::~ModernMonitoringView()
{
}

void ModernMonitoringView::OnPaint(LICE_IBitmap* drawbm, int origin_x, int origin_y, RECT* cliprect, int rscale)
{
    if (!drawbm) {
        return;
    }

    // TODO: Implement modern monitoring view painting
    // This will be implemented in later tasks

    // For now, call base class
    SNM_FiveMonitors::OnPaint(drawbm, origin_x, origin_y, cliprect, rscale);
}

void ModernMonitoringView::SetProgress(double current, double total)
{
    m_progress.current = current;
    m_progress.total = total;

    if (total > 0.0) {
        m_progress.percentage = (current / total) * 100.0;
    } else {
        m_progress.percentage = 0.0;
    }
}

void ModernMonitoringView::SetCurrentRegion(const char* name, int number)
{
    if (name) {
        m_currentName.Set(name);
    }
    m_currentNumber = number;
}

void ModernMonitoringView::SetNextRegion(const char* name, int number)
{
    if (name) {
        m_nextName.Set(name);
    }
    m_nextNumber = number;
}

void ModernMonitoringView::SetPlaylistInfo(const char* playlistName, int playlistNumber)
{
    // TODO: Store playlist info
    // This will be implemented in later tasks
}

void ModernMonitoringView::DrawProgressBar(LICE_IBitmap* bm, const RECT& r)
{
    // TODO: Implement progress bar drawing
}

void ModernMonitoringView::DrawLargeText(LICE_IBitmap* bm, const RECT& r, const char* text, LICE_CachedFont* font, int color)
{
    // TODO: Implement large text drawing
}
