# Windows build (T01, not yet validated)

This checklist is derived from this archive's CMake and source files. It is not
a successful-build record. See `docs/T01_STATUS.md` for the attempted build and blockers.

## Prerequisites

Use an x64 MSVC toolchain throughout. The existing `.github/workflows/cmake.yml`
names the following dependency versions; these are the intended baseline, not
versions validated by this T01 attempt:

| Dependency | Baseline / required layout | Evidence |
| --- | --- | --- |
| Visual Studio 2022 | Desktop development with C++, MSVC v143, Windows SDK; MSBuild | CI uses windows-2022 and Qt's MSVC2022 kit; CMake requests C++23 |
| CMake | Source minimum 3.16; record the exact installed version before building | Root CMakeLists.txt; CI currently uses an unpinned latest CMake |
| Qt | 6.9.0, win64_msvc2022_64, including qtimageformats | CI; required CMake components Core, Xml, Sql, Gui, Widgets |
| Steamworks | SDK 1.64, root containing public/steam/steam_api.h and redistributable_bin/win64/steam_api64.lib plus steam_api64.dll | CI and FindSteam.cmake |
| NoesisGUI Native SDK | 3.0.12, full SDK with Include/NoesisPCH.h, Lib/windows_x86_64/Noesis.lib, Bin/windows_x86_64/Noesis.dll, Src/Packages and Src/Tools/Bin2h/bin2h.exe | CI, FindNoesis.cmake, 3rdparty/NoesisApp/CMakeLists.txt |
| Noesis license | Valid developer license name and key | Root CMake generates src/gui/license.h |
| OpenAL Soft | 1.25.1, matching x64 headers, import library and OpenAL32.dll | CI and FindOpenAL.cmake |
| OpenGL | Development libraries through Windows SDK; driver/context required for execution | Root CMake and renderer |

GLAD, exprtk and FastNoise are included in the archive. SFML and Qt5 are not
linked by the current build. The `.gitmodules` beehive entry points to an absent
directory; no source include of that submodule was found, so it has not been
classified as a required dependency for this snapshot.

The full Noesis application framework sources are consumed from the external SDK;
the binary runtime alone is insufficient. BugSplat is optional: leave BUGSPLAT_DB
empty for this baseline, matching CI. Its C++23 compatibility remains untested.

Obtain SDKs through their authorized distribution channels. The CI names archives
in rschurade/IngnomiaBuildDeps on GitHub, but uses a mutable master branch and no
checksums. Archive availability and contents have not been verified here. Record
download provenance and SHA256 for each dependency before claiming reproducibility.

## Configure, build and install

Open PowerShell at the extracted repository root after installing prerequisites.
Replace the example SDK paths with the directories described above. Put CMake on
PATH. Obtain license values through your local environment; do not commit them or
publish CMakeCache.txt or the generated license header.

```powershell
cmake --version
$qt = 'C:/Qt/6.9.0/msvc2022_64'
$steam = 'C:/IngnomiaDeps/steamworks_sdk/sdk'
$noesis = 'C:/IngnomiaDeps/noesis_sdk'
$openal = 'C:/IngnomiaDeps/openal_sdk'

cmake -S . -B build/windows -G 'Visual Studio 17 2022' -A x64 `
  "-DCMAKE_PREFIX_PATH=$qt" `
  "-DSTEAM_SDK_ROOT=$steam" `
  "-DNOESIS_ROOT=$noesis" `
  "-DOPENAL_ROOT=$openal" `
  "-DNOESIS_LICENSE_NAME=$env:NOESIS_LICENSE_NAME" `
  "-DNOESIS_LICENSE_KEY=$env:NOESIS_LICENSE_KEY" `
  '-DBUGSPLAT_DB=' '-DCMAKE_INSTALL_BINDIR=.'
if ($LASTEXITCODE -ne 0) { throw 'Configure failed' }

cmake --build build/windows --config RelWithDebInfo --target Ingnomia
if ($LASTEXITCODE -ne 0) { throw 'Build failed' }

cmake --install build/windows --config RelWithDebInfo --prefix install/windows
if ($LASTEXITCODE -ne 0) { throw 'Install failed' }
```

Use --config with the Visual Studio multi-configuration generator; CMAKE_BUILD_TYPE
does not select its build configuration. Record MSVC and Windows SDK versions from
the configure output/project settings. Exact MSVC, SDK and CMake versions remain
to be pinned after a successful build.

The OpenAL finder accepts OPENAL_ROOT and searches headers under include/AL,
libraries under lib64, lib, libs64, libs or libs/Win64, and the DLL under those
directories or bin/Win64. For another SDK layout, set OPENAL_INCLUDE_DIR,
OPENAL_LIBRARY and OPENAL_DLL explicitly to the x64 files.

## Deployment checks still required for T01

Provide content/tilesheet and content/audio from an authorized asset distribution
before configuring; neither directory is included in this source archive. CMake
stages content beside the executable and installs recognized content file types.
Reconfigure after adding assets so the install file list includes them.

Check build/windows/RelWithDebInfo and install/windows for Ingnomia.exe, content,
Steam/Noesis/OpenAL DLLs, matching Qt DLLs, platforms/qwindows.dll and
sqldrivers/qsqlite.dll (source uses QSQLITE). Check the required image format
plugins from the matching Qt kit too.

The current deployment helper searches for windeployqt6.exe and silently skips
deployment if no Qt6::windeployqt target is available. Inspect the actual Qt kit's
tool filename before relying on automatic deployment. If necessary, invoke that
kit's windeployqt.exe manually with --release --dir install/windows against
install/windows/Ingnomia.exe, then inspect the resulting files. This is a pending
validation item, not a verified workaround in this environment.

No game launch, gameplay smoke testing or T02 work was performed in that baseline.

## PLAY-01D: Noesis-free lifecycle regression

The standalone `prototype/rmlui` project (CMake 3.25 or newer) links the original GameManager,
EventConnector, all 15 aggregators and their original simulation dependencies
into `knomira_engine_lifecycle_test`. It requires Qt Core/Gui/Widgets/Xml/Sql,
GLFW, FreeType and OpenAL Soft (vcpkg package `openal-soft:x64-windows`).
No Steam or Noesis SDK, tilesheets, audio assets, or graphics context are needed
for this startup/shutdown test. FastNoise and exprtk use the bundled sources.
The test engine has a small Qt-only PCH; the existing source compile gate still
checks its selected original sources without any PCH.

```powershell
cmake -S prototype/rmlui -B build/lifecycle -G 'Visual Studio 17 2022' -A x64 `
  "-DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
  "-DCMAKE_PREFIX_PATH=$env:QT_ROOT_DIR" -DKNOMIRA_BUILD_GAME_SHELL=ON
cmake --build build/lifecycle --config Debug --target knomira_engine_lifecycle_test --parallel 4
# Make matching Qt and OpenAL DLLs/plugins discoverable in this process.
$env:PATH = "$env:QT_ROOT_DIR/bin;$env:VCPKG_ROOT/installed/x64-windows/debug/bin;$env:PATH"
$env:QT_PLUGIN_PATH = "$env:QT_ROOT_DIR/plugins"
ctest --test-dir build/lifecycle -C Debug -R '^knomira_engine_lifecycle_test$' --output-on-failure
```

The executable creates and marks its own QTemporaryDir before constructing
Config or any engine object, and verifies `KNOMIRA_TEST_DATA_ROOT` with the
existing data-root safeguard. Only test objects have
`KNOMIRA_ISOLATED_TEST_DATA_ROOT`; the production save path and format are
unchanged. OpenAL Soft uses its real null output backend. Three cycles verify
the original objects, parent/worker-thread affinity, queued menu/no-save/exit
replies, deletion of every QObject, cleared globals, and `quit`/`wait` shutdown.
No test visits `Documents/My Games/Ingnomia`.

Build the other test targets (or the standalone default target) and run
`ctest --test-dir build/lifecycle -C Debug --output-on-failure` for the registered
Config, DB rollback/job-cache, two-material embark, data-root, save-catalog,
input, layout and lifecycle regressions. This proves engine startup and teardown;
it does not prove a playable colony or a connected RmlUi world view.

## PLAY-01E: asset-dependent colony roundtrip

`knomira_colony_lifecycle_test` reuses the guarded engine from PLAY-01D. It
generates a 64 x 64 x 130 world with seed 120531 and one settler, advances real
timer ticks with heartbeat acknowledgments, verifies pause, creates a pending
tree-felling job through EventConnector, saves and reloads the colony, resumes
ticks, lets the settler complete the restored order and produce wood, and saves
a second slot. It checks population/inventory/clock, pending
job identity, squad order, deleted session objects and SHA256 preservation of
the first save. The squad test covers the New Game crash caused by treating a
squad's entity ID as a QList insertion index.
The settings seed and initial C RNG seed are fixed. Some original subsystems
still reseed internally from the clock, so this test checks within-run roundtrip
invariants; population details can vary across runs. It does not establish
fully deterministic world generation.

```powershell
cmake --build build/lifecycle --config Debug --target knomira_colony_lifecycle_test --parallel 4
# Existing, authorized, read-only asset source; must contain tilesheet/*.png.
$env:KNOMIRA_TEST_CONTENT_DIR = 'C:/YourAuthorizedGameContent/content'
ctest --test-dir build/lifecycle -C Debug -R '^knomira_colony_lifecycle_test$' --output-on-failure
```

Qt/OpenAL PATH and QT_PLUGIN_PATH setup is the same as above. The test chooses
Qt's offscreen platform and OpenAL Soft's null output. It reads the supplied
tilesheets without copying or modifying them. Missing optional audio files
under the executable's content folder produce the original engine's warnings;
audible playback is not verified. To exercise buffer/source cleanup as well,
stage authorized audio under the test executable's `content/audio` directory.
No proprietary assets are added to Git or CI
artifacts. If the content variable is absent, CTest reports **Skipped** (exit
77); an explicitly supplied invalid or incomplete directory fails. Asset-free
CI can compile this target and verify the skip contract, but cannot claim a
colony runtime pass. All saves and settings remain in the marked temporary root.

This is a local simulation smoke test, not a full playtest: the RmlUi shell
still needs a session bridge and world renderer. Broader jobs, malformed-save
recovery, multi-season stability and legacy save compatibility remain separate
verification work.
