# Implementation Plan

- [x] 1. Setup project structure and create base files
  - Create new header and source files for modern UI components
  - Add files to CMakeLists.txt build system
  - Create forward declarations and include guards
  - _Requirements: 1.1, 1.2, 1.3, 1.4, 1.5, 2.1, 2.2, 2.3, 2.4, 2.5, 3.1, 3.2, 3.3, 3.4, 3.5, 4.1, 4.2, 4.3, 4.4, 4.5, 5.1, 5.2, 5.3, 5.4, 5.5, 6.1, 6.2, 6.3, 6.4, 6.5, 7.1, 7.2, 7.3, 7.4, 7.5, 8.1, 8.2, 8.3, 8.4, 8.5_

- [x] 2. Implement PlaylistTheme system
- [x] 2.1 Create PlaylistTheme class with color and font structures
  - Define Colors struct with all required color fields
  - Define Fonts struct with LICE_CachedFont pointers
  - Implement singleton pattern for theme access
  - _Requirements: 5.1, 5.2, 5.3_

- [x] 2.2 Implement theme detection and loading
  - Detect REAPER theme (dark/light) using existing SNM functions
  - Load appropriate color scheme based on theme
  - Initialize fonts with correct sizes (12pt, 14pt, 11pt, 24pt, 20pt)
  - _Requirements: 5.1, 5.2, 4.1, 4.2, 4.3_

- [x] 2.3 Add color calculation utilities
  - Implement hover color calculation (lighter/darker variants)
  - Implement contrast ratio validation (4.5:1, 3:1, 7:1)
  - Add color blending functions for smooth transitions
  - _Requirements: 1.4, 1.5, 4.4, 5.3_

- [x] 2.4 Implement theme customization from INI file
  - Read custom colors from reaper.ini [sws_playlist_theme_dark] section
  - Read custom colors from reaper.ini [sws_playlist_theme_light] section
  - Provide fallback to default colors if custom not found
  - _Requirements: 5.1, 5.2_

- [ ] 3. Implement PlaylistIconManager
- [ ] 3.1 Create icon manager singleton class
  - Define IconType enum (PLAY, NEXT, WARNING, LOOP_INFINITE, SYNC_LOSS)
  - Implement icon caching system using WDL_PtrList
  - Create GetIcon() and DrawIcon() methods
  - _Requirements: 2.1, 2.2, 2.3, 2.4, 2.5_

- [ ] 3.2 Generate play icon programmatically
  - Draw triangle play symbol using LICE drawing functions
  - Support multiple sizes (16x16, 24x24, 32x32)
  - Allow color customization
  - _Requirements: 2.1_

- [ ] 3.3 Generate next/queue icon programmatically
  - Draw circle or forward arrow symbol
  - Support multiple sizes
  - Allow color customization
  - _Requirements: 2.2_

- [ ] 3.4 Generate warning and sync loss icons
  - Draw exclamation mark for warning
  - Draw red X or alert symbol for sync loss
  - Support multiple sizes
  - _Requirements: 2.5_

- [ ] 3.5 Generate infinity loop icon
  - Draw infinity symbol (∞) using LICE curves or text
  - Support multiple sizes
  - Allow color customization
  - _Requirements: 2.3_

- [ ] 4. Implement ModernPlaylistItemRenderer
- [ ] 4.1 Create ItemVisualState and ItemData structures
  - Define ItemVisualState with all boolean flags and counts
  - Define ItemData with region info and timing data
  - Add validation methods
  - _Requirements: 1.1, 1.2, 1.3, 1.4, 1.5, 2.1, 2.2, 2.3, 2.4, 2.5_

- [ ] 4.2 Implement DrawBackground method
  - Draw rounded rectangle background with 4px radius
  - Apply appropriate color based on item state (playing, next, selected, hovered)
  - Add subtle shadow or border for depth
  - _Requirements: 1.4, 1.5, 5.4, 5.5, 6.1_

- [ ] 4.3 Implement DrawStatusIcon method
  - Position icon at left side of item (16x16 minimum)
  - Draw play icon for currently playing item
  - Draw next icon for queued item
  - Draw warning icon for sync loss
  - _Requirements: 2.1, 2.2, 2.5_

- [ ] 4.4 Implement DrawRegionNumber method
  - Draw region number with 14pt font minimum
  - Position after status icon with proper spacing
  - Use theme colors for text
  - _Requirements: 1.2_

- [ ] 4.5 Implement DrawRegionName method
  - Draw region name with 12pt font minimum
  - Truncate long names with ellipsis
  - Position after region number
  - _Requirements: 1.3_

- [ ] 4.6 Implement DrawTimeInfo method
  - Format duration as MM:SS
  - Draw with 11pt font minimum
  - Position at right side of item
  - Show elapsed/remaining time if item is playing
  - _Requirements: 3.1, 3.3, 3.4_

- [ ] 4.7 Implement DrawLoopBadge method
  - Draw badge with contrasting background for loop count
  - Draw infinity symbol for infinite loops (14x14 minimum)
  - Position near time info
  - _Requirements: 2.3, 2.4_

- [ ] 4.8 Implement DrawRoundedRect utility
  - Use LICE functions to draw rounded corners
  - Support configurable radius (4px default)
  - Optimize for performance
  - _Requirements: 5.4_

- [ ] 4.9 Implement main DrawItem method
  - Coordinate all drawing sub-methods
  - Apply 4px spacing between items
  - Handle edge cases (null pointers, invalid data)
  - Implement fallback rendering if errors occur
  - _Requirements: 1.1, 1.2, 1.3, 1.4, 1.5, 2.1, 2.2, 2.3, 2.4, 2.5, 3.1, 3.3, 3.4_

- [ ] 5. Implement ModernRegionPlaylistView
- [ ] 5.1 Create ModernRegionPlaylistView class extending RegionPlaylistView
  - Add member variables (renderer, theme, itemHeight, hoveredItem, modernRenderingEnabled)
  - Implement constructor and destructor
  - Initialize with default values
  - _Requirements: 1.1, 1.2, 1.3, 1.4, 1.5_

- [ ] 5.2 Override GetItemText method
  - Maintain compatibility with base class
  - Add modern formatting for text columns
  - Handle null/invalid items gracefully
  - _Requirements: 1.2, 1.3, 3.1_

- [ ] 5.3 Implement OnItemPaint method
  - Check if modern rendering is enabled
  - Get item data and visual state
  - Call ModernPlaylistItemRenderer::DrawItem
  - Implement try-catch for error handling with fallback
  - _Requirements: 1.1, 1.2, 1.3, 1.4, 1.5, 6.2, 7.1_

- [ ] 5.4 Implement hover tracking
  - Override OnMouseMove to track hovered item
  - Update m_hoveredItem index
  - Trigger repaint when hover changes
  - Implement 150ms transition for hover effect
  - _Requirements: 6.1_

- [ ] 5.5 Implement GetItemState helper method
  - Determine if item is playing, next, selected, hovered
  - Check for sync loss state
  - Get loop count and infinite loop flag
  - Return ItemVisualState structure
  - _Requirements: 1.4, 1.5, 2.1, 2.2, 2.3, 2.4, 2.5_

- [ ] 5.6 Implement GetItemData helper method
  - Extract region ID, number, name from RgnPlaylistItem
  - Get start/end positions and calculate duration
  - Validate data before returning
  - Return ItemData structure
  - _Requirements: 1.2, 1.3, 3.1, 3.2_

- [ ] 5.7 Implement SetItemHeight and GetItemHeight methods
  - Allow configurable item height
  - Ensure minimum height for readability
  - Trigger layout update when height changes
  - _Requirements: 1.1_

- [ ] 5.8 Implement EnableModernRendering toggle
  - Add flag to enable/disable modern rendering
  - Provide fallback to classic rendering
  - Save preference to reaper.ini
  - _Requirements: 7.1, 7.2, 7.3, 7.4, 7.5_

- [ ] 5.9 Implement UpdateTheme method
  - Detect theme changes from REAPER
  - Reload PlaylistTheme colors and fonts
  - Trigger full repaint
  - _Requirements: 5.1, 5.2_

- [ ] 5.10 Implement auto-scroll to current item
  - Detect when currently playing item changes
  - Scroll list to keep current item visible
  - Ensure next item is also visible if possible
  - _Requirements: 8.1, 8.2_

- [ ] 6. Implement ModernMonitoringView
- [ ] 6.1 Create ModernMonitoringView class extending SNM_FiveMonitors
  - Add member variables for progress info and region data
  - Implement constructor
  - Initialize with default values
  - _Requirements: 4.1, 4.2, 4.3, 4.4, 4.5_

- [ ] 6.2 Override OnPaint method for monitoring mode
  - Use high contrast colors (7:1 ratio minimum)
  - Draw large text for current region (24pt font)
  - Draw medium text for next region (20pt font)
  - Draw playlist info at top
  - _Requirements: 4.1, 4.2, 4.3, 4.4_

- [ ] 6.3 Implement DrawProgressBar method
  - Calculate progress percentage from current/total time
  - Draw filled bar with theme progress color
  - Draw border around progress bar
  - Update smoothly during playback
  - _Requirements: 4.5_

- [ ] 6.4 Implement SetProgress method
  - Accept current and total time values
  - Calculate percentage
  - Store in ProgressInfo structure
  - Trigger repaint if changed
  - _Requirements: 4.5_

- [ ] 6.5 Implement SetCurrentRegion and SetNextRegion methods
  - Store region name and number
  - Trigger repaint if changed
  - Handle null/empty names gracefully
  - _Requirements: 4.1, 4.2, 4.3_

- [ ] 6.6 Implement SetPlaylistInfo method
  - Store playlist name and number
  - Display at top of monitoring view
  - Use appropriate font size
  - _Requirements: 4.1_

- [ ] 7. Integrate modern UI into RegionPlaylistWnd
- [ ] 7.1 Modify RegionPlaylistWnd to use ModernRegionPlaylistView
  - Replace RegionPlaylistView instantiation with ModernRegionPlaylistView
  - Pass necessary parameters to constructor
  - Maintain backward compatibility
  - _Requirements: 1.1, 1.2, 1.3, 1.4, 1.5_

- [ ] 7.2 Modify RegionPlaylistWnd to use ModernMonitoringView
  - Replace SNM_FiveMonitors with ModernMonitoringView in monitoring mode
  - Update monitoring update logic to call new methods
  - Maintain backward compatibility
  - _Requirements: 4.1, 4.2, 4.3, 4.4, 4.5_

- [ ] 7.3 Update RegionPlaylistWnd::Update method
  - Call UpdateTheme when theme changes detected
  - Update progress bar in monitoring mode
  - Ensure smooth updates during playback (< 100ms)
  - _Requirements: 7.1, 7.4_

- [ ] 7.4 Update RegionPlaylistWnd::UpdateMonitoring method
  - Pass monitoring data to ModernMonitoringView
  - Update progress information
  - Ensure updates happen at <= 100ms intervals
  - _Requirements: 4.5, 7.4_

- [ ] 7.5 Add UI preference controls
  - Add menu items to toggle modern UI on/off
  - Add menu items to adjust item height
  - Add menu items to toggle animations
  - Save preferences to reaper.ini
  - _Requirements: 5.1, 5.2, 5.3, 5.4, 5.5_

- [ ] 8. Implement drag and drop visual feedback
- [ ] 8.1 Override OnBeginDrag in ModernRegionPlaylistView
  - Create ghost image of dragged item
  - Use semi-transparent rendering
  - Show item being dragged
  - _Requirements: 6.3_

- [ ] 8.2 Override OnDrag in ModernRegionPlaylistView
  - Highlight valid drop targets
  - Use distinct color for drop target
  - Update highlight as mouse moves
  - _Requirements: 6.4_

- [ ] 8.3 Override OnEndDrag in ModernRegionPlaylistView
  - Clear ghost image
  - Clear drop target highlights
  - Provide visual feedback within 50ms
  - _Requirements: 6.2, 6.5_

- [ ] 9. Implement performance optimizations
- [ ] 9.1 Add icon caching to PlaylistIconManager
  - Cache rendered icons in memory
  - Reuse cached icons instead of regenerating
  - Clear cache when theme changes
  - _Requirements: 7.1, 7.2_

- [ ] 9.2 Add font caching to PlaylistTheme
  - Cache loaded fonts
  - Reuse cached fonts for rendering
  - Clear cache when font size changes
  - _Requirements: 7.1, 7.2_

- [ ] 9.3 Implement dirty region tracking
  - Track which items need repainting
  - Only repaint changed items
  - Optimize full list repaints
  - _Requirements: 7.1, 7.2, 7.3_

- [ ] 9.4 Optimize DrawItem for large playlists
  - Minimize LICE drawing calls
  - Batch similar operations
  - Profile and optimize hot paths
  - Ensure > 30 FPS with 100+ items
  - _Requirements: 7.2_

- [ ] 9.5 Implement double buffering
  - Use LICE_IBitmap for off-screen rendering
  - Blit to screen in single operation
  - Eliminate flickering during updates
  - _Requirements: 7.1, 7.3_

- [ ] 10. Add platform-specific adjustments
- [ ] 10.1 Test and adjust for Windows
  - Verify High DPI support
  - Test with ClearType font rendering
  - Adjust colors if needed for Windows theme
  - _Requirements: 5.1, 5.2, 5.3_

- [ ] 10.2 Test and adjust for macOS
  - Verify Retina display support
  - Test with different macOS themes
  - Adjust rendering for Core Graphics via SWELL
  - _Requirements: 5.1, 5.2, 5.3_

- [ ] 10.3 Test and adjust for Linux
  - Test with different window managers
  - Verify font rendering with Cairo via SWELL
  - Adjust colors for common Linux themes
  - _Requirements: 5.1, 5.2, 5.3_

- [ ] 11. Implement error handling and fallbacks
- [ ] 11.1 Add null pointer checks throughout
  - Check all pointer parameters before use
  - Return early or use fallback if null
  - Log errors in debug mode
  - _Requirements: 7.1, 7.2, 7.3, 7.4, 7.5_

- [ ] 11.2 Add validation for region IDs and data
  - Validate region IDs before accessing
  - Check for valid region data
  - Handle deleted or invalid regions gracefully
  - _Requirements: 7.1, 7.2_

- [ ] 11.3 Implement fallback to classic rendering
  - Detect rendering failures
  - Fall back to RegionPlaylistView rendering
  - Log fallback events for debugging
  - _Requirements: 7.1_

- [ ] 11.4 Add graceful degradation for missing resources
  - Use default colors if theme fails to load
  - Use text fallback if icons fail to load
  - Use system font if custom fonts fail to load
  - _Requirements: 5.1, 5.2_

- [ ] 12. Manual testing and validation
- [ ] 12.1 Test basic rendering with small playlist
  - Create playlist with 10 regions
  - Verify all items display correctly
  - Check spacing, fonts, and colors
  - _Requirements: 1.1, 1.2, 1.3, 1.4, 1.5_

- [ ] 12.2 Test playback states and transitions
  - Start playback and verify current item highlight
  - Verify next item indicator
  - Test transition between regions
  - Test sync loss state
  - _Requirements: 2.1, 2.2, 2.5_

- [ ] 12.3 Test user interactions
  - Test hover effects on items
  - Test click feedback
  - Test drag and drop with visual feedback
  - Verify all interactions respond within 50-150ms
  - _Requirements: 6.1, 6.2, 6.3, 6.4, 6.5_

- [ ] 12.4 Test monitoring mode
  - Switch to monitoring mode
  - Verify large fonts (24pt, 20pt)
  - Verify progress bar updates
  - Test from distance (readability)
  - _Requirements: 4.1, 4.2, 4.3, 4.4, 4.5_

- [ ] 12.5 Test with large playlist (100+ items)
  - Create playlist with 200 items
  - Test scroll performance (verify > 30 FPS)
  - Test playback updates
  - Verify no lag or stuttering
  - _Requirements: 7.2, 7.3_

- [ ] 12.6 Test edge cases
  - Test with empty playlist
  - Test with very long region names
  - Test with very short regions (< 0.5s)
  - Test with infinite loops
  - Test theme switching
  - _Requirements: 1.1, 1.2, 1.3, 2.3, 5.1, 5.2_

- [ ] 12.7 Test on all platforms
  - Test on Windows (High DPI)
  - Test on macOS (Retina)
  - Test on Linux (various WMs)
  - Verify consistent appearance
  - _Requirements: 5.1, 5.2, 5.3_

- [ ] 13. Documentation and cleanup
- [ ] 13.1 Add code comments and documentation
  - Document all public methods
  - Add inline comments for complex logic
  - Document performance considerations
  - _Requirements: All_

- [ ] 13.2 Update build system
  - Verify CMakeLists.txt includes all new files
  - Test clean build on all platforms
  - Verify no build warnings
  - _Requirements: All_

- [ ] 13.3 Create user documentation
  - Document new UI features
  - Document preferences and customization
  - Create before/after screenshots
  - _Requirements: All_

- [ ] 13.4 Clean up debug code
  - Remove or conditionally compile debug logging
  - Remove test code
  - Optimize final build
  - _Requirements: 7.1, 7.2, 7.3, 7.4, 7.5_
