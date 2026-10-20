#!/bin/bash

# Aggiunge i percorsi standard di Homebrew a PATH nel caso si committi da CLion/GUI
export PATH="/opt/homebrew/opt/llvm/bin:/opt/homebrew/bin:$PATH"

# Verifica se clang-format è installato
if ! command -v clang-format &> /dev/null; then
    echo "Attenzione: clang-format non trovato nel PATH. Commit continuato senza formattazione."
    exit 0
fi

# Prende solo i file C/C++ aggiunti o modificati nello stage (git add)
FILES=$(git diff --cached --name-only --diff-filter=ACMR | grep -E '\.(cpp|hpp|cc|cxx|h)$')

if [ -n "$FILES" ]; then
    echo "Esecuzione clang-format sui file in staging..."
    for FILE in $FILES; do
        if [ -f "$FILE" ]; then
            clang-format -i "$FILE"
            git add "$FILE"
        fi
    done
fi

exit 0