/******************************************************************************
/ SnM_PlaylistIcons.cpp
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
#include "SnM_PlaylistIcons.h"

// Static instance
PlaylistIconManager* PlaylistIconManager::s_instance = NULL;

///////////////////////////////////////////////////////////////////////////////
// PlaylistIconManager implementation
///////////////////////////////////////////////////////////////////////////////

PlaylistIconManager::PlaylistIconManager()
{
}

PlaylistIconManager::~PlaylistIconManager()
{
    ClearCache();
}

PlaylistIconManager* PlaylistIconManager::GetInstance()
{
    if (!s_instance) {
        s_instance = new PlaylistIconManager();
    }
    return s_instance;
}

LICE_IBitmap* PlaylistIconManager::GetIcon(IconType type, int size)
{
    if (type < 0 || type >= ICON_TYPE_COUNT) {
        return NULL;
    }

    // Check cache
    int key = GetCacheKey(type, size);
    for (int i = 0; i < m_iconCache.GetSize(); i++) {
        // TODO: Implement proper cache lookup with key storage
        // For now, generate on demand
    }

    // Generate icon if not in cache
    GenerateIcon(type, size);

    // Return last generated icon (temporary implementation)
    return m_iconCache.GetSize() > 0 ? m_iconCache.Get(m_iconCache.GetSize() - 1) : NULL;
}

void PlaylistIconManager::DrawIcon(LICE_IBitmap* dest, IconType type, int x, int y, int size, int color)
{
    if (!dest) {
        return;
    }

    LICE_IBitmap* icon = GetIcon(type, size);
    if (icon) {
        LICE_Blit(dest, icon, x, y, 0, 0, size, size, 1.0f, LICE_BLIT_MODE_COPY | LICE_BLIT_USE_ALPHA);
    }
}

void PlaylistIconManager::ClearCache()
{
    for (int i = 0; i < m_iconCache.GetSize(); i++) {
        LICE_IBitmap* bm = m_iconCache.Get(i);
        if (bm) {
            delete bm;
        }
    }
    m_iconCache.Empty();
}

void PlaylistIconManager::GenerateIcon(IconType type, int size)
{
    LICE_IBitmap* bm = new LICE_MemBitmap(size, size);
    if (!bm) {
        return;
    }

    // Clear bitmap
    LICE_Clear(bm, 0);

    // Default color (white)
    int color = LICE_RGBA(255, 255, 255, 255);

    // Generate specific icon
    switch (type) {
        case ICON_PLAY:
            GeneratePlayIcon(bm, size, color);
            break;
        case ICON_NEXT:
            GenerateNextIcon(bm, size, color);
            break;
        case ICON_WARNING:
            GenerateWarningIcon(bm, size, color);
            break;
        case ICON_LOOP_INFINITE:
            GenerateLoopInfiniteIcon(bm, size, color);
            break;
        case ICON_SYNC_LOSS:
            GenerateSyncLossIcon(bm, size, color);
            break;
        default:
            break;
    }

    m_iconCache.Add(bm);
}

void PlaylistIconManager::GeneratePlayIcon(LICE_IBitmap* bm, int size, int color)
{
    if (!bm) return;

    // Draw a simple triangle play icon
    int padding = size / 4;
    int x1 = padding;
    int y1 = padding;
    int x2 = padding;
    int y2 = size - padding;
    int x3 = size - padding;
    int y3 = size / 2;

    // Draw filled triangle
    LICE_FillTriangle(bm, x1, y1, x2, y2, x3, y3, color, 1.0f, LICE_BLIT_MODE_COPY);
}

void PlaylistIconManager::GenerateNextIcon(LICE_IBitmap* bm, int size, int color)
{
    if (!bm) return;

    // Draw a forward arrow or double triangle
    int padding = size / 4;
    int mid = size / 2;

    // First triangle
    LICE_FillTriangle(bm,
        padding, padding,
        padding, size - padding,
        mid, size / 2,
        color, 1.0f, LICE_BLIT_MODE_COPY);

    // Second triangle
    LICE_FillTriangle(bm,
        mid, padding,
        mid, size - padding,
        size - padding, size / 2,
        color, 1.0f, LICE_BLIT_MODE_COPY);
}

void PlaylistIconManager::GenerateWarningIcon(LICE_IBitmap* bm, int size, int color)
{
    if (!bm) return;

    // Draw exclamation mark
    int centerX = size / 2;
    int padding = size / 6;

    // Exclamation line
    LICE_FillRect(bm, centerX - 1, padding, 2, size / 2, color, 1.0f, LICE_BLIT_MODE_COPY);

    // Exclamation dot
    LICE_FillRect(bm, centerX - 1, size - padding - 2, 2, 2, color, 1.0f, LICE_BLIT_MODE_COPY);
}

void PlaylistIconManager::GenerateLoopInfiniteIcon(LICE_IBitmap* bm, int size, int color)
{
    if (!bm) return;

    // Draw infinity symbol (simplified as two circles)
    int radius = size / 4;
    int centerY = size / 2;
    int leftX = size / 3;
    int rightX = 2 * size / 3;

    // Left circle
    LICE_Circle(bm, leftX, centerY, radius, color, 1.0f, LICE_BLIT_MODE_COPY, true);

    // Right circle
    LICE_Circle(bm, rightX, centerY, radius, color, 1.0f, LICE_BLIT_MODE_COPY, true);
}

void PlaylistIconManager::GenerateSyncLossIcon(LICE_IBitmap* bm, int size, int color)
{
    if (!bm) return;

    // Draw red X
    int padding = size / 4;
    int redColor = LICE_RGBA(255, 0, 0, 255);

    // Diagonal line 1
    LICE_Line(bm, padding, padding, size - padding, size - padding, redColor, 1.0f, LICE_BLIT_MODE_COPY, false);

    // Diagonal line 2
    LICE_Line(bm, size - padding, padding, padding, size - padding, redColor, 1.0f, LICE_BLIT_MODE_COPY, false);
}

int PlaylistIconManager::GetCacheKey(IconType type, int size) const
{
    return (type << 16) | size;
}
