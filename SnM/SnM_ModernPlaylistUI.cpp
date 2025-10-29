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
    , m_itemHeight(40) // Default height for modern items (minimum for readability)
    , m_hoveredItem(-1)
    , m_modernRenderingEnabled(true) // Enable modern rendering by default
    , m_isDragging(false)
    , m_draggedItemIndex(-1)
    , m_dropTargetIndex(-1)
    , m_dragGhostBitmap(NULL)
{
    // Initialize theme
    m_theme = PlaylistTheme::GetInstance();
    if (m_theme) {
        m_theme->UpdateTheme();
    }

    // Load modern rendering preference from reaper.ini
    char key[64];
    snprintf(key, sizeof(key), "sws_playlist_modern_ui");
    char value[32] = "";
    GetPrivateProfileString("SWS", key, "1", value, sizeof(value), get_ini_file());
    m_modernRenderingEnabled = (atoi(value) != 0);

    // Initialize drag start position
    m_dragStartPos.x = 0;
    m_dragStartPos.y = 0;
    m_dragCurrentPos.x = 0;
    m_dragCurrentPos.y = 0;

    // Initialize renderer (no special initialization needed)
}

ModernRegionPlaylistView::~ModernRegionPlaylistView()
{
    // Clean up drag ghost bitmap if it exists
    if (m_dragGhostBitmap) {
        delete m_dragGhostBitmap;
        m_dragGhostBitmap = NULL;
    }
}

void ModernRegionPlaylistView::SetItemHeight(int height)
{
    // Ensure minimum height for readability
    // Minimum height should accommodate:
    // - 14pt region number font
    // - 12pt region name font
    // - 16x16 icon
    // - Padding
    const int MIN_HEIGHT = 32;
    const int MAX_HEIGHT = 200; // Reasonable maximum

    // Clamp height to valid range
    if (height < MIN_HEIGHT) {
        height = MIN_HEIGHT;
    } else if (height > MAX_HEIGHT) {
        height = MAX_HEIGHT;
    }

    // Only update if changed
    if (m_itemHeight != height) {
        m_itemHeight = height;

        // Trigger layout update when height changes
        if (m_hwndList) {
            // For Windows ListView, we need to use LVM_SETICONSPACING or similar
            // However, standard ListView doesn't support variable item heights easily
            // We would need to use owner-draw or custom control
            // For now, we'll just invalidate to trigger a repaint
            InvalidateRect(m_hwndList, NULL, TRUE);
            UpdateWindow(m_hwndList);
        }
    }
}

void ModernRegionPlaylistView::EnableModernRendering(bool enable)
{
    // Only update if changed
    if (m_modernRenderingEnabled != enable) {
        m_modernRenderingEnabled = enable;

        // Save preference to reaper.ini
        char key[64];
        snprintf(key, sizeof(key), "sws_playlist_modern_ui");
        WritePrivateProfileString("SWS", key, enable ? "1" : "0", get_ini_file());

        // Trigger full repaint to show the change
        if (m_hwndList) {
            InvalidateRect(m_hwndList, NULL, TRUE);
            UpdateWindow(m_hwndList);
        }

        // If disabling modern rendering, we fall back to classic rendering
        // which is handled by the base RegionPlaylistView class
    }
}

void ModernRegionPlaylistView::OnMouseMove(int x, int y)
{
    if (!m_modernRenderingEnabled) {
        return;
    }

    // Get the item at the mouse position
    POINT pt = {x, y};
    int newHoveredItem = -1;

    // Convert screen coordinates to client coordinates if needed
    // and find which item is under the mouse
    if (m_hwndList) {
        // Use hit test to find the item
        LVHITTESTINFO hitTest;
        hitTest.pt = pt;
        hitTest.flags = 0;
        hitTest.iItem = -1;
        hitTest.iSubItem = 0;

        // Perform hit test
        int hitIndex = ListView_HitTest(m_hwndList, &hitTest);
        if (hitIndex >= 0 && (hitTest.flags & LVHT_ONITEM)) {
            newHoveredItem = hitIndex;
        }
    }

    // Check if hover state changed
    if (newHoveredItem != m_hoveredItem) {
        int oldHoveredItem = m_hoveredItem;
        m_hoveredItem = newHoveredItem;

        // Trigger repaint for the affected items
        // Note: In a real implementation, we would need to invalidate
        // only the specific item rectangles for efficiency
        // For now, we'll trigger a full update
        if (m_hwndList) {
            // Invalidate old hovered item
            if (oldHoveredItem >= 0) {
                ListView_RedrawItems(m_hwndList, oldHoveredItem, oldHoveredItem);
            }

            // Invalidate new hovered item
            if (m_hoveredItem >= 0) {
                ListView_RedrawItems(m_hwndList, m_hoveredItem, m_hoveredItem);
            }

            // Update the window
            UpdateWindow(m_hwndList);
        }
    }

    // Note: The 150ms transition for hover effect would be implemented
    // using a timer and gradual color interpolation. For now, we have
    // immediate hover feedback. The transition can be added in a future
    // enhancement by:
    // 1. Starting a timer when hover changes
    // 2. Interpolating colors over 150ms
    // 3. Repainting during the transition
}

void ModernRegionPlaylistView::GetItemText(SWS_ListItem* item, int iCol, char* str, int iStrMax)
{
    // Validate inputs
    if (!str || iStrMax <= 0) {
        return;
    }

    // Initialize output
    *str = '\0';

    // Handle null/invalid items gracefully
    if (!item) {
        return;
    }

    // Cast to RgnPlaylistItem
    RgnPlaylistItem* pItem = (RgnPlaylistItem*)item;
    if (!pItem || !pItem->IsValidIem()) {
        // Invalid item - return empty string or error indicator
        if (iStrMax > 1) {
            lstrcpyn(str, "-", iStrMax);
        }
        return;
    }

    // Call base class implementation for compatibility
    // The base class handles all the column formatting
    RegionPlaylistView::GetItemText(item, iCol, str, iStrMax);

    // Add modern formatting if needed
    // For now, we maintain compatibility with the base class
    // Future enhancements could add special formatting here
}

void ModernRegionPlaylistView::OnItemPaint(LICE_IBitmap* drawbm, SWS_ListItem* item, const RECT& itemRect)
{
    // Check if modern rendering is enabled
    if (!m_modernRenderingEnabled) {
        // Fallback to default rendering - do nothing, let base class handle it
        return;
    }

    // Validate inputs
    if (!drawbm || !item) {
        return;
    }

    // Validate rectangle
    if (itemRect.right <= itemRect.left || itemRect.bottom <= itemRect.top) {
        return;
    }

    try {
        // Cast to RgnPlaylistItem
        RgnPlaylistItem* plItem = static_cast<RgnPlaylistItem*>(item);
        if (!plItem || !plItem->IsValidIem()) {
            // Invalid item - draw error indicator
            LICE_FillRect(drawbm, itemRect.left, itemRect.top,
                         itemRect.right - itemRect.left,
                         itemRect.bottom - itemRect.top,
                         LICE_RGBA(64, 64, 64, 255), 1.0f, LICE_BLIT_MODE_COPY);
            return;
        }

        // Get item data and visual state
        ModernPlaylistItemRenderer::ItemData data = GetItemData(plItem);

        // Find the index of this item in the list
        int itemIndex = -1;
        if (RegionPlaylist* pl = GetPlaylist()) {
            itemIndex = pl->Find(plItem);
        }

        ModernPlaylistItemRenderer::ItemVisualState state = GetItemState(item, itemIndex);

        // Validate data before rendering
        if (!data.IsValid()) {
            // Draw fallback for invalid data
            if (m_theme) {
                const PlaylistTheme::Colors& colors = m_theme->GetColors();
                LICE_FillRect(drawbm, itemRect.left, itemRect.top,
                             itemRect.right - itemRect.left,
                             itemRect.bottom - itemRect.top - 4,
                             colors.background, 1.0f, LICE_BLIT_MODE_COPY);
            }
            return;
        }

        // Call renderer to draw the item
        m_renderer.DrawItem(drawbm, itemRect, data, state, m_theme);
    }
    catch (...) {
        // Fallback rendering in case of any errors
        // Draw a simple rectangle to indicate the item exists
        if (m_theme) {
            const PlaylistTheme::Colors& colors = m_theme->GetColors();
            LICE_FillRect(drawbm, itemRect.left, itemRect.top,
                         itemRect.right - itemRect.left,
                         itemRect.bottom - itemRect.top - 4,
                         colors.background, 1.0f, LICE_BLIT_MODE_COPY);
        }
    }
}

void ModernRegionPlaylistView::UpdateTheme()
{
    if (!m_theme) {
        // Re-initialize theme if it was somehow lost
        m_theme = PlaylistTheme::GetInstance();
    }

    if (m_theme) {
        // Detect theme changes from REAPER and reload colors and fonts
        m_theme->UpdateTheme();

        // Trigger full repaint to apply new theme
        if (m_hwndList) {
            InvalidateRect(m_hwndList, NULL, TRUE);
            UpdateWindow(m_hwndList);
        }
    }
}

ModernPlaylistItemRenderer::ItemVisualState ModernRegionPlaylistView::GetItemState(SWS_ListItem* item, int index)
{
    ModernPlaylistItemRenderer::ItemVisualState state;

    if (!item) {
        return state;
    }

    // Cast to RgnPlaylistItem
    RgnPlaylistItem* plItem = static_cast<RgnPlaylistItem*>(item);
    if (!plItem) {
        return state;
    }

    // Get current playlist
    RegionPlaylist* curpl = GetPlaylist();
    if (!curpl) {
        return state;
    }

    // Determine if this is the currently playing playlist
    bool isCurrentPlaylist = (g_playPlaylist >= 0 && curpl == GetPlaylist(g_playPlaylist));

    // Check if item is currently playing
    if (isCurrentPlaylist && !g_unsync && g_playCur >= 0) {
        RgnPlaylistItem* playingItem = curpl->Get(g_playCur);
        if (playingItem == plItem) {
            state.isPlaying = true;
        }
    }

    // Check if item is next to play
    if (isCurrentPlaylist && g_playNext >= 0) {
        RgnPlaylistItem* nextItem = curpl->Get(g_playNext);
        if (nextItem == plItem) {
            state.isNext = true;
        }
    }

    // Check if item is selected
    if (m_hwndList && index >= 0) {
        UINT itemState = ListView_GetItemState(m_hwndList, index, LVIS_SELECTED);
        state.isSelected = (itemState & LVIS_SELECTED) != 0;
    }

    // Check if item is hovered (but not if we're dragging - drop target takes priority)
    if (index >= 0 && index == m_hoveredItem && !m_isDragging) {
        state.isHovered = true;
    }

    // Check if item is the drop target during drag operation
    // This takes priority over hover state
    if (m_isDragging && index >= 0 && index == m_dropTargetIndex) {
        // Use hover state to highlight drop target with distinct color
        state.isHovered = true;
    }

    // Check for sync loss state
    // Sync loss occurs when we're playing but not in sync with the playlist
    if (isCurrentPlaylist && g_unsync) {
        state.isSyncLoss = true;
    }

    // Get loop count and infinite loop flag
    if (plItem->m_cnt < 0) {
        state.hasInfiniteLoop = true;
        state.loopCount = abs(plItem->m_cnt);
    } else {
        state.hasInfiniteLoop = false;
        state.loopCount = plItem->m_cnt;
    }

    // Additional check: if this is the currently playing item and we have a region loop
    if (state.isPlaying && g_rgnLoop != 0) {
        if (g_rgnLoop < 0) {
            state.hasInfiniteLoop = true;
        }
        // Note: g_rgnLoop represents the remaining loops, not the total count
        // We use the item's loop count for display purposes
    }

    return state;
}

void ModernRegionPlaylistView::AutoScrollToCurrentItem()
{
    if (!m_hwndList) {
        return;
    }

    // Get current playlist
    RegionPlaylist* curpl = GetPlaylist();
    if (!curpl) {
        return;
    }

    // Check if we're playing the current playlist
    if (g_playPlaylist < 0 || curpl != GetPlaylist(g_playPlaylist)) {
        return; // Not playing this playlist
    }

    // Detect when currently playing item changes
    int currentItemIndex = g_playCur;
    int nextItemIndex = g_playNext;

    // Scroll list to keep current item visible
    if (currentItemIndex >= 0 && currentItemIndex < ListView_GetItemCount(m_hwndList)) {
        // Check if current item is visible
        RECT itemRect;
        if (ListView_GetItemRect(m_hwndList, currentItemIndex, &itemRect, LVIR_BOUNDS)) {
            RECT clientRect;
            GetClientRect(m_hwndList, &clientRect);

            // Check if item is fully visible
            bool isVisible = (itemRect.top >= clientRect.top &&
                            itemRect.bottom <= clientRect.bottom);

            if (!isVisible) {
                // Scroll to make current item visible
                // Try to position it in the middle of the view if possible
                ListView_EnsureVisible(m_hwndList, currentItemIndex, FALSE);
            }
        }

        // Ensure next item is also visible if possible
        if (nextItemIndex >= 0 && nextItemIndex < ListView_GetItemCount(m_hwndList)) {
            RECT nextItemRect;
            if (ListView_GetItemRect(m_hwndList, nextItemIndex, &nextItemRect, LVIR_BOUNDS)) {
                RECT clientRect;
                GetClientRect(m_hwndList, &clientRect);

                // Check if next item is visible
                bool isNextVisible = (nextItemRect.top >= clientRect.top &&
                                     nextItemRect.bottom <= clientRect.bottom);

                // If current is visible but next is not, try to show both
                if (!isNextVisible) {
                    // Calculate if we can show both items
                    int itemHeight = nextItemRect.bottom - nextItemRect.top;
                    int clientHeight = clientRect.bottom - clientRect.top;

                    // If there's room for both items, scroll to show both
                    if (itemHeight * 2 < clientHeight) {
                        // Scroll to show the current item at the top
                        ListView_EnsureVisible(m_hwndList, nextItemIndex, FALSE);
                        ListView_EnsureVisible(m_hwndList, currentItemIndex, FALSE);
                    }
                }
            }
        }
    }
}

ModernPlaylistItemRenderer::ItemData ModernRegionPlaylistView::GetItemData(RgnPlaylistItem* item)
{
    ModernPlaylistItemRenderer::ItemData data;

    if (!item) {
        return data;
    }

    // Validate item
    if (!item->IsValidIem()) {
        return data;
    }

    // Extract region ID
    int regionId = item->m_rgnId;
    if (regionId <= 0) {
        return data;
    }

    // Get region number from ID
    data.regionNumber = GetMarkerRegionNumFromId(regionId);

    // Get region name
    char nameBuf[256] = "";
    if (EnumMarkerRegionDescById(NULL, regionId, nameBuf, sizeof(nameBuf), SNM_REGION_MASK, false, true, false) >= 0) {
        data.regionName.Set(nameBuf);
    } else {
        // Fallback if region name cannot be retrieved
        data.regionName.Set("Unknown Region");
    }

    // Get start and end positions
    double startPos = 0.0;
    double endPos = 0.0;

    if (EnumMarkerRegionById(NULL, regionId, NULL, &startPos, &endPos, NULL, NULL, NULL) >= 0) {
        data.startTime = startPos;
        data.endTime = endPos;

        // Calculate duration
        if (endPos >= startPos) {
            data.duration = endPos - startPos;
        } else {
            // Invalid timing - set to 0
            data.duration = 0.0;
        }
    } else {
        // Failed to get region timing
        data.startTime = 0.0;
        data.endTime = 0.0;
        data.duration = 0.0;
    }

    // Validate data before returning
    // If region number is 0 or negative, the data is invalid
    if (data.regionNumber <= 0) {
        // Return empty/invalid data
        ModernPlaylistItemRenderer::ItemData invalidData;
        return invalidData;
    }

    // If region name is empty, set a default
    if (data.regionName.GetLength() == 0) {
        data.regionName.SetFormatted(32, "Region %d", data.regionNumber);
    }

    return data;
}

///////////////////////////////////////////////////////////////////////////////
// Drag and drop visual feedback implementation
///////////////////////////////////////////////////////////////////////////////

void ModernRegionPlaylistView::OnBeginDrag(SWS_ListItem* item)
{
    // Call base class implementation first to maintain functionality
    RegionPlaylistView::OnBeginDrag(item);

    // Only add visual feedback if modern rendering is enabled
    if (!m_modernRenderingEnabled || !item || !m_hwndList) {
        return;
    }

    // Get the index of the dragged item
    RegionPlaylist* pl = GetPlaylist();
    if (!pl) {
        return;
    }

    RgnPlaylistItem* plItem = static_cast<RgnPlaylistItem*>(item);
    if (!plItem) {
        return;
    }

    m_draggedItemIndex = pl->Find(plItem);
    if (m_draggedItemIndex < 0) {
        return;
    }

    // Set dragging state
    m_isDragging = true;
    m_dropTargetIndex = -1;

    // Get cursor position
    GetCursorPos(&m_dragStartPos);
    m_dragCurrentPos = m_dragStartPos;

    // Create ghost image of dragged item
    // Get the item rectangle
    RECT itemRect;
    if (ListView_GetItemRect(m_hwndList, m_draggedItemIndex, &itemRect, LVIR_BOUNDS)) {
        int width = itemRect.right - itemRect.left;
        int height = itemRect.bottom - itemRect.top;

        // Create bitmap for ghost image
        if (m_dragGhostBitmap) {
            delete m_dragGhostBitmap;
        }
        m_dragGhostBitmap = new LICE_SysBitmap(width, height);

        if (m_dragGhostBitmap) {
            // Clear the bitmap with transparent background
            LICE_Clear(m_dragGhostBitmap, 0);

            // Get item data and state
            ModernPlaylistItemRenderer::ItemData data = GetItemData(plItem);
            ModernPlaylistItemRenderer::ItemVisualState state = GetItemState(item, m_draggedItemIndex);

            // Render the item to the ghost bitmap
            RECT ghostRect = {0, 0, width, height};
            m_renderer.DrawItem(m_dragGhostBitmap, ghostRect, data, state, m_theme);

            // Apply semi-transparent rendering by blending with a transparent overlay
            // This creates the ghost effect
            // LICE doesn't have a direct alpha channel manipulation, so we'll use blit mode
            // The ghost will be drawn with reduced opacity in DrawDragGhost()
        }
    }

    // Trigger repaint to show initial drag state
    if (m_hwndList) {
        InvalidateRect(m_hwndList, NULL, FALSE);
        UpdateWindow(m_hwndList);
    }
}

void ModernRegionPlaylistView::OnDrag()
{
    // Call base class implementation first to maintain functionality
    RegionPlaylistView::OnDrag();

    // Only add visual feedback if modern rendering is enabled
    if (!m_modernRenderingEnabled || !m_isDragging || !m_hwndList) {
        return;
    }

    // Get current cursor position
    POINT currentPos;
    GetCursorPos(&currentPos);
    m_dragCurrentPos = currentPos;

    // Determine drop target index
    int newDropTarget = GetDropTargetIndex();

    // Check if drop target changed
    if (newDropTarget != m_dropTargetIndex) {
        // Store old drop target for invalidation
        int oldDropTarget = m_dropTargetIndex;
        m_dropTargetIndex = newDropTarget;

        // Invalidate the affected items to update highlight
        if (oldDropTarget >= 0) {
            ListView_RedrawItems(m_hwndList, oldDropTarget, oldDropTarget);
        }
        if (m_dropTargetIndex >= 0) {
            ListView_RedrawItems(m_hwndList, m_dropTargetIndex, m_dropTargetIndex);
        }

        // Update the window
        UpdateWindow(m_hwndList);
    }

    // Note: The ghost image would be drawn by the window's paint handler
    // For a complete implementation, we would need to hook into the window's
    // WM_PAINT message or use a layered window overlay
    // For now, we'll rely on the drop target highlighting
}

void ModernRegionPlaylistView::OnEndDrag()
{
    // Clear visual feedback before calling base class
    if (m_modernRenderingEnabled && m_isDragging) {
        ClearDragGhost();

        // Invalidate the drop target area
        if (m_hwndList && m_dropTargetIndex >= 0) {
            ListView_RedrawItems(m_hwndList, m_dropTargetIndex, m_dropTargetIndex);
            UpdateWindow(m_hwndList);
        }
    }

    // Reset drag state
    m_isDragging = false;
    m_draggedItemIndex = -1;
    m_dropTargetIndex = -1;

    // Call base class implementation to complete the drag operation
    // This must be called last as it may trigger updates
    RegionPlaylistView::OnEndDrag();

    // Provide visual feedback within 50ms by triggering immediate repaint
    if (m_hwndList) {
        InvalidateRect(m_hwndList, NULL, FALSE);
        UpdateWindow(m_hwndList);
    }
}

void ModernRegionPlaylistView::DrawDragGhost(LICE_IBitmap* drawbm)
{
    // This method would be called from the paint handler to draw the ghost image
    // at the current cursor position
    if (!drawbm || !m_dragGhostBitmap || !m_isDragging) {
        return;
    }

    // Convert screen coordinates to client coordinates
    POINT clientPos = m_dragCurrentPos;
    ScreenToClient(m_hwndList, &clientPos);

    // Draw the ghost image with semi-transparent rendering (50% opacity)
    LICE_Blit(drawbm, m_dragGhostBitmap,
             clientPos.x, clientPos.y,
             0, 0,
             m_dragGhostBitmap->getWidth(), m_dragGhostBitmap->getHeight(),
             0.5f, // 50% opacity for ghost effect
             LICE_BLIT_MODE_COPY | LICE_BLIT_USE_ALPHA);
}

void ModernRegionPlaylistView::ClearDragGhost()
{
    // Clean up the drag ghost bitmap
    if (m_dragGhostBitmap) {
        delete m_dragGhostBitmap;
        m_dragGhostBitmap = NULL;
    }
}

int ModernRegionPlaylistView::GetDropTargetIndex()
{
    if (!m_hwndList) {
        return -1;
    }

    // Get cursor position in client coordinates
    POINT pt = m_dragCurrentPos;
    ScreenToClient(m_hwndList, &pt);

    // Perform hit test to find the item under the cursor
    LVHITTESTINFO hitTest;
    hitTest.pt = pt;
    hitTest.flags = 0;
    hitTest.iItem = -1;
    hitTest.iSubItem = 0;

    int hitIndex = ListView_HitTest(m_hwndList, &hitTest);

    // Return the hit item index if it's a valid drop target
    if (hitIndex >= 0 && (hitTest.flags & LVHT_ONITEM)) {
        return hitIndex;
    }

    return -1;
}

///////////////////////////////////////////////////////////////////////////////
// ModernMonitoringView implementation
///////////////////////////////////////////////////////////////////////////////

ModernMonitoringView::ModernMonitoringView()
    : SNM_FiveMonitors()
    , m_currentNumber(0)
    , m_nextNumber(0)
    , m_playlistNumber(0)
    , m_theme(NULL)
{
    // Initialize theme
    m_theme = PlaylistTheme::GetInstance();
    if (m_theme) {
        m_theme->UpdateTheme();
    }

    // Initialize strings with default values
    m_currentName.Set("");
    m_nextName.Set("");
    m_playlistName.Set("");

    // Initialize progress info (already initialized by struct constructor)
    m_progress.current = 0.0;
    m_progress.total = 0.0;
    m_progress.percentage = 0.0;
}

ModernMonitoringView::~ModernMonitoringView()
{
}

void ModernMonitoringView::OnPaint(LICE_IBitmap* drawbm, int origin_x, int origin_y, RECT* cliprect, int rscale)
{
    if (!drawbm || !m_theme) {
        // Fallback to base class if no theme
        SNM_FiveMonitors::OnPaint(drawbm, origin_x, origin_y, cliprect, rscale);
        return;
    }

    // Get colors with high contrast (7:1 ratio minimum as per requirements)
    const PlaylistTheme::Colors& colors = m_theme->GetColors();
    const PlaylistTheme::Fonts& fonts = m_theme->GetFonts();

    // Get the drawing area
    RECT r = m_position;
    r.left += origin_x;
    r.top += origin_y;
    r.right += origin_x;
    r.bottom += origin_y;

    // Apply clipping if provided
    if (cliprect) {
        if (r.left < cliprect->left) r.left = cliprect->left;
        if (r.top < cliprect->top) r.top = cliprect->top;
        if (r.right > cliprect->right) r.right = cliprect->right;
        if (r.bottom > cliprect->bottom) r.bottom = cliprect->bottom;
    }

    // Validate rectangle
    if (r.right <= r.left || r.bottom <= r.top) {
        return;
    }

    // Clear background with high contrast background color
    LICE_FillRect(drawbm, r.left, r.top, r.right - r.left, r.bottom - r.top,
                  colors.background, 1.0f, LICE_BLIT_MODE_COPY);

    // Calculate layout areas
    int totalHeight = r.bottom - r.top;
    int totalWidth = r.right - r.left;

    // Layout:
    // - Top 15%: Playlist info
    // - Middle 40%: Current region (large text - 24pt)
    // - Next 30%: Next region (medium text - 20pt)
    // - Bottom 15%: Progress bar

    int playlistHeight = (int)(totalHeight * 0.15);
    int currentHeight = (int)(totalHeight * 0.40);
    int nextHeight = (int)(totalHeight * 0.30);
    int progressHeight = totalHeight - playlistHeight - currentHeight - nextHeight;

    RECT playlistRect = {r.left, r.top, r.right, r.top + playlistHeight};
    RECT currentRect = {r.left, playlistRect.bottom, r.right, playlistRect.bottom + currentHeight};
    RECT nextRect = {r.left, currentRect.bottom, r.right, currentRect.bottom + nextHeight};
    RECT progressRect = {r.left, nextRect.bottom, r.right, r.bottom};

    // Draw playlist info at top
    if (fonts.monitorMedium && m_playlistName.GetLength() > 0) {
        char playlistInfo[256];
        if (m_playlistNumber > 0) {
            snprintf(playlistInfo, sizeof(playlistInfo), "Playlist %d: %s",
                    m_playlistNumber, m_playlistName.Get());
        } else {
            snprintf(playlistInfo, sizeof(playlistInfo), "%s", m_playlistName.Get());
        }

        DrawLargeText(drawbm, playlistRect, playlistInfo, fonts.monitorMedium, colors.text);
    }

    // Draw current region with large text (24pt font)
    if (fonts.monitorLarge && m_currentName.GetLength() > 0) {
        char currentInfo[512];
        if (m_currentNumber > 0) {
            snprintf(currentInfo, sizeof(currentInfo), "NOW: %d. %s",
                    m_currentNumber, m_currentName.Get());
        } else {
            snprintf(currentInfo, sizeof(currentInfo), "NOW: %s", m_currentName.Get());
        }

        // Use high contrast color for current item
        DrawLargeText(drawbm, currentRect, currentInfo, fonts.monitorLarge, colors.currentItemText);

        // Draw a subtle background highlight for current region
        RECT highlightRect = currentRect;
        highlightRect.left += 10;
        highlightRect.right -= 10;
        highlightRect.top += 5;
        highlightRect.bottom -= 5;
        LICE_FillRect(drawbm, highlightRect.left, highlightRect.top,
                     highlightRect.right - highlightRect.left,
                     highlightRect.bottom - highlightRect.top,
                     colors.currentItemBg, 0.3f, LICE_BLIT_MODE_COPY);

        // Redraw text on top of highlight
        DrawLargeText(drawbm, currentRect, currentInfo, fonts.monitorLarge, colors.currentItemText);
    }

    // Draw next region with medium text (20pt font)
    if (fonts.monitorMedium && m_nextName.GetLength() > 0) {
        char nextInfo[512];
        if (m_nextNumber > 0) {
            snprintf(nextInfo, sizeof(nextInfo), "NEXT: %d. %s",
                    m_nextNumber, m_nextName.Get());
        } else {
            snprintf(nextInfo, sizeof(nextInfo), "NEXT: %s", m_nextName.Get());
        }

        DrawLargeText(drawbm, nextRect, nextInfo, fonts.monitorMedium, colors.nextItemText);
    }

    // Draw progress bar
    if (m_progress.total > 0.0) {
        DrawProgressBar(drawbm, progressRect);
    }
}

void ModernMonitoringView::SetProgress(double current, double total)
{
    // Store previous values to detect changes
    double prevCurrent = m_progress.current;
    double prevTotal = m_progress.total;

    // Accept current and total time values
    m_progress.current = current;
    m_progress.total = total;

    // Calculate percentage
    if (total > 0.0) {
        m_progress.percentage = (current / total) * 100.0;

        // Clamp percentage to 0-100 range
        if (m_progress.percentage < 0.0) {
            m_progress.percentage = 0.0;
        } else if (m_progress.percentage > 100.0) {
            m_progress.percentage = 100.0;
        }
    } else {
        m_progress.percentage = 0.0;
    }

    // Trigger repaint if changed
    // We check if the values changed significantly (more than 0.1 seconds)
    // to avoid excessive repaints for tiny changes
    const double CHANGE_THRESHOLD = 0.1;
    bool hasChanged = (fabs(current - prevCurrent) > CHANGE_THRESHOLD) ||
                      (fabs(total - prevTotal) > CHANGE_THRESHOLD);

    if (hasChanged) {
        // Request repaint
        RequestRedraw(NULL);
    }
}

void ModernMonitoringView::SetCurrentRegion(const char* name, int number)
{
    // Store previous values to detect changes
    WDL_FastString prevName;
    prevName.Set(m_currentName.Get());
    int prevNumber = m_currentNumber;

    // Handle null/empty names gracefully
    if (name && name[0] != '\0') {
        m_currentName.Set(name);
    } else {
        // Set empty string for null/empty names
        m_currentName.Set("");
    }

    // Store region number
    m_currentNumber = number;

    // Trigger repaint if changed
    bool hasChanged = (strcmp(prevName.Get(), m_currentName.Get()) != 0) ||
                      (prevNumber != m_currentNumber);

    if (hasChanged) {
        // Request repaint
        RequestRedraw(NULL);
    }
}

void ModernMonitoringView::SetNextRegion(const char* name, int number)
{
    // Store previous values to detect changes
    WDL_FastString prevName;
    prevName.Set(m_nextName.Get());
    int prevNumber = m_nextNumber;

    // Handle null/empty names gracefully
    if (name && name[0] != '\0') {
        m_nextName.Set(name);
    } else {
        // Set empty string for null/empty names
        m_nextName.Set("");
    }

    // Store region number
    m_nextNumber = number;

    // Trigger repaint if changed
    bool hasChanged = (strcmp(prevName.Get(), m_nextName.Get()) != 0) ||
                      (prevNumber != m_nextNumber);

    if (hasChanged) {
        // Request repaint
        RequestRedraw(NULL);
    }
}

void ModernMonitoringView::SetPlaylistInfo(const char* playlistName, int playlistNumber)
{
    // Store previous values to detect changes
    WDL_FastString prevName;
    prevName.Set(m_playlistName.Get());
    int prevNumber = m_playlistNumber;

    // Store playlist name and number
    if (playlistName && playlistName[0] != '\0') {
        m_playlistName.Set(playlistName);
    } else {
        // Set empty string for null/empty names
        m_playlistName.Set("");
    }

    // Store playlist number
    m_playlistNumber = playlistNumber;

    // Trigger repaint if changed
    bool hasChanged = (strcmp(prevName.Get(), m_playlistName.Get()) != 0) ||
                      (prevNumber != m_playlistNumber);

    if (hasChanged) {
        // Request repaint to display at top of monitoring view
        RequestRedraw(NULL);
    }
}

void ModernMonitoringView::DrawProgressBar(LICE_IBitmap* bm, const RECT& r)
{
    if (!bm || !m_theme) {
        return;
    }

    // Validate rectangle
    if (r.right <= r.left || r.bottom <= r.top) {
        return;
    }

    const PlaylistTheme::Colors& colors = m_theme->GetColors();

    // Add padding around the progress bar
    const int PADDING = 20;
    RECT barRect = r;
    barRect.left += PADDING;
    barRect.right -= PADDING;
    barRect.top += PADDING / 2;
    barRect.bottom -= PADDING / 2;

    // Ensure we still have a valid rectangle after padding
    if (barRect.right <= barRect.left || barRect.bottom <= barRect.top) {
        return;
    }

    int barWidth = barRect.right - barRect.left;
    int barHeight = barRect.bottom - barRect.top;

    // Calculate progress percentage from current/total time
    double percentage = 0.0;
    if (m_progress.total > 0.0) {
        percentage = (m_progress.current / m_progress.total);
        // Clamp to 0-1 range
        if (percentage < 0.0) percentage = 0.0;
        if (percentage > 1.0) percentage = 1.0;
    }

    // Calculate filled width
    int filledWidth = (int)(barWidth * percentage);

    // Draw border around progress bar (2px border)
    const int BORDER_WIDTH = 2;
    LICE_DrawRect(bm, barRect.left - BORDER_WIDTH, barRect.top - BORDER_WIDTH,
                  barWidth + BORDER_WIDTH * 2, barHeight + BORDER_WIDTH * 2,
                  colors.border, 1.0f, LICE_BLIT_MODE_COPY);

    // Draw background (unfilled portion)
    LICE_FillRect(bm, barRect.left, barRect.top, barWidth, barHeight,
                  colors.background, 1.0f, LICE_BLIT_MODE_COPY);

    // Draw filled bar with theme progress color
    if (filledWidth > 0) {
        LICE_FillRect(bm, barRect.left, barRect.top, filledWidth, barHeight,
                      colors.progressBar, 1.0f, LICE_BLIT_MODE_COPY);
    }

    // Draw progress text (percentage and time)
    if (m_theme->GetFonts().itemTime) {
        char progressText[128];

        // Format current and total time as MM:SS
        int currentMinutes = (int)(m_progress.current / 60.0);
        int currentSeconds = (int)m_progress.current % 60;
        int totalMinutes = (int)(m_progress.total / 60.0);
        int totalSeconds = (int)m_progress.total % 60;

        snprintf(progressText, sizeof(progressText), "%d:%02d / %d:%02d (%.0f%%)",
                currentMinutes, currentSeconds, totalMinutes, totalSeconds, percentage * 100.0);

        // Measure text to center it
        RECT textRect = {0, 0, 0, 0};
        LICE_MeasureText(progressText, &textRect.right, &textRect.bottom, m_theme->GetFonts().itemTime);

        // Center text in the progress bar area
        int textX = barRect.left + (barWidth - textRect.right) / 2;
        int textY = barRect.top + (barHeight - textRect.bottom) / 2;

        // Draw text with high contrast
        LICE_DrawText(bm, textX, textY, progressText, colors.text,
                     1.0f, LICE_BLIT_MODE_COPY, m_theme->GetFonts().itemTime);
    }
}

void ModernMonitoringView::DrawLargeText(LICE_IBitmap* bm, const RECT& r, const char* text, LICE_CachedFont* font, int color)
{
    if (!bm || !text || !font) {
        return;
    }

    // Validate rectangle
    if (r.right <= r.left || r.bottom <= r.top) {
        return;
    }

    // Measure text to center it
    RECT textRect = {0, 0, 0, 0};
    LICE_MeasureText(text, &textRect.right, &textRect.bottom, font);

    // Calculate centered position
    int textX = r.left + (r.right - r.left - textRect.right) / 2;
    int textY = r.top + (r.bottom - r.top - textRect.bottom) / 2;

    // Ensure text is within bounds
    if (textX < r.left) textX = r.left;
    if (textY < r.top) textY = r.top;

    // Draw the text with high contrast color
    LICE_DrawText(bm, textX, textY, text, color, 1.0f, LICE_BLIT_MODE_COPY, font);
}
