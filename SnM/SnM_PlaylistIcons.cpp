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

void PlaylistIconManager::DestroyInstance()
{
    if (s_instance) {
        delete s_instance;
        s_instance = NULL;
    }
}

LICE_IBitmap* PlaylistIconManager::GetIcon(IconType type, int size)
{
    // TASK 11.2: Validate icon type
    if (type < 0 || type >= ICON_TYPE_COUNT) {
        #ifdef _DEBUG
        OutputDebugString("PlaylistIconManager::GetIcon - Invalid icon type\n");
        #endif
        return NULL;
    }

    // TASK 11.2: Validate and clamp size
    if (size < 8 || size > 128) {
        #ifdef _DEBUG
        OutputDebugString("PlaylistIconManager::GetIcon - Invalid size, using default\n");
        #endif
        size = 16; // Default to 16x16
    }

    // Check cache
    int key = GetCacheKey(type, size);

    // TASK 11.1: Validate cache entries before accessing
    for (int i = 0; i < m_iconCache.GetSize(); i++) {
        CacheEntry* entry = m_iconCache.Get(i);
        if (entry && entry->key == key && entry->bitmap) {
            return entry->bitmap;
        }
    }

    // TASK 11.3: Handle icon generation failure gracefully
    // Generate icon if not in cache
    LICE_IBitmap* icon = GenerateIcon(type, size);
    if (icon) {
        // TASK 11.3: Handle cache entry creation failure
        #ifdef _DEBUG
        try {
        #endif
            CacheEntry* entry = new CacheEntry();
            if (entry) {
                entry->key = key;
                entry->bitmap = icon;
                m_iconCache.Add(entry);
            } else {
                #ifdef _DEBUG
                OutputDebugString("PlaylistIconManager::GetIcon - Failed to create cache entry\n");
                #endif
                // Still return the icon even if caching failed
            }
        #ifdef _DEBUG
        }
        catch (...) {
            OutputDebugString("PlaylistIconManager::GetIcon - Exception during cache entry creation\n");
            // Still return the icon even if caching failed
        }
        #endif
    } else {
        #ifdef _DEBUG
        OutputDebugString("PlaylistIconManager::GetIcon - Failed to generate icon\n");
        #endif
    }

    return icon;
}

void PlaylistIconManager::DrawIcon(LICE_IBitmap* dest, IconType type, int x, int y, int size, int color)
{
    // TASK 11.1: Null pointer check
    if (!dest) {
        #ifdef _DEBUG
        OutputDebugString("PlaylistIconManager::DrawIcon - NULL destination bitmap\n");
        #endif
        return;
    }

    // TASK 11.1: Check if icon is available
    LICE_IBitmap* icon = GetIcon(type, size);
    if (icon) {
        if (color == -1) {
            // Use original icon colors
            LICE_Blit(dest, icon, x, y, 0, 0, size, size, 1.0f, LICE_BLIT_MODE_COPY | LICE_BLIT_USE_ALPHA);
        } else {
            // Apply color tint
            LICE_Blit(dest, icon, x, y, 0, 0, size, size, 1.0f, LICE_BLIT_MODE_COPY | LICE_BLIT_USE_ALPHA);
            // TODO: Implement color tinting if needed
        }
    } else {
        #ifdef _DEBUG
        OutputDebugString("PlaylistIconManager::DrawIcon - Failed to get icon\n");
        #endif
        // TASK 11.4: Graceful degradation - draw a simple placeholder
        // Draw a small rectangle as fallback
        LICE_FillRect(dest, x, y, size, size, LICE_RGBA(128, 128, 128, 255), 0.5f, LICE_BLIT_MODE_COPY);
    }
}

void PlaylistIconManager::ClearCache()
{
    for (int i = 0; i < m_iconCache.GetSize(); i++) {
        CacheEntry* entry = m_iconCache.Get(i);
        if (entry) {
            if (entry->bitmap) {
                delete entry->bitmap;
            }
            delete entry;
        }
    }
    m_iconCache.Empty();
}

LICE_IBitmap* PlaylistIconManager::GenerateIcon(IconType type, int size)
{
    // TASK 11.3: Handle bitmap creation failure
    LICE_IBitmap* bm = new LICE_MemBitmap(size, size);
    if (!bm) {
        #ifdef _DEBUG
        OutputDebugString("PlaylistIconManager::GenerateIcon - Failed to create bitmap\n");
        #endif
        return NULL;
    }

    // TASK 11.3: Use try-catch for icon generation
    #ifdef _DEBUG
    try {
    #endif
        // Clear bitmap with transparent background
        LICE_Clear(bm, LICE_RGBA(0, 0, 0, 0));

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
                #ifdef _DEBUG
                OutputDebugString("PlaylistIconManager::GenerateIcon - Unknown icon type\n");
                #endif
                break;
        }

        return bm;
    #ifdef _DEBUG
    }
    catch (...) {
        OutputDebugString("PlaylistIconManager::GenerateIcon - Exception during icon generation\n");
        // Clean up bitmap on failure
        if (bm) {
            delete bm;
        }
        return NULL;
    }
    #endif
}

void PlaylistIconManager::GeneratePlayIcon(LICE_IBitmap* bm, int size, int color)
{
    if (!bm) return;

    // Draw a triangle play icon pointing right
    // Scale padding based on size for better appearance
    float paddingRatio = 0.2f; // 20% padding
    int padding = (int)(size * paddingRatio);

    // Triangle vertices (pointing right)
    int x1 = padding;
    int y1 = padding;
    int x2 = padding;
    int y2 = size - padding;
    int x3 = size - padding;
    int y3 = size / 2;

    // Draw filled triangle
    LICE_FillTriangle(bm, x1, y1, x2, y2, x3, y3, color, 1.0f, LICE_BLIT_MODE_COPY);

    // Draw outline for better visibility at small sizes
    if (size >= 16) {
        LICE_Line(bm, x1, y1, x2, y2, color, 1.0f, LICE_BLIT_MODE_COPY, false);
        LICE_Line(bm, x2, y2, x3, y3, color, 1.0f, LICE_BLIT_MODE_COPY, false);
        LICE_Line(bm, x3, y3, x1, y1, color, 1.0f, LICE_BLIT_MODE_COPY, false);
    }
}

void PlaylistIconManager::GenerateNextIcon(LICE_IBitmap* bm, int size, int color)
{
    if (!bm) return;

    // Draw a double triangle (skip forward icon)
    float paddingRatio = 0.2f;
    int padding = (int)(size * paddingRatio);
    int mid = size / 2;
    int gap = size / 16; // Small gap between triangles

    // First triangle (left)
    int x1_1 = padding;
    int y1_1 = padding;
    int x1_2 = padding;
    int y1_2 = size - padding;
    int x1_3 = mid - gap;
    int y1_3 = size / 2;

    LICE_FillTriangle(bm, x1_1, y1_1, x1_2, y1_2, x1_3, y1_3, color, 1.0f, LICE_BLIT_MODE_COPY);

    // Second triangle (right)
    int x2_1 = mid + gap;
    int y2_1 = padding;
    int x2_2 = mid + gap;
    int y2_2 = size - padding;
    int x2_3 = size - padding;
    int y2_3 = size / 2;

    LICE_FillTriangle(bm, x2_1, y2_1, x2_2, y2_2, x2_3, y2_3, color, 1.0f, LICE_BLIT_MODE_COPY);
}

void PlaylistIconManager::GenerateWarningIcon(LICE_IBitmap* bm, int size, int color)
{
    if (!bm) return;

    // Draw exclamation mark in a triangle
    int centerX = size / 2;
    float paddingRatio = 0.15f;
    int padding = (int)(size * paddingRatio);

    // Draw warning triangle outline
    int triX1 = centerX;
    int triY1 = padding;
    int triX2 = padding;
    int triY2 = size - padding;
    int triX3 = size - padding;
    int triY3 = size - padding;

    LICE_Line(bm, triX1, triY1, triX2, triY2, color, 1.0f, LICE_BLIT_MODE_COPY, false);
    LICE_Line(bm, triX2, triY2, triX3, triY3, color, 1.0f, LICE_BLIT_MODE_COPY, false);
    LICE_Line(bm, triX3, triY3, triX1, triY1, color, 1.0f, LICE_BLIT_MODE_COPY, false);

    // Exclamation line (scaled based on size)
    int lineWidth = size >= 24 ? 2 : 1;
    int lineHeight = size / 2;
    LICE_FillRect(bm, centerX - lineWidth/2, padding + size/6, lineWidth, lineHeight, color, 1.0f, LICE_BLIT_MODE_COPY);

    // Exclamation dot
    int dotSize = size >= 24 ? 3 : 2;
    LICE_FillRect(bm, centerX - dotSize/2, size - padding - dotSize - 2, dotSize, dotSize, color, 1.0f, LICE_BLIT_MODE_COPY);
}

void PlaylistIconManager::GenerateLoopInfiniteIcon(LICE_IBitmap* bm, int size, int color)
{
    if (!bm) return;

    // Draw infinity symbol using two circles connected
    int centerY = size / 2;
    float radiusRatio = 0.25f;
    int radius = (int)(size * radiusRatio);

    // Position circles with slight overlap for infinity effect
    int leftX = size / 3;
    int rightX = 2 * size / 3;

    // Draw left circle (outline)
    LICE_Circle(bm, leftX, centerY, radius, color, 1.0f, LICE_BLIT_MODE_COPY, false);
    if (size >= 24) {
        // Draw thicker outline for larger sizes
        LICE_Circle(bm, leftX, centerY, radius - 1, color, 1.0f, LICE_BLIT_MODE_COPY, false);
    }

    // Draw right circle (outline)
    LICE_Circle(bm, rightX, centerY, radius, color, 1.0f, LICE_BLIT_MODE_COPY, false);
    if (size >= 24) {
        // Draw thicker outline for larger sizes
        LICE_Circle(bm, rightX, centerY, radius - 1, color, 1.0f, LICE_BLIT_MODE_COPY, false);
    }

    // Draw connecting lines to create infinity symbol effect
    int lineY1 = centerY - radius / 2;
    int lineY2 = centerY + radius / 2;

    // Top connecting curve
    LICE_Line(bm, leftX + radius/2, lineY1, rightX - radius/2, lineY1, color, 1.0f, LICE_BLIT_MODE_COPY, false);

    // Bottom connecting curve
    LICE_Line(bm, leftX + radius/2, lineY2, rightX - radius/2, lineY2, color, 1.0f, LICE_BLIT_MODE_COPY, false);
}

void PlaylistIconManager::GenerateSyncLossIcon(LICE_IBitmap* bm, int size, int color)
{
    if (!bm) return;

    // Draw red X with circle background
    float paddingRatio = 0.2f;
    int padding = (int)(size * paddingRatio);
    int redColor = LICE_RGBA(255, 0, 0, 255);

    // Draw circle background
    int centerX = size / 2;
    int centerY = size / 2;
    int radius = (size - padding * 2) / 2;
    LICE_Circle(bm, centerX, centerY, radius, redColor, 1.0f, LICE_BLIT_MODE_COPY, true);

    // Draw X with thicker lines for visibility
    int lineWidth = size >= 24 ? 2 : 1;
    int xPadding = padding + 2;

    // Draw diagonal lines multiple times for thickness
    for (int i = 0; i < lineWidth; i++) {
        // Diagonal line 1 (top-left to bottom-right)
        LICE_Line(bm, xPadding + i, xPadding, size - xPadding + i, size - xPadding, LICE_RGBA(255, 255, 255, 255), 1.0f, LICE_BLIT_MODE_COPY, false);
        LICE_Line(bm, xPadding, xPadding + i, size - xPadding, size - xPadding + i, LICE_RGBA(255, 255, 255, 255), 1.0f, LICE_BLIT_MODE_COPY, false);

        // Diagonal line 2 (top-right to bottom-left)
        LICE_Line(bm, size - xPadding - i, xPadding, xPadding - i, size - xPadding, LICE_RGBA(255, 255, 255, 255), 1.0f, LICE_BLIT_MODE_COPY, false);
        LICE_Line(bm, size - xPadding, xPadding + i, xPadding, size - xPadding + i, LICE_RGBA(255, 255, 255, 255), 1.0f, LICE_BLIT_MODE_COPY, false);
    }
}

int PlaylistIconManager::GetCacheKey(IconType type, int size) const
{
    return (type << 16) | size;
}
