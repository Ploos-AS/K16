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
