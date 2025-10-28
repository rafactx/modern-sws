/******************************************************************************
/ SnM_PlaylistIcons.h
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

#ifndef _SNM_PLAYLISTICONS_H_
#define _SNM_PLAYLISTICONS_H_

#include "../WDL/lice/lice.h"
#include "../WDL/ptrlist.h"

///////////////////////////////////////////////////////////////////////////////
// PlaylistIconManager - Manages icons for playlist UI
///////////////////////////////////////////////////////////////////////////////

class PlaylistIconManager {
public:
    // Icon types
    enum IconType {
        ICON_PLAY,
        ICON_NEXT,
        ICON_WARNING,
        ICON_LOOP_INFINITE,
        ICON_SYNC_LOSS,
        ICON_TYPE_COUNT
    };

    // Singleton access
    static PlaylistIconManager* GetInstance();

    // Icon access
    LICE_IBitmap* GetIcon(IconType type, int size = 16);
    void DrawIcon(LICE_IBitmap* dest, IconType type, int x, int y, int size = 16, int color = -1);

    // Cache management
    void ClearCache();

private:
    PlaylistIconManager();
    ~PlaylistIconManager();

    // Prevent copying
    PlaylistIconManager(const PlaylistIconManager&);
    PlaylistIconManager& operator=(const PlaylistIconManager&);

    // Icon generation
    void GenerateIcon(IconType type, int size);
    void GeneratePlayIcon(LICE_IBitmap* bm, int size, int color);
    void GenerateNextIcon(LICE_IBitmap* bm, int size, int color);
    void GenerateWarningIcon(LICE_IBitmap* bm, int size, int color);
    void GenerateLoopInfiniteIcon(LICE_IBitmap* bm, int size, int color);
    void GenerateSyncLossIcon(LICE_IBitmap* bm, int size, int color);

    // Cache key generation
    int GetCacheKey(IconType type, int size) const;

    WDL_PtrList<LICE_IBitmap> m_iconCache;

    static PlaylistIconManager* s_instance;
};

#endif // _SNM_PLAYLISTICONS_H_
