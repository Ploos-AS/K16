#!/bin/sh
set -eu
# M5.55 differential oracle bootstrap.
# Tom Harte ProcessorTests provides independent, language-agnostic single-step
# JSON vectors. Keep the large corpus outside git; pin the revision in CI/local use.
DEST="${1:-.cache/ProcessorTests}"
REV="${PROCESSOR_TESTS_REV:-bb11756436da8fd16cce86aef63dc6725f48836f}"
if [ ! -d "$DEST/.git" ]; then
  git clone --filter=blob:none --no-checkout https://github.com/SingleStepTests/ProcessorTests.git "$DEST"
fi
git -C "$DEST" fetch --depth=1 origin "$REV"
git -C "$DEST" checkout --detach FETCH_HEAD
git -C "$DEST" sparse-checkout init --cone
git -C "$DEST" sparse-checkout set 65816
printf '%s\n' "ProcessorTests 65816 oracle ready at $DEST"
git -C "$DEST" rev-parse HEAD
