# 4. Shared pimpl library, application and tests as separate parts

- Status: accepted
- Date: 2026-10-31

## Context

The project should be a reusable library, an application that uses it, and tests with their own test data. The library will be deployed as a shared library (see [0001](0001-dynamic-linking-via-vcpkg-triplets.md)), so its ABI matters: an application built against one release should keep working with the next compatible `.so`.

## Decision

**Layout.** One folder and one `CMakeLists.txt` per part:

| Folder | Target | Contents |
|---|---|---|
| `lib/` | `affine_stitcher` (`AffineStitcher::affine_stitcher`), shared | public headers in `lib/include/`, sources and private headers in `lib/src/` |
| `app/` | `affine_stitcher_app` (binary `affine_stitcher`) and `affine_stitcher_app_core` (static) | `main.cpp`; command line, TOML config and logging setup |
| `tests/` | three test executables, see below | `public/`, `internal/`, `app/`, `support/`, `abi/`, `data/` |

**Pimpl.** Every public class keeps its data in a `struct Impl` behind `std::unique_ptr<Impl>`, with all special members defined in the `.cpp`. That includes `StitcherOptions`, so new settings don't change any class layout. `Stitcher` is move-only; `StitcherOptions` is copyable (deep copy) and movable. Moved-from objects may only be assigned to or destroyed.

**Visibility.** The library is compiled with `-fvisibility=hidden` and `-fvisibility-inlines-hidden`. Only classes marked `AFFINE_STITCHER_EXPORT` are exported. The macro comes from `export.hpp`, which CMake's `GenerateExportHeader` writes to `build/<preset>/lib/generated/`. Internal code lives in `affine_stitcher::detail`. The test `abi.exported_symbols` checks the exports with `nm`: public API present, `detail::` absent.

**SONAME.** `libaffine_stitcher.so.<major>.<minor>`, currently `.so.0.1`. While the version is 0.x every minor release may break the ABI. From 1.0 on, the SONAME should become `.so.<major>`.

**Public dependencies.** OpenCV is public, because the API uses `cv::Mat`. spdlog is private: the library logs through spdlog's default logger, which the application installs. That works because spdlog is a shared library, so library and app share its logger registry. toml++ and CLI11 belong to the application only.

**Tests.** Three executables, each with a CTest name prefix:

- `public.` links the shared library like any user would, and only sees exported symbols.
- `internal.` links the object library `affine_stitcher_objects` (the same object files the `.so` is made of), so it can test `detail::` code.
- `app.` links `affine_stitcher_app_core`.

**Test data.** Small images (at most 500 KB each) are committed to `tests/data/<scene>/` in plain git. The rules are in `tests/data/README.md`, and a test enforces them.

## Consequences

- Adding a setting means a private field in `StitcherOptions::Impl` plus a getter and setter. The layout doesn't change, but each new exported function is new ABI and must not be removed again within the same SONAME.
- Removing or changing an exported function, or the meaning of a setting, needs a new SONAME, which means a minor version bump while on 0.x.
- Public headers must not include private headers or third-party headers other than OpenCV.
- The `affine_stitcher` namespace comment (needed by Doxygen, see [0002](0002-documentation-doxygen-and-markdown.md)) now lives in `stitcher.hpp`.
- An internal function tested in `internal.` is compiled once and linked twice (into the `.so` and into the internal tests). That's intended: both use exactly the same objects.
