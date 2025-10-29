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

    // Validate inputs
    if (!state.IsValid() || !data.IsValid()) {
        // Fallback: draw a simple error indicator
        LICE_FillRect(drawbm, itemRect.left, itemRect.top,
                     itemRect.right - itemRect.left,
                     itemRect.bottom - itemRect.top,
                     LICE_RGBA(64, 64, 64, 255), 1.0f, LICE_BLIT_MODE_COPY);
        return;
    }

    try {
        // Apply 4px spacing between items by adjusting the rect
        RECT drawRect = itemRect;
        const int ITEM_SPACING = 4;
        drawRect.bottom -= ITEM_SPACING;

        // Ensure we have a valid rectangle
        if (drawRect.bottom <= drawRect.top || drawRect.right <= drawRect.left) {
            return;
        }

        // Step 1: Draw background with rounded corners and appropriate color
        DrawBackground(drawbm, drawRect, state, theme);

        // Step 2: Draw status icon (play, next, or warning)
        if (state.NeedsStatusIcon()) {
            DrawStatusIcon(drawbm, drawRect, state);
        }

        // Step 3: Draw region number
        DrawRegionNumber(drawbm, drawRect, data.regionNumber, theme);

        // Step 4: Draw region name (with ellipsis if too long)
        DrawRegionName(drawbm, drawRect, data.regionName.Get(), theme);

        // Step 5: Draw time information
        DrawTimeInfo(drawbm, drawRect, data, theme);

        // Step 6: Draw loop badge if applicable
        if (state.hasInfiniteLoop || state.loopCount > 1) {
            DrawLoopBadge(drawbm, drawRect, state.loopCount, state.hasInfiniteLoop, theme);
        }
    }
    catch (...) {
        // Fallback rendering in case of any errors
        // Draw a simple rectangle to indicate the item exists
        const PlaylistTheme::Colors& colors = theme->GetColors();
        LICE_FillRect(drawbm, itemRect.left, itemRect.top,
                     itemRect.right - itemRect.left,
                     itemRect.bottom - itemRect.top - 4,
                     colors.background, 1.0f, LICE_BLIT_MODE_COPY);

        // Draw a simple text fallback
        if (theme->GetFonts().itemName) {
            char fallbackText[256];
            snprintf(fallbackText, sizeof(fallbackText), "%d. %s",
                    data.regionNumber, data.regionName.Get());
            LICE_DrawText(drawbm, itemRect.left + 8, itemRect.top + 8,
                         fallbackText, colors.text, 1.0f, LICE_BLIT_MODE_COPY,
                         theme->GetFonts().itemName);
        }
    }
}

void ModernPlaylistItemRenderer::DrawBackground(LICE_IBitmap* bm, const RECT& r, const ItemVisualState& state, const PlaylistTheme* theme)
{
    if (!bm || !theme) {
        return;
    }

    const PlaylistTheme::Colors& colors = theme->GetColors();
    int bgColor = colors.background;

    // Determine background color based on state priority
    // Priority: Playing > Next > Selected > Hovered > Default
    if (state.isPlaying) {
        bgColor = colors.currentItemBg;
    }
    else if (state.isNext) {
        bgColor = colors.nextItemBg;
    }
    else if (state.isSelected) {
        bgColor = colors.selectedBg;
    }
    else if (state.isHovered) {
        bgColor = colors.hoverBg;
    }

    // Draw rounded rectangle background with 4px radius
    const int CORNER_RADIUS = 4;
    DrawRoundedRect(bm, r, CORNER_RADIUS, bgColor);

    // Add subtle border for depth (1px darker/lighter than background)
    int borderColor = colors.border;

    // Draw border as a rounded rectangle outline
    // We'll draw a slightly larger rounded rect and then draw the filled one on top
    RECT borderRect = r;
    borderRect.left -= 1;
    borderRect.top -= 1;
    borderRect.right += 1;
    borderRect.bottom += 1;

    // Only draw border if we have space
    if (borderRect.left >= 0 && borderRect.top >= 0) {
        // Draw thin border by drawing lines around the rounded rect
        // Top border
        LICE_Line(bm, r.left + CORNER_RADIUS, r.top, r.right - CORNER_RADIUS, r.top, borderColor, 0.3f, LICE_BLIT_MODE_COPY, false);
        // Bottom border
        LICE_Line(bm, r.left + CORNER_RADIUS, r.bottom - 1, r.right - CORNER_RADIUS, r.bottom - 1, borderColor, 0.3f, LICE_BLIT_MODE_COPY, false);
        // Left border
        LICE_Line(bm, r.left, r.top + CORNER_RADIUS, r.left, r.bottom - CORNER_RADIUS, borderColor, 0.3f, LICE_BLIT_MODE_COPY, false);
        // Right border
        LICE_Line(bm, r.right - 1, r.top + CORNER_RADIUS, r.right - 1, r.bottom - CORNER_RADIUS, borderColor, 0.3f, LICE_BLIT_MODE_COPY, false);
    }
}

void ModernPlaylistItemRenderer::DrawStatusIcon(LICE_IBitmap* bm, const RECT& r, const ItemVisualState& state)
{
    if (!bm) {
        return;
    }

    // Only draw icon if needed
    if (!state.NeedsStatusIcon()) {
        return;
    }

    PlaylistIconManager* iconMgr = PlaylistIconManager::GetInstance();
    if (!iconMgr) {
        return;
    }

    // Position icon at left side with padding
    const int ICON_SIZE = 16; // Minimum 16x16 as per requirements
    const int LEFT_PADDING = 8;

    int iconX = r.left + LEFT_PADDING;
    int iconY = r.top + (r.bottom - r.top - ICON_SIZE) / 2; // Center vertically

    // Determine which icon to draw based on priority
    // Priority: Sync Loss > Playing > Next
    PlaylistIconManager::IconType iconType;

    if (state.isSyncLoss) {
        iconType = PlaylistIconManager::ICON_SYNC_LOSS;
    }
    else if (state.isPlaying) {
        iconType = PlaylistIconManager::ICON_PLAY;
    }
    else if (state.isNext) {
        iconType = PlaylistIconManager::ICON_NEXT;
    }
    else {
        return; // No icon to draw
    }

    // Draw the icon
    iconMgr->DrawIcon(bm, iconType, iconX, iconY, ICON_SIZE);
}

void ModernPlaylistItemRenderer::DrawRegionNumber(LICE_IBitmap* bm, const RECT& r, int number, const PlaylistTheme* theme)
{
    if (!bm || !theme || number <= 0) {
        return;
    }

    const PlaylistTheme::Fonts& fonts = theme->GetFonts();
    const PlaylistTheme::Colors& colors = theme->GetColors();

    if (!fonts.itemNumber) {
        return;
    }

    // Format the region number
    char numStr[32];
    snprintf(numStr, sizeof(numStr), "%d.", number);

    // Position after status icon with proper spacing
    const int ICON_SIZE = 16;
    const int LEFT_PADDING = 8;
    const int ICON_SPACING = 8;
    const int NUMBER_X = r.left + LEFT_PADDING + ICON_SIZE + ICON_SPACING;

    // Center vertically
    int textY = r.top + (r.bottom - r.top) / 2;

    // Draw the region number with 14pt font (itemNumber)
    LICE_DrawText(bm, NUMBER_X, textY, numStr, colors.text, 1.0f, LICE_BLIT_MODE_COPY, fonts.itemNumber);
}

void ModernPlaylistItemRenderer::DrawRegionName(LICE_IBitmap* bm, const RECT& r, const char* name, const PlaylistTheme* theme)
{
    if (!bm || !theme || !name || !name[0]) {
        return;
    }

    const PlaylistTheme::Fonts& fonts = theme->GetFonts();
    const PlaylistTheme::Colors& colors = theme->GetColors();

    if (!fonts.itemName) {
        return;
    }

    // Calculate position after region number
    const int ICON_SIZE = 16;
    const int LEFT_PADDING = 8;
    const int ICON_SPACING = 8;
    const int NUMBER_WIDTH = 40; // Approximate width for region number (e.g., "999.")
    const int NAME_X = r.left + LEFT_PADDING + ICON_SIZE + ICON_SPACING + NUMBER_WIDTH;

    // Reserve space for time info on the right
    const int TIME_WIDTH = 80; // Approximate width for time display
    const int RIGHT_PADDING = 8;
    const int AVAILABLE_WIDTH = r.right - NAME_X - TIME_WIDTH - RIGHT_PADDING;

    if (AVAILABLE_WIDTH <= 0) {
        return; // Not enough space
    }

    // Center vertically
    int textY = r.top + (r.bottom - r.top) / 2;

    // Truncate name if too long
    WDL_FastString displayName;
    displayName.Set(name);

    // Measure text width
    RECT textRect = {0, 0, 0, 0};
    LICE_MeasureText(displayName.Get(), &textRect.right, &textRect.bottom, fonts.itemName);

    // If text is too wide, truncate with ellipsis
    if (textRect.right > AVAILABLE_WIDTH) {
        const char* ellipsis = "...";
        RECT ellipsisRect = {0, 0, 0, 0};
        LICE_MeasureText(ellipsis, &ellipsisRect.right, &ellipsisRect.bottom, fonts.itemName);

        // Binary search for the right length
        int len = displayName.GetLength();
        while (len > 0 && textRect.right + ellipsisRect.right > AVAILABLE_WIDTH) {
            len--;
            displayName.SetLen(len);
            displayName.Append(ellipsis);

            textRect.right = 0;
            textRect.bottom = 0;
            LICE_MeasureText(displayName.Get(), &textRect.right, &textRect.bottom, fonts.itemName);

            // Remove ellipsis for next iteration
            displayName.SetLen(len);
        }

        if (len > 0) {
            displayName.Append(ellipsis);
        }
    }

    // Draw the region name with 12pt font (itemName)
    LICE_DrawText(bm, NAME_X, textY, displayName.Get(), colors.text, 1.0f, LICE_BLIT_MODE_COPY, fonts.itemName);
}

void ModernPlaylistItemRenderer::DrawTimeInfo(LICE_IBitmap* bm, const RECT& r, const ItemData& data, const PlaylistTheme* theme)
{
    if (!bm || !theme) {
        return;
    }

    const PlaylistTheme::Fonts& fonts = theme->GetFonts();
    const PlaylistTheme::Colors& colors = theme->GetColors();

    if (!fonts.itemTime) {
        return;
    }

    // Format duration as MM:SS
    int totalSeconds = (int)data.duration;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    char timeStr[32];
    snprintf(timeStr, sizeof(timeStr), "%d:%02d", minutes, seconds);

    // Measure text to position it at the right side
    RECT textRect = {0, 0, 0, 0};
    LICE_MeasureText(timeStr, &textRect.right, &textRect.bottom, fonts.itemTime);

    // Position at right side with padding
    const int RIGHT_PADDING = 8;
    const int LOOP_BADGE_SPACE = 40; // Reserve space for loop badge if present
    int timeX = r.right - textRect.right - RIGHT_PADDING - LOOP_BADGE_SPACE;

    // Center vertically
    int textY = r.top + (r.bottom - r.top) / 2;

    // Draw the time with 11pt font (itemTime)
    LICE_DrawText(bm, timeX, textY, timeStr, colors.text, 1.0f, LICE_BLIT_MODE_COPY, fonts.itemTime);

    // TODO: Show elapsed/remaining time if item is playing
    // This would require additional state information about current playback position
    // For now, we just show the total duration
}

void ModernPlaylistItemRenderer::DrawLoopBadge(LICE_IBitmap* bm, const RECT& r, int count, bool infinite, const PlaylistTheme* theme)
{
    if (!bm || !theme) {
        return;
    }

    // Only draw badge if loop count > 1 or infinite
    if (count <= 1 && !infinite) {
        return;
    }

    const PlaylistTheme::Colors& colors = theme->GetColors();
    const PlaylistTheme::Fonts& fonts = theme->GetFonts();

    // Position near time info (at the far right)
    const int RIGHT_PADDING = 8;
    const int BADGE_SIZE = 24; // Size for badge
    const int ICON_SIZE = 14; // Minimum 14x14 for infinity symbol

    int badgeX = r.right - BADGE_SIZE - RIGHT_PADDING;
    int badgeY = r.top + (r.bottom - r.top - BADGE_SIZE) / 2;

    if (infinite) {
        // Draw infinity symbol
        PlaylistIconManager* iconMgr = PlaylistIconManager::GetInstance();
        if (iconMgr) {
            int iconX = badgeX + (BADGE_SIZE - ICON_SIZE) / 2;
            int iconY = badgeY + (BADGE_SIZE - ICON_SIZE) / 2;
            iconMgr->DrawIcon(bm, PlaylistIconManager::ICON_LOOP_INFINITE, iconX, iconY, ICON_SIZE);
        }
    }
    else {
        // Draw loop count badge with contrasting background
        RECT badgeRect;
        badgeRect.left = badgeX;
        badgeRect.top = badgeY;
        badgeRect.right = badgeX + BADGE_SIZE;
        badgeRect.bottom = badgeY + BADGE_SIZE;

        // Draw badge background with accent color
        DrawRoundedRect(bm, badgeRect, 3, colors.accentBlue);

        // Draw loop count text
        if (fonts.itemTime) {
            char countStr[16];
            snprintf(countStr, sizeof(countStr), "x%d", count);

            // Measure text to center it
            RECT textRect = {0, 0, 0, 0};
            LICE_MeasureText(countStr, &textRect.right, &textRect.bottom, fonts.itemTime);

            int textX = badgeX + (BADGE_SIZE - textRect.right) / 2;
            int textY = badgeY + (BADGE_SIZE - textRect.bottom) / 2;

            // Use white text for contrast on accent background
            int textColor = LICE_RGBA(255, 255, 255, 255);
            LICE_DrawText(bm, textX, textY, countStr, textColor, 1.0f, LICE_BLIT_MODE_COPY, fonts.itemTime);
        }
    }
}

void ModernPlaylistItemRenderer::DrawRoundedRect(LICE_IBitmap* bm, const RECT& r, int radius, int color)
{
    if (!bm || radius < 0) {
        return;
    }

    // Validate rectangle
    if (r.right <= r.left || r.bottom <= r.top) {
        return;
    }

    int width = r.right - r.left;
    int height = r.bottom - r.top;

    // Clamp radius to half of the smallest dimension
    int maxRadius = (width < height ? width : height) / 2;
    if (radius > maxRadius) {
        radius = maxRadius;
    }

    // If radius is 0 or very small, just draw a regular rectangle
    if (radius <= 1) {
        LICE_FillRect(bm, r.left, r.top, width, height, color, 1.0f, LICE_BLIT_MODE_COPY);
        return;
    }

    // Draw the main body (center rectangle)
    LICE_FillRect(bm, r.left + radius, r.top, width - 2 * radius, height, color, 1.0f, LICE_BLIT_MODE_COPY);

    // Draw left and right side rectangles
    LICE_FillRect(bm, r.left, r.top + radius, radius, height - 2 * radius, color, 1.0f, LICE_BLIT_MODE_COPY);
    LICE_FillRect(bm, r.right - radius, r.top + radius, radius, height - 2 * radius, color, 1.0f, LICE_BLIT_MODE_COPY);

    // Draw rounded corners using circles
    // Top-left corner
    LICE_FillCircle(bm, r.left + radius, r.top + radius, (float)radius, color, 1.0f, LICE_BLIT_MODE_COPY, true);

    // Top-right corner
    LICE_FillCircle(bm, r.right - radius - 1, r.top + radius, (float)radius, color, 1.0f, LICE_BLIT_MODE_COPY, true);

    // Bottom-left corner
    LICE_FillCircle(bm, r.left + radius, r.bottom - radius - 1, (float)radius, color, 1.0f, LICE_BLIT_MODE_COPY, true);

    // Bottom-right corner
    LICE_FillCircle(bm, r.right - radius - 1, r.bottom - radius - 1, (float)radius, color, 1.0f, LICE_BLIT_MODE_COPY, true);
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
