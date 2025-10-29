#!/usr/bin/env python3
"""
Script para corrigir todas as chamadas LICE_DrawText e LICE_MeasureText
"""

import re

def fix_lice_calls(filename):
    with open(filename, 'r') as f:
        content = f.read()

    # Backup
    with open(filename + '.backup', 'w') as f:
        f.write(content)

    # Fix LICE_DrawText calls (8 params -> 6 params)
    # Pattern: LICE_DrawText(bm, x, y, text, color, alpha, mode, font)
    # Replace: DrawTextWithFont(bm, x, y, text, color, font)
    pattern1 = r'LICE_DrawText\(([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*[^,]+,\s*[^,]+,\s*([^)]+)\)'
    replacement1 = r'DrawTextWithFont(\1, \2, \3, \4, \5, \6)'
    content = re.sub(pattern1, replacement1, content)

    # Fix LICE_MeasureText calls
    # Pattern: LICE_MeasureText(text, &rect.right, &rect.bottom, font)
    # Replace: MeasureTextWithFont(text, &width, &height, font)
    # This is more complex because we need to handle RECT vs int* differently

    # For now, let's just replace the function name and let the compiler catch issues
    content = content.replace('LICE_MeasureText(', 'MeasureTextWithFont(')

    # Write back
    with open(filename, 'w') as f:
        f.write(content)

    print(f"Fixed {filename}")
    print(f"Backup saved to {filename}.backup")

if __name__ == '__main__':
    fix_lice_calls('SnM/SnM_ModernPlaylistUI.cpp')
