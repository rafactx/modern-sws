#!/bin/bash

# Script para corrigir todas as chamadas LICE_DrawText e LICE_MeasureText
# no arquivo SnM_ModernPlaylistUI.cpp

FILE="SnM/SnM_ModernPlaylistUI.cpp"

echo "Corrigindo chamadas LICE no arquivo $FILE..."

# Backup do arquivo original
cp "$FILE" "$FILE.backup"

# Substituir LICE_DrawText com 8 parâmetros por DrawTextWithFont com 6
sed -i '' 's/LICE_DrawText(\([^,]*\), \([^,]*\), \([^,]*\), \([^,]*\), \([^,]*\), [^,]*, [^,]*, \([^)]*\))/DrawTextWithFont(\1, \2, \3, \4, \5, \6)/g' "$FILE"

# Substituir LICE_MeasureText com 4 parâmetros por MeasureTextWithFont
# Primeiro, precisamos converter RECT para int*
# Isso é mais complexo, então vamos fazer manualmente as substituições restantes

echo "Substituições automáticas concluídas."
echo "Backup salvo em $FILE.backup"
echo ""
echo "ATENÇÃO: Algumas substituições precisam ser feitas manualmente:"
echo "1. Procure por 'LICE_MeasureText' restantes"
echo "2. Procure por 'LICE_DrawText' restantes"
echo "3. Substitua conforme o padrão nas funções helper"
echo ""
echo "Para verificar:"
echo "  grep -n 'LICE_DrawText\\|LICE_MeasureText' $FILE"
