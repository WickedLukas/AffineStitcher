#!/usr/bin/env bash
# PostToolUse hook: run clang-format on C++ files Claude has just written or edited.
set -euo pipefail

file=$(jq -r '.tool_input.file_path // empty')
case "$file" in
    *.cpp | *.cc | *.cxx | *.hpp | *.hh | *.hxx | *.h | *.ipp) ;;
    *) exit 0 ;;
esac

if [[ -f "$file" ]] && command -v clang-format >/dev/null; then
    clang-format -i --style=file "$file"
fi
exit 0
