# M5.55 — Independent W65C816 differential qualification

K16 uses the **SingleStepTests/ProcessorTests 65816** corpus as the independent
instruction-level oracle. The corpus contains JSON single-step vectors with
initial/final CPU state, memory effects and bus-cycle observations and is kept
outside this repository because of its size.

## Rules

- Do not copy an emulator implementation into K16.
- Treat the external corpus as test data only.
- Record the exact ProcessorTests commit used for every qualification run.
- Compare architectural state first: A, X, Y, S, D, DBR, PBR, PC, P/E and
  touched memory.
- Bus-cycle comparison is a separate timing gate; semantic mismatches must be
  fixed before timing differences are waived.
- Run both emulation and native-mode vectors, including M/X width combinations.

## Bootstrap

Run `tools/fetch-processor-tests.sh`. Set `PROCESSOR_TESTS_REV` to a commit
SHA for reproducible qualification. The default `main` is for exploration
only and is not sufficient for a release qualification.

## M5.55 exit gate

M5.55 is complete only when a K16 adapter can consume the corpus, execute the
selected vectors, emit deterministic mismatch diagnostics, and a pinned full
semantic run passes. Until then ROADMAP must remain unchecked.

## First pinned CI smoke

GitHub Actions **M5.55 Differential Smoke #9** passed against ProcessorTests revision `bb11756436da8fd16cce86aef63dc6725f48836f`.

The smoke executes the first 10 NOP (`EA`) vectors in both emulation and native mode. The first real corpus run exposed an adapter-state issue: ProcessorTests can supply a non-`01xx` initial S value while emulation mode constrains the effective stack pointer to page 1. The adapter now normalizes initial S to `0x0100 | (S & 0x00ff)` when E=1. With that representation fix, both pinned NOP smoke sets pass.

This is an infrastructure/semantic smoke milestone only; it does **not** complete M5.55. Broader opcode/mode coverage and the full pinned semantic corpus remain required. Bus-cycle traces remain a separate timing qualification gate.
