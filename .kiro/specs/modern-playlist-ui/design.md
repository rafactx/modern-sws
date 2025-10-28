# Design Document - Modern Playlist UI

## Overview

Este documento descreve o design técnico para modernizar a interface da Region Playlist no plugin SWS para REAPER. A solução mantém a arquitetura existente baseada em `SWS_ListView` e `WDL_VWnd` (Virtual Windows), mas adiciona camadas de renderização customizadas, novos componentes visuais e melhorias de layout para criar uma experiência moderna e otimizada para shows ao vivo.

A abordagem é conservadora: modificamos apenas os componentes de visualização sem alterar a lógica de negócio da playlist, garantindo compatibilidade e estabilidade.

## Architecture

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                  RegionPlaylistWnd                          │
│                  (SWS_DockWnd)                              │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────────────────────────────────────────────┐  │
│  │         Modern Header Component                      │  │
│  │  - Playlist Info  - Transport Controls  - Status     │  │
│  └──────────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────────┐  │
│  │      ModernRegionPlaylistView                        │  │
│  │      (extends RegionPlaylistView)                    │  │
│  │  ┌────────────────────────────────────────────────┐  │  │
│  │  │  ModernPlaylistItemRenderer                    │  │  │
│  │  │  - Custom drawing for each item                │  │  │
│  │  │  - Icons, colors, spacing                      │  │  │
│  │  └────────────────────────────────────────────────┘  │  │
│  └──────────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────────┐  │
│  │      ModernMonitoringView                            │  │
│  │      (enhanced SNM_FiveMonitors)                     │  │
│  │  - Large text display                                │  │
│  │  - Progress bar                                      │  │
│  │  - High contrast colors                              │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### Component Hierarchy

1. **RegionPlaylistWnd** (existing, modified)
   - Gerencia o layout geral da janela
   - Coordena entre modo de edição e monitoramento
   - Mantém referências aos componentes visuais

2. **ModernRegionPlaylistView** (new, extends RegionPlaylistView)
   - Herda toda a lógica de `RegionPlaylistView`
   - Override de `GetItemText()` para formatação customizada
   - Override de `OnPaint()` para renderização moderna
   - Adiciona `DrawModernItem()` para desenhar cada item

3. **ModernPlaylistItemRenderer** (new helper class)
   - Classe auxiliar para renderizar itens individuais
   - Encapsula lógica de desenho de ícones, badges, cores
   - Reutilizável e testável

4. **ModernMonitoringView** (new, extends SNM_FiveMonitors)
   - Versão aprimorada do modo de monitoramento
   - Adiciona barra de progresso
   - Melhora contraste e tamanhos de fonte

5. **PlaylistTheme** (new)
   - Gerencia cores, fontes e estilos
   - Suporta temas dark e light
   - Integra-se com o sistema de temas do REAPER

## Components and Interfaces

### 1. PlaylistTheme

```cpp
class PlaylistTheme {
public:
    struct Colors {
        int background;
        int text;
        int currentItemBg;
        int currentItemText;
        int nextItemBg;
        int nextItemText;
        int selectedBg;
        int selectedText;
        int hoverBg;
        int border;
        int progressBar;
        int warningRed;
        int accentBlue;
    };

    struct Fonts {
        LICE_CachedFont* itemName;      // 12pt
        LICE_CachedFont* itemNumber;    // 14pt
        LICE_CachedFont* itemTime;      // 11pt
        LICE_CachedFont* monitorLarge;  // 24pt
        LICE_CachedFont* monitorMedium; // 20pt
    };

    static PlaylistTheme* GetInstance();
    void LoadTheme(bool isDark);
    const Colors& GetColors() const;
    const Fonts& GetFonts() const;

private:
    Colors m_colors;
    Fonts m_fonts;
    bool m_isDark;
};
```

**Responsabilidades:**
- Centralizar todas as definições de cores e fontes
- Detectar tema do REAPER (dark/light)
- Fornecer acesso thread-safe às configurações visuais
- Calcular cores derivadas (hover, disabled, etc.)

### 2. ModernPlaylistItemRenderer

```cpp
class ModernPlaylistItemRenderer {
public:
    struct ItemVisualState {
        bool isPlaying;
        bool isNext;
        bool isSelected;
        bool isHovered;
        bool hasInfiniteLoop;
        int loopCount;
        bool isSyncLoss;
    };

    struct ItemData {
        int regionNumber;
        WDL_FastString regionName;
        double startTime;
        double endTime;
        double duration;
    };

    void DrawItem(
        LICE_IBitmap* drawbm,
        const RECT& itemRect,
        const ItemData& data,
        const ItemVisualState& state,
        const PlaylistTheme* theme
    );

private:
    void DrawBackground(LICE_IBitmap* bm, const RECT& r, const ItemVisualState& state, const PlaylistTheme* theme);
    void DrawStatusIcon(LICE_IBitmap* bm, const RECT& r, const ItemVisualState& state);
    void DrawRegionNumber(LICE_IBitmap* bm, const RECT& r, int number, const PlaylistTheme* theme);
    void DrawRegionName(LICE_IBitmap* bm, const RECT& r, const char* name, const PlaylistTheme* theme);
    void DrawTimeInfo(LICE_IBitmap* bm, const RECT& r, const ItemData& data, const PlaylistTheme* theme);
    void DrawLoopBadge(LICE_IBitmap* bm, const RECT& r, int count, bool infinite, const PlaylistTheme* theme);
    void DrawRoundedRect(LICE_IBitmap* bm, const RECT& r, int radius, int color);
};
```

**Responsabilidades:**
- Renderizar cada item da playlist com estilo moderno
- Desenhar ícones de status (play, next, warning)
- Aplicar cores baseadas no estado do item
- Desenhar badges para loops
- Criar efeitos visuais (cantos arredondados, sombras)

### 3. ModernRegionPlaylistView

```cpp
class ModernRegionPlaylistView : public RegionPlaylistView {
public:
    ModernRegionPlaylistView(HWND hwndList, HWND hwndEdit);
    virtual ~ModernRegionPlaylistView();

protected:
    // Override métodos de renderização
    void GetItemText(SWS_ListItem* item, int iCol, char* str, int iStrMax) override;
    void OnItemPaint(LICE_IBitmap* drawbm, SWS_ListItem* item, const RECT& itemRect);

    // Novos métodos
    void SetItemHeight(int height);
    int GetItemHeight() const;
    void EnableModernRendering(bool enable);
    bool IsModernRenderingEnabled() const;

    // Hover tracking
    void OnMouseMove(int x, int y);
    int GetHoveredItemIndex() const;

private:
    ModernPlaylistItemRenderer m_renderer;
    PlaylistTheme* m_theme;
    int m_itemHeight;
    int m_hoveredItem;
    bool m_modernRenderingEnabled;

    void UpdateTheme();
    ModernPlaylistItemRenderer::ItemVisualState GetItemState(SWS_ListItem* item, int index);
    ModernPlaylistItemRenderer::ItemData GetItemData(RgnPlaylistItem* item);
};
```

**Responsabilidades:**
- Estender `RegionPlaylistView` sem quebrar funcionalidade existente
- Coordenar renderização moderna de itens
- Gerenciar estado de hover
- Controlar altura dos itens
- Integrar com o sistema de temas

### 4. ModernMonitoringView

```cpp
class ModernMonitoringView : public SNM_FiveMonitors {
public:
    ModernMonitoringView();

    void OnPaint(LICE_IBitmap* drawbm, int origin_x, int origin_y, RECT* cliprect, int rscale) override;
    void SetProgress(double current, double total);
    void SetCurrentRegion(const char* name, int number);
    void SetNextRegion(const char* name, int number);
    void SetPlaylistInfo(const char* playlistName, int playlistNumber);

private:
    struct ProgressInfo {
        double current;
        double total;
        double percentage;
    };

    ProgressInfo m_progress;
    WDL_FastString m_currentName;
    WDL_FastString m_nextName;
    int m_currentNumber;
    int m_nextNumber;

    void DrawProgressBar(LICE_IBitmap* bm, const RECT& r);
    void DrawLargeText(LICE_IBitmap* bm, const RECT& r, const char* text, LICE_CachedFont* font, int color);
};
```

**Responsabilidades:**
- Exibir informações de monitoramento com fontes grandes
- Desenhar barra de progresso da região atual
- Usar cores de alto contraste
- Otimizar para visualização à distância

### 5. PlaylistIconManager

```cpp
class PlaylistIconManager {
public:
    enum IconType {
        ICON_PLAY,
        ICON_NEXT,
        ICON_WARNING,
        ICON_LOOP_INFINITE,
        ICON_SYNC_LOSS
    };

    static PlaylistIconManager* GetInstance();

    LICE_IBitmap* GetIcon(IconType type, int size = 16);
    void DrawIcon(LICE_IBitmap* dest, IconType type, int x, int y, int size = 16, int color = -1);

private:
    void LoadIcons();
    void GenerateIcon(IconType type, int size);

    WDL_PtrList<LICE_IBitmap> m_icons;
};
```

**Responsabilidades:**
- Gerenciar ícones usados na interface
- Gerar ícones programaticamente (para evitar dependências de arquivos)
- Cachear ícones em diferentes tamanhos
- Permitir colorização de ícones

## Data Models

### ItemRenderData

Estrutura que encapsula todos os dados necessários para renderizar um item:

```cpp
struct ItemRenderData {
    // Identificação
    int regionId;
    int regionNumber;
    WDL_FastString regionName;

    // Timing
    double startPos;
    double endPos;
    double duration;

    // Estado
    bool isPlaying;
    bool isNext;
    bool isSelected;
    bool isHovered;
    bool isSyncLoss;

    // Loop
    int loopCount;
    bool hasInfiniteLoop;

    // Validação
    bool isValid;
};
```

### MonitoringData

Estrutura para dados do modo de monitoramento:

```cpp
struct MonitoringData {
    // Playlist atual
    WDL_FastString playlistName;
    int playlistNumber;

    // Região atual
    WDL_FastString currentRegionName;
    int currentRegionNumber;
    double currentRegionPos;
    double currentRegionEnd;

    // Próxima região
    WDL_FastString nextRegionName;
    int nextRegionNumber;

    // Estado
    bool isPlaying;
    bool isSyncLoss;
    bool isEndOfPlaylist;

    // Progresso
    double playPosition;
    double progressPercentage;
};
```

## Error Handling

### Estratégia de Error Handling

1. **Validação de Dados**
   - Verificar ponteiros nulos antes de acessar
   - Validar IDs de região antes de renderizar
   - Verificar bounds de arrays e listas

2. **Fallback Rendering**
   - Se renderização moderna falhar, usar renderização clássica
   - Manter flag `m_modernRenderingEnabled` para controle

3. **Logging**
   - Usar `OutputDebugString` para debug (já usado no código existente)
   - Adicionar logs apenas em modo debug para não impactar performance

4. **Graceful Degradation**
   - Se tema não carregar, usar cores padrão
   - Se ícones não carregarem, usar texto alternativo
   - Se fontes não carregarem, usar fonte do sistema

### Exemplo de Tratamento

```cpp
void ModernRegionPlaylistView::OnItemPaint(LICE_IBitmap* drawbm, SWS_ListItem* item, const RECT& itemRect) {
    if (!drawbm || !item || !m_modernRenderingEnabled) {
        // Fallback para renderização padrão
        RegionPlaylistView::OnItemPaint(drawbm, item, itemRect);
        return;
    }

    try {
        RgnPlaylistItem* plItem = static_cast<RgnPlaylistItem*>(item);
        if (!plItem || !plItem->IsValidIem()) {
            return; // Item inválido, não renderizar
        }

        ItemRenderData data = GetItemData(plItem);
        ItemVisualState state = GetItemState(item, GetItemIndex(item));

        m_renderer.DrawItem(drawbm, itemRect, data, state, m_theme);
    }
    catch (...) {
        // Em caso de erro, usar renderização padrão
        RegionPlaylistView::OnItemPaint(drawbm, item, itemRect);
    }
}
```

## Testing Strategy

### Unit Testing Approach

Dado que o projeto usa C++ sem framework de testes aparente, a estratégia será:

1. **Manual Testing**
   - Criar playlists de teste com diferentes configurações
   - Testar todos os estados visuais (playing, next, hover, selected)
   - Testar com temas dark e light
   - Testar redimensionamento da janela

2. **Visual Regression Testing**
   - Capturar screenshots de estados conhecidos
   - Comparar visualmente após mudanças
   - Documentar casos de teste visuais

3. **Performance Testing**
   - Criar playlist com 100+ itens
   - Medir FPS durante scroll
   - Medir tempo de atualização durante playback
   - Usar ferramentas de profiling do Visual Studio/Xcode

4. **Integration Testing**
   - Testar integração com REAPER
   - Verificar que ações existentes continuam funcionando
   - Testar salvamento/carregamento de estado
   - Testar com diferentes versões do REAPER

### Test Scenarios

#### Scenario 1: Renderização Básica
- Criar playlist com 10 regiões
- Verificar que todas são exibidas corretamente
- Verificar espaçamento entre itens
- Verificar tamanhos de fonte

#### Scenario 2: Estados de Playback
- Iniciar playback
- Verificar highlight da região atual
- Verificar indicador da próxima região
- Verificar transição entre regiões

#### Scenario 3: Interação do Usuário
- Hover sobre itens
- Click em itens
- Drag and drop de itens
- Verificar feedback visual

#### Scenario 4: Modo de Monitoramento
- Alternar para modo de monitoramento
- Verificar tamanhos de fonte grandes
- Verificar barra de progresso
- Verificar atualização em tempo real

#### Scenario 5: Performance
- Criar playlist com 200 itens
- Scroll rápido pela lista
- Verificar FPS > 30
- Verificar sem lag visual

#### Scenario 6: Edge Cases
- Playlist vazia
- Região com nome muito longo
- Região com duração muito curta
- Loop infinito
- Sync loss

### Performance Benchmarks

Targets de performance:

| Operação | Target | Medição |
|----------|--------|---------|
| Item render | < 5ms | Tempo para desenhar um item |
| Full list render | < 100ms | Tempo para desenhar lista completa |
| Scroll FPS | > 30 FPS | Frame rate durante scroll |
| Playback update | < 100ms | Tempo para atualizar item atual |
| Theme switch | < 200ms | Tempo para trocar tema |
| Window resize | < 200ms | Tempo para reflow após resize |

## Implementation Notes

### Compatibilidade

- Manter compatibilidade com código existente
- Não quebrar API pública de `RegionPlaylistView`
- Permitir desabilitar renderização moderna via flag
- Suportar Windows, macOS e Linux (via SWELL)

### Otimizações

1. **Caching**
   - Cachear ícones renderizados
   - Cachear fontes carregadas
   - Cachear cores calculadas

2. **Dirty Regions**
   - Apenas redesenhar itens que mudaram
   - Usar dirty flags para controlar atualizações

3. **Lazy Loading**
   - Carregar ícones apenas quando necessário
   - Carregar fontes apenas quando necessário

4. **Double Buffering**
   - Usar LICE_IBitmap para double buffering
   - Evitar flickering durante atualizações

### Platform-Specific Considerations

#### Windows
- Usar GDI+ para renderização suave
- Suportar High DPI displays
- Testar com ClearType

#### macOS
- Usar Core Graphics via SWELL
- Suportar Retina displays
- Testar com diferentes escalas

#### Linux
- Usar Cairo via SWELL
- Testar com diferentes window managers
- Verificar renderização de fontes

### Migration Path

Implementação em fases:

**Phase 1: Foundation**
- Criar classes base (PlaylistTheme, ModernPlaylistItemRenderer)
- Implementar sistema de ícones
- Adicionar flag para habilitar/desabilitar

**Phase 2: List View**
- Implementar ModernRegionPlaylistView
- Adicionar renderização customizada de itens
- Implementar hover effects

**Phase 3: Monitoring**
- Implementar ModernMonitoringView
- Adicionar barra de progresso
- Melhorar contraste e fontes

**Phase 4: Polish**
- Ajustar cores e espaçamentos
- Otimizar performance
- Adicionar animações suaves (opcional)

**Phase 5: Testing & Refinement**
- Testes extensivos
- Correções de bugs
- Ajustes baseados em feedback

## Diagrams

### Sequence Diagram: Item Rendering

```
User                RegionPlaylistWnd    ModernRegionPlaylistView    ModernPlaylistItemRenderer    PlaylistTheme
 |                         |                        |                            |                      |
 |-- Scroll List --------->|                        |                            |                      |
 |                         |-- Update() ----------->|                            |                      |
 |                         |                        |-- GetItemData() -------->  |                      |
 |                         |                        |<- ItemData --------------|  |                      |
 |                         |                        |-- GetItemState() ------->  |                      |
 |                         |                        |<- ItemVisualState -------|  |                      |
 |                         |                        |-- DrawItem() ---------------------------------->  |
 |                         |                        |                            |-- GetColors() ------>|
 |                         |                        |                            |<- Colors ------------|
 |                         |                        |                            |-- GetFonts() ------->|
 |                         |                        |                            |<- Fonts -------------|
 |                         |                        |                            |-- DrawBackground() ->|
 |                         |                        |                            |-- DrawStatusIcon() ->|
 |                         |                        |                            |-- DrawRegionNumber()->|
 |                         |                        |                            |-- DrawRegionName() ->|
 |                         |                        |                            |-- DrawTimeInfo() --->|
 |                         |                        |<- Rendered Item ----------------------------------|
 |<- Visual Update --------|                        |                            |                      |
```

### State Diagram: Item Visual States

```
                    ┌─────────────┐
                    │   Normal    │
                    └──────┬──────┘
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
        ▼                  ▼                  ▼
   ┌─────────┐       ┌─────────┐       ┌─────────┐
   │ Hovered │       │Selected │       │ Playing │
   └─────────┘       └─────────┘       └────┬────┘
                                             │
                                    ┌────────┴────────┐
                                    │                 │
                                    ▼                 ▼
                              ┌──────────┐      ┌─────────┐
                              │   Next   │      │SyncLoss │
                              └──────────┘      └─────────┘
```

## Dependencies

### External Dependencies
- LICE (já presente no WDL)
- WDL (já presente)
- REAPER API (já presente)

### Internal Dependencies
- `SnM/SnM_RegionPlaylist.h/cpp` (modificar)
- `SnM/SnM_VWnd.h/cpp` (usar)
- `SnM/SnM_Dlg.h/cpp` (usar para temas)
- `sws_util.h/cpp` (usar)

### New Files to Create
- `SnM/SnM_ModernPlaylistUI.h`
- `SnM/SnM_ModernPlaylistUI.cpp`
- `SnM/SnM_PlaylistTheme.h`
- `SnM/SnM_PlaylistTheme.cpp`
- `SnM/SnM_PlaylistIcons.h`
- `SnM/SnM_PlaylistIcons.cpp`

## Configuration

### User Preferences

Adicionar preferências para customização:

```cpp
// Preferências armazenadas no reaper.ini
struct PlaylistUIPreferences {
    bool enableModernUI;           // Habilitar UI moderna
    bool enableAnimations;         // Habilitar animações
    int itemHeight;                // Altura dos itens (pixels)
    int fontSize;                  // Tamanho base da fonte
    bool autoScrollToCurrent;      // Auto-scroll para item atual
    bool showTimeRemaining;        // Mostrar tempo restante
    bool showProgressBar;          // Mostrar barra de progresso
    int colorScheme;               // 0=auto, 1=dark, 2=light
};
```

### Theme Customization

Permitir customização de cores via arquivo INI:

```ini
[sws_playlist_theme_dark]
background=0x1E1E1E
text=0xE0E0E0
current_bg=0x2D5F8D
current_text=0xFFFFFF
next_bg=0x3E3E42
next_text=0xE0E0E0

[sws_playlist_theme_light]
background=0xFFFFFF
text=0x1E1E1E
current_bg=0x0078D7
current_text=0xFFFFFF
next_bg=0xE0E0E0
next_text=0x1E1E1E
```

## Conclusion

Este design mantém a arquitetura existente do SWS enquanto adiciona uma camada moderna de visualização. A abordagem modular permite implementação incremental e fácil manutenção. O foco em performance e compatibilidade garante que a solução seja adequada para uso em shows ao vivo sem comprometer a estabilidade do plugin.
