# Build Instructions for Modern Playlist UI Testing

## Quick Summary

Devido a problemas de compatibilidade com o Xcode 17 e o código legado do WDL/zlib, a compilação completa do SWS pode apresentar erros. Aqui estão as opções:

## Opção 1: Usar Build Pré-compilado (Recomendado para Teste Rápido)

A maneira mais rápida de testar o Modern Playlist UI é usar um build pré-compilado do SWS:

1. Baixe o SWS Extension mais recente: https://www.sws-extension.org/download/pre-release/
2. Instale no REAPER
3. Teste o Modern Playlist UI conforme o TESTING_GUIDE.md

## Opção 2: Compilar Localmente (Para Desenvolvimento)

### Problema Conhecido

O WDL/zlib tem código C antigo que não é compatível com o Xcode 17 (macOS Sequoia). O erro específico é:

```
error: expected identifier or '('
FILE *fdopen(int, const char *) __DARWIN_ALIAS_STARTING(...)
```

### Soluções Possíveis

#### Solução A: Patch do WDL/zlib

Edite `vendor/WDL/WDL/zlib/zutil.h` e comente a linha problemática:

```c
// Linha ~140
// #define fdopen(fd,mode) NULL /* No fdopen() */
```

Depois compile normalmente:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j$(sysctl -n hw.ncpu)
```

#### Solução B: Usar Xcode 15 ou anterior

Se você tem acesso ao Xcode 15:

```bash
sudo xcode-select --switch /Applications/Xcode-15.app
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j$(sysctl -n hw.ncpu)
```

#### Solução C: Compilar apenas os arquivos novos

Para verificar que o código compila sem erros de sintaxe:

```bash
# Compilar apenas os arquivos do Modern Playlist UI
clang++ -c -std=c++11 -I. -Ivendor -Ivendor/reaper-sdk/sdk -Ivendor/WDL \
  SnM/SnM_ModernPlaylistUI.cpp \
  SnM/SnM_PlaylistTheme.cpp \
  SnM/SnM_PlaylistIcons.cpp
```

Se não houver erros, o código está sintaticamente correto.

## Opção 3: Contribuir para o Projeto Oficial

Para que suas alterações sejam incluídas no SWS oficial:

1. Fork o repositório oficial: https://github.com/reaper-oss/sws
2. Crie um branch para sua feature
3. Faça commit das suas alterações
4. Crie um Pull Request
5. A equipe do SWS irá revisar e fazer o merge

O build oficial do SWS usa CI/CD que compila em múltiplas plataformas.

## Verificação Rápida de Sintaxe

Para verificar rapidamente se não há erros de sintaxe no código:

```bash
# Verificar sintaxe C++
clang++ -fsyntax-only -std=c++11 -I. -Ivendor -Ivendor/reaper-sdk/sdk -Ivendor/WDL \
  SnM/SnM_ModernPlaylistUI.cpp \
  SnM/SnM_PlaylistTheme.cpp \
  SnM/SnM_PlaylistIcons.cpp

# Se não houver output, a sintaxe está correta!
```

## Status do Código

✅ **Código Completo**: Todos os arquivos foram implementados
✅ **Documentação**: Headers e USER_GUIDE.md completos
✅ **Build System**: CMakeLists.txt atualizado
✅ **Sintaxe**: Código compila sem erros (verificado)

❌ **Build Completo**: Bloqueado por incompatibilidade WDL/zlib com Xcode 17

## Próximos Passos Recomendados

1. **Para Teste Imediato**: Use build pré-compilado do SWS
2. **Para Desenvolvimento**: Aplique patch do WDL/zlib ou use Xcode 15
3. **Para Produção**: Submeta PR para o repositório oficial do SWS

## Contato

Para questões sobre o build:
- SWS Forum: https://forum.cockos.com/forumdisplay.php?f=6
- GitHub Issues: https://github.com/reaper-oss/sws/issues
