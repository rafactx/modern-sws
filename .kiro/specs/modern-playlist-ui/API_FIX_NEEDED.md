# API Fix Needed - LICE Font Functions

## Problema

O código está usando funções `LICE_DrawText()` e `LICE_MeasureText()` que não existem na API do LICE.

A API correta do LICE usa métodos do objeto `LICE_CachedFont`:

```cpp
// ERRADO (não existe):
LICE_DrawText(bm, x, y, text, color, alpha, mode, font);
LICE_MeasureText(text, &w, &h, font);

// CORRETO:
RECT r = {x, y, x+1000, y+1000};
font->SetTextColor(color);
font->DrawText(bm, text, -1, &r, DT_SINGLELINE|DT_NOPREFIX);

// Para medir:
RECT r = {0, 0, 0, 0};
font->DrawText(NULL, text, -1, &r, DT_SINGLELINE|DT_NOPREFIX|DT_CALCRECT);
// Agora r.right e r.bottom contêm as dimensões
```

## Solução Rápida

### Opção 1: Criar Funções Helper (Recomendado)

Adicione estas funções helper no início de `SnM_ModernPlaylistUI.cpp`:

```cpp
// Helper functions for LICE font rendering
static void DrawTextWithFont(LICE_IBitmap* bm, int x, int y, const char* text,
                             LICE_pixel color, LICE_CachedFont* font)
{
    if (!bm || !text || !font) return;

    RECT r = {x, y, x + 10000, y + 10000};
    font->SetTextColor(color);
    font->DrawText(bm, text, -1, &r, DT_SINGLELINE|DT_NOPREFIX);
}

static void MeasureTextWithFont(const char* text, int* w, int* h, LICE_CachedFont* font)
{
    if (!text || !w || !h || !font) return;

    RECT r = {0, 0, 0, 0};
    font->DrawText(NULL, text, -1, &r, DT_SINGLELINE|DT_NOPREFIX|DT_CALCRECT);
    *w = r.right;
    *h = r.bottom;
}
```

Depois, faça busca e substituição:

```bash
# Substituir LICE_DrawText
LICE_DrawText(drawbm, currentX, centerY, numStr, colors.text, 1.0f, LICE_BLIT_MODE_COPY, fonts.itemNumber);
# Por:
DrawTextWithFont(drawbm, currentX, centerY, numStr, colors.text, fonts.itemNumber);

# Substituir LICE_MeasureText
LICE_MeasureText(data.regionName.Get(), &textRect.right, &textRect.bottom, fonts.itemName);
# Por:
MeasureTextWithFont(data.regionName.Get(), &textRect.right, &textRect.bottom, fonts.itemName);
```

### Opção 2: Usar API Direta

Substitua cada chamada manualmente seguindo o padrão acima.

## Arquivos Afetados

- `SnM/SnM_ModernPlaylistUI.cpp` - ~20 ocorrências

## Comando para Encontrar Todas as Ocorrências

```bash
grep -n "LICE_DrawText\|LICE_MeasureText" SnM/SnM_ModernPlaylistUI.cpp
```

## Após Corrigir

```bash
# Limpar e recompilar
rm -rf build && mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j$(sysctl -n hw.ncpu)
```

## Nota

Este é um erro comum ao portar código que usa diferentes APIs de renderização de texto.
A API do LICE é baseada em Windows GDI/DrawText, que usa RECTs e flags DT_*.
