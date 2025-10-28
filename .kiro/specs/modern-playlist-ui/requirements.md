# Requirements Document

## Introduction

Este documento define os requisitos para modernizar a interface da Region Playlist no dock do plugin SWS para REAPER. A playlist atual possui uma visualização básica em lista que não é intuitiva para uso em shows ao vivo, onde a clareza visual e a facilidade de navegação são críticas. O objetivo é criar uma interface moderna, visualmente clara e otimizada para performances ao vivo sem margem de erro.

## Glossary

- **Region Playlist Window**: A janela/dock do SWS que exibe e controla playlists de regiões
- **Region**: Uma seção marcada no projeto REAPER com início e fim definidos
- **Playlist Item**: Uma entrada na playlist que referencia uma região específica
- **Monitoring Mode**: Modo de visualização que mostra informações sobre a reprodução atual
- **Edition Mode**: Modo que permite editar a playlist
- **Loop Count**: Número de vezes que uma região deve ser repetida
- **SWS_ListView**: Componente de interface que exibe a lista de itens da playlist
- **VWnd (Virtual Window)**: Sistema de controles virtuais usado pelo SWS para interface gráfica

## Requirements

### Requirement 1: Visualização Clara de Itens da Playlist

**User Story:** Como operador de show ao vivo, eu quero ver claramente cada música/bloco na playlist, para que eu possa identificar rapidamente qual item está tocando e qual será o próximo.

#### Acceptance Criteria

1. WHEN THE Region Playlist Window displays playlist items, THE Region Playlist Window SHALL render each item with visual separation of at least 4 pixels between items
2. WHEN THE Region Playlist Window displays a playlist item, THE Region Playlist Window SHALL show the region number with font size at least 14 points
3. WHEN THE Region Playlist Window displays a playlist item, THE Region Playlist Window SHALL show the region name with font size at least 12 points
4. WHEN THE Region Playlist Window displays the currently playing item, THE Region Playlist Window SHALL highlight the item with a distinct background color with contrast ratio of at least 4.5:1
5. WHEN THE Region Playlist Window displays the next item to play, THE Region Playlist Window SHALL highlight the item with a secondary distinct color with contrast ratio of at least 3:1

### Requirement 2: Indicadores Visuais de Status

**User Story:** Como operador de show ao vivo, eu quero ver indicadores visuais claros do status de cada item, para que eu possa entender rapidamente o estado da playlist sem ler texto.

#### Acceptance Criteria

1. WHEN a playlist item is currently playing, THE Region Playlist Window SHALL display a play icon with minimum size of 16x16 pixels adjacent to the item
2. WHEN a playlist item is queued as next, THE Region Playlist Window SHALL display a queue icon with minimum size of 16x16 pixels adjacent to the item
3. WHEN a playlist item has infinite loop enabled, THE Region Playlist Window SHALL display an infinity symbol with minimum size of 14x14 pixels
4. WHEN a playlist item has a loop count greater than 1, THE Region Playlist Window SHALL display the loop count in a badge with contrasting background color
5. WHEN THE Region Playlist Window is in sync loss state, THE Region Playlist Window SHALL display a warning icon with red color code #FF0000 or equivalent

### Requirement 3: Informações de Tempo Visíveis

**User Story:** Como operador de show ao vivo, eu quero ver claramente a duração de cada item e o tempo total, para que eu possa gerenciar o timing do show.

#### Acceptance Criteria

1. WHEN THE Region Playlist Window displays a playlist item, THE Region Playlist Window SHALL show the item duration in MM:SS format with font size at least 11 points
2. WHEN THE Region Playlist Window displays a playlist, THE Region Playlist Window SHALL show the total playlist duration in the header area
3. WHEN a playlist item is playing, THE Region Playlist Window SHALL display the elapsed time within the current item
4. WHEN a playlist item is playing, THE Region Playlist Window SHALL display the remaining time for the current item
5. WHEN THE Region Playlist Window calculates total duration with infinite loops, THE Region Playlist Window SHALL display the infinity symbol instead of a numeric value

### Requirement 4: Modo de Monitoramento Aprimorado

**User Story:** Como operador de show ao vivo, eu quero um modo de monitoramento com informações grandes e claras, para que eu possa ver o status da playlist de longe durante a performance.

#### Acceptance Criteria

1. WHEN THE Region Playlist Window is in monitoring mode, THE Region Playlist Window SHALL display the current region name with font size at least 24 points
2. WHEN THE Region Playlist Window is in monitoring mode, THE Region Playlist Window SHALL display the next region name with font size at least 20 points
3. WHEN THE Region Playlist Window is in monitoring mode, THE Region Playlist Window SHALL display the current region number with font size at least 20 points
4. WHEN THE Region Playlist Window is in monitoring mode, THE Region Playlist Window SHALL use high contrast colors with minimum contrast ratio of 7:1
5. WHEN THE Region Playlist Window is in monitoring mode, THE Region Playlist Window SHALL display a progress bar showing the position within the current region

### Requirement 5: Cores e Temas Modernos

**User Story:** Como usuário do REAPER, eu quero que a interface da playlist siga padrões modernos de design, para que a ferramenta seja agradável de usar e profissional.

#### Acceptance Criteria

1. THE Region Playlist Window SHALL support dark theme with background color luminance below 20%
2. THE Region Playlist Window SHALL support light theme with background color luminance above 80%
3. WHEN THE Region Playlist Window displays selected items, THE Region Playlist Window SHALL use an accent color that is visually distinct from the background
4. THE Region Playlist Window SHALL use rounded corners with radius of 4 pixels for item containers
5. THE Region Playlist Window SHALL apply subtle shadows or borders to create visual depth between UI elements

### Requirement 6: Feedback Visual de Interação

**User Story:** Como usuário, eu quero feedback visual claro quando interajo com a playlist, para que eu saiba que minhas ações foram reconhecidas.

#### Acceptance Criteria

1. WHEN the user hovers over a playlist item, THE Region Playlist Window SHALL change the item background color with transition duration of 150 milliseconds
2. WHEN the user clicks a playlist item, THE Region Playlist Window SHALL provide visual feedback within 50 milliseconds
3. WHEN the user drags a playlist item, THE Region Playlist Window SHALL display a ghost image of the item being dragged
4. WHEN a playlist item is being dragged over a valid drop target, THE Region Playlist Window SHALL highlight the drop target with a distinct color
5. WHEN the user performs an invalid action, THE Region Playlist Window SHALL display visual feedback indicating the action cannot be completed

### Requirement 7: Responsividade e Performance

**User Story:** Como operador de show ao vivo, eu preciso que a interface responda instantaneamente, para que não haja atrasos durante a performance.

#### Acceptance Criteria

1. WHEN THE Region Playlist Window updates the current playing item, THE Region Playlist Window SHALL complete the visual update within 100 milliseconds
2. WHEN THE Region Playlist Window renders a playlist with 100 items, THE Region Playlist Window SHALL maintain frame rate above 30 FPS during scrolling
3. WHEN the user resizes THE Region Playlist Window, THE Region Playlist Window SHALL reflow the layout within 200 milliseconds
4. THE Region Playlist Window SHALL update monitoring information at intervals not exceeding 100 milliseconds during playback
5. WHEN THE Region Playlist Window receives a playlist update event, THE Region Playlist Window SHALL refresh the display within 150 milliseconds

### Requirement 8: Acessibilidade de Informações Críticas

**User Story:** Como operador de show ao vivo, eu quero que as informações mais importantes estejam sempre visíveis, para que eu não precise rolar ou procurar durante a performance.

#### Acceptance Criteria

1. WHEN THE Region Playlist Window is in edition mode, THE Region Playlist Window SHALL keep the currently playing item visible by auto-scrolling when necessary
2. WHEN THE Region Playlist Window displays a playlist, THE Region Playlist Window SHALL show at minimum the current item, next item, and previous item simultaneously
3. WHEN THE Region Playlist Window has limited vertical space, THE Region Playlist Window SHALL prioritize displaying the current and next items over other items
4. THE Region Playlist Window SHALL display transport controls with minimum touch target size of 32x32 pixels
5. WHEN THE Region Playlist Window displays playlist information, THE Region Playlist Window SHALL show the playlist name and number in a fixed header that remains visible during scrolling
