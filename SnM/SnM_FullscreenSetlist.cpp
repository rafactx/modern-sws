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
// SetlistView - Implementation
///////////////////////////////////////////////////////////////////////////////

SetlistView::SetlistView(RECT bounds)
    : m_bounds(bounds)
    , m_selectedIndex(0)
    , m_scrollOffset(0)
    , m_itemHeight(80)  // Default height for each item
{
}

SetlistView::~SetlistView() {
}

void SetlistView::SetItems(const WDL_TypedBuf<SetlistItem>& items) {
    m_items = items;
    CalculateLayout();
}

const SetlistView::SetlistItem* SetlistView::GetItem(int index) const {
    if (index < 0 || index >= m_items.GetSize()) {
        return NULL;
    }
    return &m_items.Get()[index];
}

void SetlistView::SetSelectedIndex(int index) {
    if (index < 0 || index >= m_items.GetSize()) {
        return;
    }
    m_selectedIndex = index;
}

void SetlistView::SetBounds(RECT bounds) {
    m_bounds = bounds;
    CalculateLayout();
}

void SetlistView::CalculateLayout() {
    // Item height is fixed at 80 pixels for large text
    m_itemHeight = 80;
}

void SetlistView::ScrollToIndex(int index) {
    if (index < 0 || index >= m_items.GetSize()) {
        return;
    }

    // Calculate target scroll position to center the item
    int viewHeight = m_bounds.bottom - m_bounds.top;
    int targetScroll = (index * m_itemHeight) - (viewHeight / 2) + (m_itemHeight / 2);

    // Clamp scroll position
    int maxScroll = (m_items.GetSize() * m_itemHeight) - viewHeight;
    if (maxScroll < 0) maxScroll = 0;

    if (targetScroll < 0) targetScroll = 0;
    if (targetScroll > maxScroll) targetScroll = maxScroll;

    m_scrollOffset = targetScroll;
}

void SetlistView::ScrollBy(int delta) {
    m_scrollOffset += delta;

    // Clamp scroll position
    int viewHeight = m_bounds.bottom - m_bounds.top;
    int maxScroll = (m_items.GetSize() * m_itemHeight) - viewHeight;
    if (maxScroll < 0) maxScroll = 0;

    if (m_scrollOffset < 0) m_scrollOffset = 0;
    if (m_scrollOffset > maxScroll) m_scrollOffset = maxScroll;
}

void SetlistView::EnsureVisible(int index) {
    if (index < 0 || index >= m_items.GetSize()) {
        return;
    }

    int viewHeight = m_bounds.bottom - m_bounds.top;
    int itemTop = index * m_itemHeight;
    int itemBottom = itemTop + m_itemHeight;

    // Check if item is above visible area
    if (itemTop < m_scrollOffset) {
        m_scrollOffset = itemTop;
    }
    // Check if item is below visible area
    else if (itemBottom > m_scrollOffset + viewHeight) {
        m_scrollOffset = itemBottom - viewHeight;
    }

    // Clamp scroll position
    int maxScroll = (m_items.GetSize() * m_itemHeight) - viewHeight;
    if (maxScroll < 0) maxScroll = 0;
    if (m_scrollOffset < 0) m_scrollOffset = 0;
    if (m_scrollOffset > maxScroll) m_scrollOffset = maxScroll;
}

int SetlistView::GetItemAtPoint(int x, int y) const {
    // Check if point is within bounds
    if (x < m_bounds.left || x > m_bounds.right ||
        y < m_bounds.top || y > m_bounds.bottom) {
        return -1;
    }

    // Calculate item index from y coordinate
    int relativeY = y - m_bounds.top + m_scrollOffset;
    int index = relativeY / m_itemHeight;

    // Validate index
    if (index < 0 || index >= m_items.GetSize()) {
        return -1;
    }

    return index;
}

void SetlistView::Draw(LICE_IBitmap* bm, FullscreenTheme* theme) {
    if (!bm || !theme) {
        return;
    }

    int viewHeight = m_bounds.bottom - m_bounds.top;
    int viewWidth = m_bounds.right - m_bounds.left;

    // Calculate visible item range
    int firstVisibleIndex = m_scrollOffset / m_itemHeight;
    int lastVisibleIndex = (m_scrollOffset + viewHeight) / m_itemHeight + 1;

    // Clamp to valid range
    if (firstVisibleIndex < 0) firstVisibleIndex = 0;
    if (lastVisibleIndex > m_items.GetSize()) lastVisibleIndex = m_items.GetSize();

    // Draw visible items only
    for (int i = firstVisibleIndex; i < lastVisibleIndex; i++) {
        const SetlistItem* item = GetItem(i);
        if (!item) continue;

        // Calculate item rectangle
        RECT itemRect;
        itemRect.left = m_bounds.left;
        itemRect.top = m_bounds.top + (i * m_itemHeight) - m_scrollOffset;
        itemRect.right = m_bounds.right;
        itemRect.bottom = itemRect.top + m_itemHeight;

        // Skip if completely outside view
        if (itemRect.bottom < m_bounds.top || itemRect.top > m_bounds.bottom) {
            continue;
        }

        // Draw the item
        SetlistItemRenderer::DrawItem(bm, *item, itemRect, theme);
    }

    // Draw scrollbar if needed
    int totalHeight = m_items.GetSize() * m_itemHeight;
    if (totalHeight > viewHeight) {
        DrawScrollbar(bm, theme);
    }
}

void SetlistView::DrawScrollbar(LICE_IBitmap* bm, FullscreenTheme* theme) {
    int viewHeight = m_bounds.bottom - m_bounds.top;
    int totalHeight = m_items.GetSize() * m_itemHeight;

    if (totalHeight <= viewHeight) {
        return;  // No scrollbar needed
    }

    // Scrollbar dimensions
    const int scrollbarWidth = 8;
    int scrollbarX = m_bounds.right - scrollbarWidth - 4;

    // Calculate scrollbar thumb size and position
    float visibleRatio = (float)viewHeight / (float)totalHeight;
    int thumbHeight = (int)(viewHeight * visibleRatio);
    if (thumbHeight < 20) thumbHeight = 20;  // Minimum thumb size

    float scrollRatio = (float)m_scrollOffset / (float)(totalHeight - viewHeight);
    int thumbY = m_bounds.top + (int)((viewHeight - thumbHeight) * scrollRatio);

    // Draw scrollbar track
    RECT trackRect;
    trackRect.left = scrollbarX;
    trackRect.top = m_bounds.top;
    trackRect.right = scrollbarX + scrollbarWidth;
    trackRect.bottom = m_bounds.bottom;
    LICE_FillRect(bm, trackRect.left, trackRect.top, scrollbarWidth, viewHeight,
                  theme->GetColors().progressBarBg, 1.0f, LICE_BLIT_MODE_COPY);

    // Draw scrollbar thumb
    LICE_FillRect(bm, scrollbarX, thumbY, scrollbarWidth, thumbHeight,
                  theme->GetColors().buttonNormal, 1.0f, LICE_BLIT_MODE_COPY);
}

///////////////////////////////////////////////////////////////////////////////
// KeyboardController - Implementation
///////////////////////////////////////////////////////////////////////////////

KeyboardController::KeyboardController()
    : m_numberBuffer(0)
{
}

KeyboardController::~KeyboardController() {
}

KeyboardController::Action KeyboardController::MapKeyToAction(WPARAM key) {
    switch (key) {
        case VK_DOWN:
            return ACTION_SELECT_NEXT;
        case VK_UP:
            return ACTION_SELECT_PREVIOUS;
        case VK_RETURN:
            return ACTION_PLAY_SELECTED;
        case VK_SPACE:
            return ACTION_TOGGLE_PLAY_PAUSE;
        case VK_ESCAPE:
            return ACTION_EXIT_FULLSCREEN;
        case VK_PRIOR:  // Page Up
            return ACTION_PAGE_UP;
        case VK_NEXT:   // Page Down
            return ACTION_PAGE_DOWN;
        case VK_HOME:
            return ACTION_JUMP_FIRST;
        case VK_END:
            return ACTION_JUMP_LAST;
        case VK_RIGHT:
            return ACTION_PLAY_NEXT;
        case VK_LEFT:
            return ACTION_PLAY_PREVIOUS;
        // Number keys 0-9
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return ACTION_JUMP_TO_NUMBER;
        default:
            return ACTION_NONE;
    }
}

KeyboardController::Action KeyboardController::ProcessKey(WPARAM key, LPARAM lParam) {
    Action action = MapKeyToAction(key);

    // Handle number buffer for jump-to-number
    if (action == ACTION_JUMP_TO_NUMBER) {
        int digit = key - '0';
        m_numberBuffer = (m_numberBuffer * 10) + digit;
        return ACTION_JUMP_TO_NUMBER;
    }

    return action;
}

///////////////////////////////////////////////////////////////////////////////
// TransportPanel - Implementation
///////////////////////////////////////////////////////////////////////////////

TransportPanel::TransportPanel(RECT bounds)
    : m_bounds(bounds)
    , m_isPlaying(false)
    , m_totalDuration(0.0)
    , m_hoveredButton(BTN_COUNT)
    , m_pressedButton(BTN_COUNT)
{
    memset(m_buttonRects, 0, sizeof(m_buttonRects));
    CalculateLayout();
}

TransportPanel::~TransportPanel() {
}

void TransportPanel::SetBounds(RECT bounds) {
    m_bounds = bounds;
    CalculateLayout();
}

void TransportPanel::CalculateLayout() {
    int panelWidth = m_bounds.right - m_bounds.left;
    int panelHeight = m_bounds.bottom - m_bounds.top;

    // Button size (80x80 minimum)
    const int buttonSize = 80;
    const int buttonSpacing = 40;

    // Calculate total width needed for buttons
    int totalButtonWidth = (buttonSize * BTN_COUNT) + (buttonSpacing * (BTN_COUNT - 1));

    // Center buttons horizontally
    int startX = m_bounds.left + (panelWidth - totalButtonWidth) / 2;
    int buttonY = m_bounds.top + (panelHeight - buttonSize) / 2;

    // Calculate button rectangles
    for (int i = 0; i < BTN_COUNT; i++) {
        m_buttonRects[i].left = startX + (i * (buttonSize + buttonSpacing));
        m_buttonRects[i].top = buttonY;
        m_buttonRects[i].right = m_buttonRects[i].left + buttonSize;
        m_buttonRects[i].bottom = m_buttonRects[i].top + buttonSize;
    }
}

TransportPanel::Button TransportPanel::GetButtonAtPoint(int x, int y) const {
    for (int i = 0; i < BTN_COUNT; i++) {
        if (x >= m_buttonRects[i].left && x <= m_buttonRects[i].right &&
            y >= m_buttonRects[i].top && y <= m_buttonRects[i].bottom) {
            return (Button)i;
        }
    }
    return BTN_COUNT;
}

void TransportPanel::Draw(LICE_IBitmap* bm, FullscreenTheme* theme) {
    if (!bm || !theme) {
        return;
    }

    // Draw panel background
    LICE_FillRect(bm, m_bounds.left, m_bounds.top,
                  m_bounds.right - m_bounds.left, m_bounds.bottom - m_bounds.top,
                  theme->GetColors().background, 1.0f, LICE_BLIT_MODE_COPY);

    // Draw buttons
    for (int i = 0; i < BTN_COUNT; i++) {
        DrawButton(bm, (Button)i, theme);
    }

    // Draw total time
    DrawTotalTime(bm, theme);
}

void TransportPanel::DrawButton(LICE_IBitmap* bm, Button btn, FullscreenTheme* theme) {
    RECT rect = m_buttonRects[btn];

    // Determine button color based on state
    int bgColor;
    if (m_pressedButton == btn) {
        bgColor = theme->GetColors().buttonPressed;
    }
    else if (m_hoveredButton == btn) {
        bgColor = theme->GetColors().buttonHover;
    }
    else {
        bgColor = theme->GetColors().buttonNormal;
    }

    // Draw button background
    LICE_FillRect(bm, rect.left, rect.top,
                  rect.right - rect.left, rect.bottom - rect.top,
                  bgColor, 1.0f, LICE_BLIT_MODE_COPY);

    // Draw button icon
    int centerX = (rect.left + rect.right) / 2;
    int centerY = (rect.top + rect.bottom) / 2;
    int iconColor = theme->GetColors().buttonText;
    const int iconSize = 30;

    switch (btn) {
        case BTN_PREVIOUS:
            // Draw previous icon (|<)
            LICE_Line(bm, centerX - iconSize, centerY,
                      centerX - 5, centerY - iconSize / 2, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
            LICE_Line(bm, centerX - 5, centerY - iconSize / 2,
                      centerX - 5, centerY + iconSize / 2, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
            LICE_Line(bm, centerX - 5, centerY + iconSize / 2,
                      centerX - iconSize, centerY, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
            // Bar
            LICE_Line(bm, centerX - iconSize - 5, centerY - iconSize / 2,
                      centerX - iconSize - 5, centerY + iconSize / 2, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
            break;

        case BTN_PLAY_STOP:
            if (m_isPlaying) {
                // Draw stop icon (square)
                LICE_FillRect(bm, centerX - iconSize / 2, centerY - iconSize / 2,
                              iconSize, iconSize, iconColor, 1.0f, LICE_BLIT_MODE_COPY);
            }
            else {
                // Draw play icon (triangle)
                LICE_Line(bm, centerX - iconSize / 2, centerY - iconSize / 2,
                          centerX + iconSize / 2, centerY, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
                LICE_Line(bm, centerX + iconSize / 2, centerY,
                          centerX - iconSize / 2, centerY + iconSize / 2, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
                LICE_Line(bm, centerX - iconSize / 2, centerY + iconSize / 2,
                          centerX - iconSize / 2, centerY - iconSize / 2, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
            }
            break;

        case BTN_NEXT:
            // Draw next icon (>|)
            LICE_Line(bm, centerX + 5, centerY - iconSize / 2,
                      centerX + iconSize, centerY, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
            LICE_Line(bm, centerX + iconSize, centerY,
                      centerX + 5, centerY + iconSize / 2, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
            LICE_Line(bm, centerX + 5, centerY + iconSize / 2,
                      centerX + 5, centerY - iconSize / 2, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
            // Bar
            LICE_Line(bm, centerX + iconSize + 5, centerY - iconSize / 2,
                      centerX + iconSize + 5, centerY + iconSize / 2, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
            break;

        default:
            break;
    }
}

void TransportPanel::DrawTotalTime(LICE_IBitmap* bm, FullscreenTheme* theme) {
    // Format total duration
    int hours = (int)(m_totalDuration / 3600.0);
    int minutes = (int)((m_totalDuration - (hours * 3600)) / 60.0);
    int seconds = (int)(m_totalDuration) % 60;

    char timeStr[64];
    if (hours > 0) {
        snprintf(timeStr, sizeof(timeStr), "Total: %02d:%02d:%02d", hours, minutes, seconds);
    }
    else {
        snprintf(timeStr, sizeof(timeStr), "Total: %02d:%02d", minutes, seconds);
    }

    // Position at right side of panel
    int x = m_bounds.right - 200;
    int y = m_bounds.top + 20;

    int textColor = theme->GetColors().textDimmed;

    LICE_DrawText(bm, x, y, timeStr, textColor, 1.0f, LICE_BLIT_MODE_COPY);
}

///////////////////////////////////////////////////////////////////////////////
// NowPlayingPanel - Implementation
///////////////////////////////////////////////////////////////////////////////

NowPlayingPanel::NowPlayingPanel(RECT bounds)
    : m_bounds(bounds)
{
    memset(&m_info, 0, sizeof(NowPlayingInfo));
}

NowPlayingPanel::~NowPlayingPanel() {
}

void NowPlayingPanel::SetInfo(const NowPlayingInfo& info) {
    m_info = info;
}

void NowPlayingPanel::SetBounds(RECT bounds) {
    m_bounds = bounds;
}

void NowPlayingPanel::Draw(LICE_IBitmap* bm, FullscreenTheme* theme) {
    if (!bm || !theme) {
        return;
    }

    // Draw panel background
    LICE_FillRect(bm, m_bounds.left, m_bounds.top,
                  m_bounds.right - m_bounds.left, m_bounds.bottom - m_bounds.top,
                  theme->GetColors().background, 1.0f, LICE_BLIT_MODE_COPY);

    // Draw components
    DrawPlayIcon(bm, theme);
    DrawSongName(bm, theme);
    DrawTimeInfo(bm, theme);
    DrawProgressBar(bm, theme);
}

void NowPlayingPanel::DrawPlayIcon(LICE_IBitmap* bm, FullscreenTheme* theme) {
    if (!m_info.isPlaying) {
        return;
    }

    // Icon position (left side, 48x48)
    const int iconSize = 48;
    const int iconPadding = 20;
    int iconX = m_bounds.left + iconPadding;
    int iconY = m_bounds.top + iconPadding;

    int iconColor = theme->GetColors().currentItemText;

    // Draw play triangle
    int centerX = iconX + iconSize / 2;
    int centerY = iconY + iconSize / 2;
    int triSize = iconSize / 3;

    LICE_Line(bm, centerX - triSize, centerY - triSize,
              centerX + triSize, centerY, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
    LICE_Line(bm, centerX + triSize, centerY,
              centerX - triSize, centerY + triSize, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
    LICE_Line(bm, centerX - triSize, centerY + triSize,
              centerX - triSize, centerY - triSize, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
}

void NowPlayingPanel::DrawSongName(LICE_IBitmap* bm, FullscreenTheme* theme) {
    // Position after icon
    const int iconWidth = 88;  // Icon + padding
    int x = m_bounds.left + iconWidth;
    int y = m_bounds.top + 20;

    int textColor = theme->GetColors().text;

    // Draw song name with large font (40pt)
    LICE_DrawText(bm, x, y, m_info.songName, textColor, 1.0f, LICE_BLIT_MODE_COPY);
}

void NowPlayingPanel::DrawTimeInfo(LICE_IBitmap* bm, FullscreenTheme* theme) {
    // Format times as MM:SS
    int currentMin = (int)(m_info.currentTime / 60.0);
    int currentSec = (int)(m_info.currentTime) % 60;
    int totalMin = (int)(m_info.totalTime / 60.0);
    int totalSec = (int)(m_info.totalTime) % 60;

    char timeStr[64];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d / %02d:%02d",
             currentMin, currentSec, totalMin, totalSec);

    // Position below song name
    const int iconWidth = 88;
    int x = m_bounds.left + iconWidth;
    int y = m_bounds.top + 70;  // Below song name

    int textColor = theme->GetColors().textDimmed;

    LICE_DrawText(bm, x, y, timeStr, textColor, 1.0f, LICE_BLIT_MODE_COPY);
}

void NowPlayingPanel::DrawProgressBar(LICE_IBitmap* bm, FullscreenTheme* theme) {
    // Progress bar dimensions
    const int barHeight = 8;
    const int padding = 20;
    int barWidth = (m_bounds.right - m_bounds.left) - (padding * 2);
    int barX = m_bounds.left + padding;
    int barY = m_bounds.bottom - barHeight - padding;

    // Draw background
    LICE_FillRect(bm, barX, barY, barWidth, barHeight,
                  theme->GetColors().progressBarBg, 1.0f, LICE_BLIT_MODE_COPY);

    // Draw filled portion
    int fillWidth = (int)(barWidth * m_info.progress);
    if (fillWidth > 0) {
        LICE_FillRect(bm, barX, barY, fillWidth, barHeight,
                      theme->GetColors().progressBarFill, 1.0f, LICE_BLIT_MODE_COPY);
    }

    // Draw border
    int borderColor = theme->GetColors().textDimmed;
    LICE_Line(bm, barX, barY, barX + barWidth, barY, borderColor, 1.0f, LICE_BLIT_MODE_COPY, false);
    LICE_Line(bm, barX + barWidth, barY, barX + barWidth, barY + barHeight, borderColor, 1.0f, LICE_BLIT_MODE_COPY, false);
    LICE_Line(bm, barX + barWidth, barY + barHeight, barX, barY + barHeight, borderColor, 1.0f, LICE_BLIT_MODE_COPY, false);
    LICE_Line(bm, barX, barY + barHeight, barX, barY, borderColor, 1.0f, LICE_BLIT_MODE_COPY, false);
}

///////////////////////////////////////////////////////////////////////////////
// SetlistItemRenderer - Implementation
///////////////////////////////////////////////////////////////////////////////

void SetlistItemRenderer::DrawItem(
    LICE_IBitmap* bm,
    const SetlistView::SetlistItem& item,
    RECT itemRect,
    FullscreenTheme* theme)
{
    if (!bm || !theme) {
        return;
    }

    // Draw components in order
    DrawBackground(bm, itemRect, item, theme);
    DrawStatusIcon(bm, itemRect, item, theme);
    DrawNumber(bm, itemRect, item, theme);
    DrawSongName(bm, itemRect, item, theme);
    DrawDuration(bm, itemRect, item, theme);
}

void SetlistItemRenderer::DrawBackground(
    LICE_IBitmap* bm,
    RECT rect,
    const SetlistView::SetlistItem& item,
    FullscreenTheme* theme)
{
    int bgColor;

    if (item.isPlaying) {
        // High contrast background for current item
        bgColor = theme->GetColors().currentItemBg;
    }
    else if (item.isNext) {
        // Subtle background for next item
        bgColor = theme->GetColors().nextItemBg;
    }
    else {
        // No background for other items
        return;
    }

    // Fill background
    LICE_FillRect(bm, rect.left, rect.top,
                  rect.right - rect.left, rect.bottom - rect.top,
                  bgColor, 1.0f, LICE_BLIT_MODE_COPY);

    // Draw border for selected item
    if (item.isSelected) {
        int borderColor = theme->GetColors().selectedBorder;
        // Top border
        LICE_FillRect(bm, rect.left, rect.top, rect.right - rect.left, 2,
                      borderColor, 1.0f, LICE_BLIT_MODE_COPY);
        // Bottom border
        LICE_FillRect(bm, rect.left, rect.bottom - 2, rect.right - rect.left, 2,
                      borderColor, 1.0f, LICE_BLIT_MODE_COPY);
    }
}

void SetlistItemRenderer::DrawStatusIcon(
    LICE_IBitmap* bm,
    RECT rect,
    const SetlistView::SetlistItem& item,
    FullscreenTheme* theme)
{
    if (!item.isPlaying && !item.isNext) {
        return;  // No icon for regular items
    }

    // Icon position (left side, 48x48)
    const int iconSize = 48;
    const int iconPadding = 16;
    int iconX = rect.left + iconPadding;
    int iconY = rect.top + ((rect.bottom - rect.top) - iconSize) / 2;

    int iconColor = item.isPlaying ? theme->GetColors().currentItemText : theme->GetColors().nextItemText;

    if (item.isPlaying) {
        // Draw play triangle
        int centerX = iconX + iconSize / 2;
        int centerY = iconY + iconSize / 2;
        int triSize = iconSize / 3;

        // Simple triangle using lines
        LICE_Line(bm, centerX - triSize, centerY - triSize,
                  centerX + triSize, centerY, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
        LICE_Line(bm, centerX + triSize, centerY,
                  centerX - triSize, centerY + triSize, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
        LICE_Line(bm, centerX - triSize, centerY + triSize,
                  centerX - triSize, centerY - triSize, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
    }
    else if (item.isNext) {
        // Draw "next" indicator (double arrow or ">>")
        int centerX = iconX + iconSize / 2;
        int centerY = iconY + iconSize / 2;
        int arrowSize = iconSize / 4;

        // First arrow
        LICE_Line(bm, centerX - arrowSize - 5, centerY - arrowSize,
                  centerX - 5, centerY, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
        LICE_Line(bm, centerX - 5, centerY,
                  centerX - arrowSize - 5, centerY + arrowSize, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);

        // Second arrow
        LICE_Line(bm, centerX + 5, centerY - arrowSize,
                  centerX + arrowSize + 5, centerY, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
        LICE_Line(bm, centerX + arrowSize + 5, centerY,
                  centerX + 5, centerY + arrowSize, iconColor, 1.0f, LICE_BLIT_MODE_COPY, false);
    }
}

void SetlistItemRenderer::DrawNumber(
    LICE_IBitmap* bm,
    RECT rect,
    const SetlistView::SetlistItem& item,
    FullscreenTheme* theme)
{
    char numStr[16];
    snprintf(numStr, sizeof(numStr), "%d.", item.number);

    // Position after icon
    const int iconWidth = 80;  // Icon + padding
    int x = rect.left + iconWidth;
    int y = rect.top + ((rect.bottom - rect.top) - 28) / 2;  // Center vertically

    int textColor = item.isPlaying ? theme->GetColors().currentItemText : theme->GetColors().text;

    LICE_DrawText(bm, x, y, numStr, textColor, 1.0f, LICE_BLIT_MODE_COPY);
}

void SetlistItemRenderer::DrawSongName(
    LICE_IBitmap* bm,
    RECT rect,
    const SetlistView::SetlistItem& item,
    FullscreenTheme* theme)
{
    // Position after number
    const int numberWidth = 60;
    const int iconWidth = 80;
    int x = rect.left + iconWidth + numberWidth;
    int y = rect.top + ((rect.bottom - rect.top) - 32) / 2;  // Center vertically

    // Calculate available width (leave space for duration)
    const int durationWidth = 100;
    int availableWidth = (rect.right - x) - durationWidth - 20;

    int textColor = item.isPlaying ? theme->GetColors().currentItemText : theme->GetColors().text;

    // Draw song name (LICE will handle truncation)
    LICE_DrawText(bm, x, y, item.name, textColor, 1.0f, LICE_BLIT_MODE_COPY);
}

void SetlistItemRenderer::DrawDuration(
    LICE_IBitmap* bm,
    RECT rect,
    const SetlistView::SetlistItem& item,
    FullscreenTheme* theme)
{
    // Format duration as MM:SS
    int minutes = (int)(item.duration / 60.0);
    int seconds = (int)(item.duration) % 60;
    char durationStr[16];
    snprintf(durationStr, sizeof(durationStr), "[%02d:%02d]", minutes, seconds);

    // Position at right side
    const int padding = 20;
    int x = rect.right - 100 - padding;
    int y = rect.top + ((rect.bottom - rect.top) - 20) / 2;  // Center vertically

    int textColor = item.isPlaying ? theme->GetColors().currentItemText : theme->GetColors().textDimmed;

    LICE_DrawText(bm, x, y, durationStr, textColor, 1.0f, LICE_BLIT_MODE_COPY);
}

///////////////////////////////////////////////////////////////////////////////
// FullscreenSetlistWindow - Main window class implementation
///////////////////////////////////////////////////////////////////////////////

// Window class name
#define FULLSCREEN_SETLIST_WND_CLASS "SWS_FullscreenSetlistWindow"

FullscreenSetlistWindow::FullscreenSetlistWindow()
    : m_hwnd(NULL)
    , m_nowPlayingPanel(NULL)
    , m_setlistView(NULL)
    , m_transportPanel(NULL)
    , m_keyboardController(NULL)
    , m_theme(NULL)
    , m_currentPlaylistIndex(-1)
    , m_selectedItemIndex(0)
    , m_isFullscreen(false)
    , m_savedWindowStyle(0)
{
    // Initialize saved window rect
    memset(&m_savedWindowRect, 0, sizeof(RECT));

    // Get theme instance
    m_theme = FullscreenTheme::GetInstance();

    // Create the main window
    if (!CreateMainWindow()) {
        // Window creation failed
        return;
    }

    // Initialize panels with temporary bounds (will be updated in UpdateLayout)
    RECT tempBounds = {0, 0, 1280, 720};
    m_nowPlayingPanel = new NowPlayingPanel(tempBounds);
    m_setlistView = new SetlistView(tempBounds);
    m_transportPanel = new TransportPanel(tempBounds);
    m_keyboardController = new KeyboardController();

    // Update layout to set proper bounds
    UpdateLayout();
}

FullscreenSetlistWindow::~FullscreenSetlistWindow() {
    // Clean up panels
    if (m_nowPlayingPanel) {
        delete m_nowPlayingPanel;
        m_nowPlayingPanel = NULL;
    }
    if (m_setlistView) {
        delete m_setlistView;
        m_setlistView = NULL;
    }
    if (m_transportPanel) {
        delete m_transportPanel;
        m_transportPanel = NULL;
    }
    if (m_keyboardController) {
        delete m_keyboardController;
        m_keyboardController = NULL;
    }

    // Destroy window
    DestroyMainWindow();
}

bool FullscreenSetlistWindow::CreateMainWindow() {
    // Register window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = g_hInst;
    wc.lpszClassName = FULLSCREEN_SETLIST_WND_CLASS;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    // Register (ignore error if already registered)
    RegisterClass(&wc);

    // Create window with WS_POPUP style for fullscreen capability
    // Start with a reasonable size, will be maximized when shown
    int width = 1280;
    int height = 720;
    int x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

    m_hwnd = CreateWindowEx(
        0,                              // Extended style
        FULLSCREEN_SETLIST_WND_CLASS,   // Class name
        "Fullscreen Setlist",           // Window title
        WS_POPUP | WS_VISIBLE,          // Style: popup for fullscreen
        x, y, width, height,            // Position and size
        NULL,                           // Parent window
        NULL,                           // Menu
        g_hInst,                        // Instance
        this                            // User data (this pointer)
    );

    if (!m_hwnd) {
        return false;
    }

    // Store this pointer in window user data for WndProc
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);

    // Initially hide the window
    ShowWindow(m_hwnd, SW_HIDE);

    return true;
}

void FullscreenSetlistWindow::DestroyMainWindow() {
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Window procedure
///////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK FullscreenSetlistWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Get the this pointer from window user data
    FullscreenSetlistWindow* pThis = (FullscreenSetlistWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
        case WM_PAINT:
            if (pThis) {
                pThis->OnPaint();
            }
            return 0;

        case WM_KEYDOWN:
            if (pThis) {
                pThis->OnKeyDown(wParam);
            }
            return 0;

        case WM_MOUSEMOVE:
            if (pThis) {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                pThis->OnMouseMove(x, y);
            }
            return 0;

        case WM_LBUTTONDOWN:
            if (pThis) {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                pThis->OnMouseClick(x, y);
            }
            return 0;

        case WM_SIZE:
            if (pThis) {
                int width = LOWORD(lParam);
                int height = HIWORD(lParam);
                pThis->OnResize(width, height);
            }
            return 0;

        case WM_CLOSE:
            // Hide instead of destroying
            if (pThis) {
                pThis->Hide();
            }
            return 0;

        case WM_ERASEBKGND:
            // Prevent flicker by handling erase ourselves
            return 1;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Event handlers
///////////////////////////////////////////////////////////////////////////////

void FullscreenSetlistWindow::OnPaint() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hwnd, &ps);

    if (hdc) {
        // Get client rect
        RECT clientRect;
        GetClientRect(m_hwnd, &clientRect);

        // Create a bitmap for double buffering
        LICE_SysBitmap bm(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

        // Clear background with theme color
        LICE_Clear(&bm, m_theme->GetColors().background);

        // Draw panels
        if (m_nowPlayingPanel) m_nowPlayingPanel->Draw(&bm, m_theme);
        if (m_setlistView) m_setlistView->Draw(&bm, m_theme);
        if (m_transportPanel) m_transportPanel->Draw(&bm, m_theme);

        // Blit to screen
        LICE_Blit(&bm, hdc, 0, 0, NULL, 1.0f, LICE_BLIT_MODE_COPY);
    }

    EndPaint(m_hwnd, &ps);
}

void FullscreenSetlistWindow::OnKeyDown(WPARAM key) {
    // TODO: Process keyboard input with KeyboardController in future tasks
    // For now, handle basic keys directly

    switch (key) {
        case VK_ESCAPE:
            // Exit fullscreen or hide window
            if (m_isFullscreen) {
                ToggleFullscreen();
            } else {
                Hide();
            }
            break;

        case VK_DOWN:
            SelectNext();
            break;

        case VK_UP:
            SelectPrevious();
            break;

        case VK_RETURN:
            PlaySelected();
            break;

        case VK_SPACE:
            TogglePlayPause();
            break;

        case VK_HOME:
            JumpToFirst();
            break;

        case VK_END:
            JumpToLast();
            break;

        case VK_PRIOR:  // Page Up
            // TODO: Implement page up scrolling
            break;

        case VK_NEXT:   // Page Down
            // TODO: Implement page down scrolling
            break;
    }

    // Trigger repaint for visual feedback
    InvalidateRect(m_hwnd, NULL, FALSE);
}

void FullscreenSetlistWindow::OnMouseMove(int x, int y) {
    // TODO: Implement hover effects for transport buttons in future tasks
    // For now, just track mouse position

    // Check if over transport panel buttons
    // if (m_transportPanel) {
    //     TransportPanel::Button btn = m_transportPanel->GetButtonAtPoint(x, y);
    //     m_transportPanel->SetHoveredButton(btn);
    //     InvalidateRect(m_hwnd, NULL, FALSE);
    // }
}

void FullscreenSetlistWindow::OnMouseClick(int x, int y) {
    // TODO: Implement click handling for transport buttons and setlist items
    // For now, just provide basic functionality

    // Check if clicked on transport button
    // if (m_transportPanel) {
    //     TransportPanel::Button btn = m_transportPanel->GetButtonAtPoint(x, y);
    //     // Handle button click
    // }

    // Check if clicked on setlist item
    // if (m_setlistView) {
    //     int itemIndex = m_setlistView->GetItemAtPoint(x, y);
    //     if (itemIndex >= 0) {
    //         SelectItem(itemIndex);
    //     }
    // }
}

void FullscreenSetlistWindow::OnResize(int width, int height) {
    // Update layout when window is resized
    UpdateLayout();

    // Trigger repaint
    InvalidateRect(m_hwnd, NULL, FALSE);
}

///////////////////////////////////////////////////////////////////////////////
// Window management methods
///////////////////////////////////////////////////////////////////////////////

void FullscreenSetlistWindow::Show() {
    if (!m_hwnd) {
        return;
    }

    // Show the window
    ShowWindow(m_hwnd, SW_SHOW);

    // Set focus to the window for keyboard input
    SetFocus(m_hwnd);

    // Bring to foreground
    SetForegroundWindow(m_hwnd);

    // Update layout
    UpdateLayout();

    // Refresh data from SWS
    RefreshFromSWS();

    // Trigger initial paint
    InvalidateRect(m_hwnd, NULL, FALSE);
}

void FullscreenSetlistWindow::Hide() {
    if (!m_hwnd) {
        return;
    }

    // If in fullscreen mode, exit it first
    if (m_isFullscreen) {
        ToggleFullscreen();
    }

    // Hide the window
    ShowWindow(m_hwnd, SW_HIDE);
}

void FullscreenSetlistWindow::ToggleFullscreen() {
    if (!m_hwnd) {
        return;
    }

    if (m_isFullscreen) {
        // Exit fullscreen mode - restore window

        // Restore window style
        SetWindowLong(m_hwnd, GWL_STYLE, m_savedWindowStyle);

        // Restore window position and size
        SetWindowPos(
            m_hwnd,
            HWND_NOTOPMOST,
            m_savedWindowRect.left,
            m_savedWindowRect.top,
            m_savedWindowRect.right - m_savedWindowRect.left,
            m_savedWindowRect.bottom - m_savedWindowRect.top,
            SWP_FRAMECHANGED | SWP_SHOWWINDOW
        );

        m_isFullscreen = false;
    }
    else {
        // Enter fullscreen mode

        // Save current window position and style
        GetWindowRect(m_hwnd, &m_savedWindowRect);
        m_savedWindowStyle = GetWindowLong(m_hwnd, GWL_STYLE);

        // Remove window borders and caption
        LONG style = m_savedWindowStyle;
        style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
        SetWindowLong(m_hwnd, GWL_STYLE, style);

        // Get monitor info for the monitor containing the window
        HMONITOR hMonitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = { sizeof(MONITORINFO) };
        GetMonitorInfo(hMonitor, &mi);

        // Set window to cover entire monitor
        SetWindowPos(
            m_hwnd,
            HWND_TOPMOST,
            mi.rcMonitor.left,
            mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top,
            SWP_FRAMECHANGED | SWP_SHOWWINDOW
        );

        m_isFullscreen = true;
    }

    // Update layout for new size
    UpdateLayout();

    // Trigger repaint
    InvalidateRect(m_hwnd, NULL, FALSE);
}

///////////////////////////////////////////////////////////////////////////////
// Layout management
///////////////////////////////////////////////////////////////////////////////

void FullscreenSetlistWindow::UpdateLayout() {
    if (!m_hwnd) {
        return;
    }

    // Get client rect
    RECT clientRect;
    GetClientRect(m_hwnd, &clientRect);

    int totalWidth = clientRect.right - clientRect.left;
    int totalHeight = clientRect.bottom - clientRect.top;

    // Define panel heights (as specified in design)
    const int NOW_PLAYING_HEIGHT = 150;
    const int TRANSPORT_HEIGHT = 150;

    // Calculate bounds for NowPlayingPanel (150px height at top)
    RECT nowPlayingBounds;
    nowPlayingBounds.left = 0;
    nowPlayingBounds.top = 0;
    nowPlayingBounds.right = totalWidth;
    nowPlayingBounds.bottom = NOW_PLAYING_HEIGHT;

    // Calculate bounds for TransportPanel (150px height at bottom)
    RECT transportBounds;
    transportBounds.left = 0;
    transportBounds.top = totalHeight - TRANSPORT_HEIGHT;
    transportBounds.right = totalWidth;
    transportBounds.bottom = totalHeight;

    // Calculate bounds for SetlistView (middle, scrollable)
    RECT setlistBounds;
    setlistBounds.left = 0;
    setlistBounds.top = NOW_PLAYING_HEIGHT;
    setlistBounds.right = totalWidth;
    setlistBounds.bottom = totalHeight - TRANSPORT_HEIGHT;

    // Update panel bounds
    if (m_nowPlayingPanel) {
        m_nowPlayingPanel->SetBounds(nowPlayingBounds);
    }
    if (m_setlistView) {
        m_setlistView->SetBounds(setlistBounds);
    }
    if (m_transportPanel) {
        m_transportPanel->SetBounds(transportBounds);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Stub implementations for methods to be implemented in future tasks
///////////////////////////////////////////////////////////////////////////////

void FullscreenSetlistWindow::LoadSetlist(int playlistIndex) {
    m_currentPlaylistIndex = playlistIndex;
    RefreshFromSWS();
}

void FullscreenSetlistWindow::RefreshFromSWS() {
    // Get playlist from SWS
    RegionPlaylist* playlist = GetPlaylist(m_currentPlaylistIndex);
    if (!playlist) {
        return;
    }

    // Convert playlist items to SetlistView items
    WDL_TypedBuf<SetlistView::SetlistItem> items;
    items.Resize(playlist->GetSize());

    for (int i = 0; i < playlist->GetSize(); i++) {
        RgnPlaylistItem* plItem = playlist->Get(i);
        if (!plItem || !plItem->IsValidIem()) {
            continue;
        }

        SetlistView::SetlistItem& item = items.Get()[i];
        item.number = i + 1;
        item.regionIndex = plItem->m_rgnId;
        item.isPlaying = false;  // TODO: Determine from playback state
        item.isNext = (i == m_selectedItemIndex + 1);
        item.isSelected = (i == m_selectedItemIndex);

        // Get region info
        char name[256] = "";
        double pos = 0.0, end = 0.0;
        int rgnIdx = EnumMarkerRegionById(NULL, plItem->m_rgnId, NULL, &pos, &end, name, NULL, NULL);
        if (rgnIdx >= 0) {
            snprintf(item.name, sizeof(item.name), "%s", name);
            item.duration = end - pos;
        }
        else {
            snprintf(item.name, sizeof(item.name), "Unknown Region");
            item.duration = 0.0;
        }
    }

    // Update setlist view
    if (m_setlistView) {
        m_setlistView->SetItems(items);
        m_setlistView->SetSelectedIndex(m_selectedItemIndex);
    }

    // Update transport panel with total duration
    if (m_transportPanel && playlist) {
        double totalDuration = playlist->GetLength();
        m_transportPanel->SetTotalDuration(totalDuration);
    }

    // TODO: Update now playing panel with current song info
}

void FullscreenSetlistWindow::PlaySelected() {
    if (m_selectedItemIndex < 0) {
        return;
    }

    // Play the selected item using SWS playlist functions
    PlaylistPlay(m_currentPlaylistIndex, m_selectedItemIndex);

    // Refresh UI
    RefreshFromSWS();
}

void FullscreenSetlistWindow::PlayNext() {
    // Get next valid item
    int nextItem = GetNextValidItem(m_currentPlaylistIndex, m_selectedItemIndex, false, true, false);
    if (nextItem >= 0) {
        m_selectedItemIndex = nextItem;
        PlaySelected();
    }
}

void FullscreenSetlistWindow::PlayPrevious() {
    // Get previous valid item
    int prevItem = GetPrevValidItem(m_currentPlaylistIndex, m_selectedItemIndex, false, true, false);
    if (prevItem >= 0) {
        m_selectedItemIndex = prevItem;
        PlaySelected();
    }
}

void FullscreenSetlistWindow::Stop() {
    PlaylistStop();

    // Refresh UI
    RefreshFromSWS();
}

void FullscreenSetlistWindow::TogglePlayPause() {
    // Check if currently playing
    int playState = GetPlayState();

    if (playState & 1) {  // Playing
        Stop();
    }
    else {
        PlaySelected();
    }
}

void FullscreenSetlistWindow::SelectNext() {
    if (!m_setlistView) {
        return;
    }

    int itemCount = m_setlistView->GetItemCount();
    if (itemCount == 0) {
        return;
    }

    m_selectedItemIndex++;
    if (m_selectedItemIndex >= itemCount) {
        m_selectedItemIndex = itemCount - 1;
    }

    m_setlistView->SetSelectedIndex(m_selectedItemIndex);
    m_setlistView->EnsureVisible(m_selectedItemIndex);

    // Update item states
    RefreshFromSWS();
}

void FullscreenSetlistWindow::SelectPrevious() {
    if (!m_setlistView) {
        return;
    }

    m_selectedItemIndex--;
    if (m_selectedItemIndex < 0) {
        m_selectedItemIndex = 0;
    }

    m_setlistView->SetSelectedIndex(m_selectedItemIndex);
    m_setlistView->EnsureVisible(m_selectedItemIndex);

    // Update item states
    RefreshFromSWS();
}

void FullscreenSetlistWindow::SelectItem(int index) {
    if (!m_setlistView) {
        return;
    }

    int itemCount = m_setlistView->GetItemCount();
    if (index < 0 || index >= itemCount) {
        return;
    }

    m_selectedItemIndex = index;
    m_setlistView->SetSelectedIndex(m_selectedItemIndex);
    m_setlistView->EnsureVisible(m_selectedItemIndex);

    // Update item states
    RefreshFromSWS();
}

void FullscreenSetlistWindow::ScrollToItem(int index) {
    if (!m_setlistView) {
        return;
    }

    m_setlistView->ScrollToIndex(index);
}

void FullscreenSetlistWindow::JumpToFirst() {
    SelectItem(0);
}

void FullscreenSetlistWindow::JumpToLast() {
    if (!m_setlistView) {
        return;
    }

    int itemCount = m_setlistView->GetItemCount();
    if (itemCount > 0) {
        SelectItem(itemCount - 1);
    }
}

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
