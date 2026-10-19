# T01 status: blocked / incomplete

Date: 2026-10-19 (America/Denver).

## Input and guidance

The requested /mnt/data paths do not exist in this Windows environment.
Source was found at C:/Users/durps/Downloads/Ingnomia-master.zip and extracted
without overwriting an existing repository.

Source ZIP SHA256:
1D8069365B5537BD3E8939FFDCCAC56C5114045181986FC8B65454AB1CEC7215

The user located the pack at Desktop/Gnomoria Remake/ingnomia_codex_pack.
All three files were read and copied into the repository root unchanged.
AGENTS.md is an additional byte-identical copy of CODEX_MASTER_INSTRUCTIONS.md.
The exact T01 acceptance criteria require configure, compile and launch; those
criteria have not been met. Only T01 was investigated.

## Build attempt

From the task workspace:

```powershell
cmake -S outputs/Ingnomia-master -B work/build -G 'Visual Studio 17 2022' -A x64
```

PowerShell returned: "The term 'cmake' is not recognized as a name of a cmdlet,
function, script file, or executable program."

Configure did not start. No compiler, linker, install or runtime result exists.
No compiler warnings can be reported because compilation was not reached.

Toolchain inventory:

- Get-Command found no cmake, ninja, cl or qmake on PATH.
- Visual Studio Installer's vswhere -all -products '*' -format json returned [].
- Standard C:/Qt and Program Files (x86)/Windows Kits/10 locations are absent.
- Matching Steamworks, NoesisGUI and OpenAL development SDKs were not supplied.
- The source archive lacks content/tilesheet and content/audio.

This is an environment/input blocker, not evidence of a C++ compile failure.
No machine-wide tools or SDKs were installed during this attempt.

## Changes and remaining work

- README.md: replace stale Windows/Qt5/SFML setup with a link to source-derived steps.
- BUILDING.md: prerequisites, SDK layouts, commands and deployment checks.
- docs/T01_STATUS.md: input identity, attempt, limitations and pending work.
- CODEX_MASTER_INSTRUCTIONS.md, CODEX_TICKETS.md, START_HERE.md: original pack.
- AGENTS.md: copy of the original master instructions as repository guidance.

All original CMake and C++ files remain unchanged. No functionality was removed.
The extracted repository has no .git history; no commit was made.
SHA256 comparison against all 673 original archive files found only README.md
modified and no original files missing. AGENTS.md and the master instructions
have matching SHA256 hashes. No automated build tests ran because configure
could not start.

To finish T01: provision the toolchain and SDKs, record exact dependency
versions/hashes, configure, compile,
install and verify deployment. Investigate the windeployqt lookup against the
installed Qt kit. CI's mutable dependency URLs and unpinned CMake remain obstacles
to a fully reproducible build. Make only fixes supported by those build results.
Do not advance to T02 until T01 is reviewed and accepted.
