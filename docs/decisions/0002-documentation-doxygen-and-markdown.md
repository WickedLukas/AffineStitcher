# 2. Doxygen for the API, Markdown for design docs

- Status: accepted
- Date: 2026-10-31

## Context

The project needs documentation that grows with the code. There are two kinds: reference documentation for the public API, and design documentation (architecture, math conventions, reasons behind decisions) that doesn't belong in code comments.

## Decision

- The API is documented with `///` comments in `include/affine_stitcher/`. The `docs` CMake target runs Doxygen over the headers, `README.md` and `docs/`, writing HTML to `build/<preset>/docs/html/`. The target exists only when Doxygen is installed and is not part of `ALL`.
- Design docs are plain Markdown in `docs/`: `architecture.md`, `math.md`, and numbered decision records in `docs/decisions/`. They render on GitHub without a build step, and Doxygen includes them as pages.
- `WARN_IF_UNDOCUMENTED` together with `WARN_AS_ERROR=FAIL_ON_WARNINGS` makes the `docs` target fail when public API is missing its comment.
- Doxygen and Graphviz come from Debian packages in the dev container image (Doxygen 1.9.4 on Bookworm).
- Sphinx and MkDocs were not chosen, because they need Python packages and a separate site, which only pays off once the project has outside users.

## Consequences

- The `affine_stitcher` namespace must keep its `///` comment, which lives in `affine.hpp`. Without it, Doxygen 1.9.4 silently leaves out all namespace members and gives no warning.
- `USE_MDFILE_AS_MAINPAGE` is set as an absolute path. Doxygen 1.9.4 otherwise also treats `docs/decisions/README.md` as a main page and fails.
- The undocumented-API check only runs when someone builds the `docs` target. The normal build and tests don't run it.
