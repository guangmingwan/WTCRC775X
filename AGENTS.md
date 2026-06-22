# Repository Instructions

## Project Shape
- This is a Keil MDK / uVision STM32F103RC firmware project, not a Make/CMake repo. The authoritative project file is `MDK-ARM/wtcrc103rct6.uvprojx` with target `wtcrc103rct6`, ARMCLANG V6.16, C99 enabled, defines `USE_HAL_DRIVER,STM32F103xE,SSD_1115_DW`.
- There is no checked-in CLI build/test script or CI workflow. Use Keil MDK/uVision to build unless the user provides a local toolchain command.
- Real runtime entrypoints are `Src/main.c` for MCU init and loop, `Src/tuner.c` for radio state/tuning/volume, `Src/ui.c` for keys, menus, IR remote, and voice feedback.

## Encoding
- All project `.c` and `.h` files are GBK encoded. Treat project C/H sources as GBK unless a file is explicitly known otherwise. Before editing Chinese text, verify with `iconv -f gbk -t utf-8 <file> > /dev/null`.
- Do not patch GBK Chinese text directly through UTF-8 tools. Convert a temporary copy GBK -> UTF-8, edit that, then convert back UTF-8 -> GBK.
- If a source file fails GBK conversion, stop before modifying that file and report it; mixed encoding has occurred in this repo before.

## Voice Feedback Rules
- This is a blind-accessible radio project: user-visible controls should have UART/TTS feedback through `printf`.
- Voice strings must end with `\n` and contain only GBK Chinese characters and/or English letters/digits. Avoid punctuation in spoken strings.
- Prefer existing helpers in `Src/ui.c`: `VoiceSay`, `VoiceSayValue`, `VoiceSayFrequency`, `VoiceSayVolume`, `VoiceSayBand`, `VoiceSayTuneType`, `VoiceSayStep`, `VoiceSayFilter`, `VoiceSayChannel`, `VoiceSayMenuItem`.

## Generated And Local Files
- Do not commit Keil user/session files or generated output unless explicitly requested. Common local noise includes `MDK-ARM/wtcrc103rct6.uvoptx`, `MDK-ARM/wtcrc103rct6/`, `MDK-ARM/DebugConfig/`, `MDK-ARM/RTE/`, and `*.uvguix.*`.
- Driver/CMSIS files may appear untracked after opening the project in Keil. Do not add them unless the task is specifically about project dependencies.

## Verification
- At minimum after source edits, run GBK validation on touched C/H files with `iconv -f gbk -t utf-8` and `git diff --check`.
- If changing Keil project membership or compiler settings, inspect `MDK-ARM/wtcrc103rct6.uvprojx`; do not rely on `uvoptx` for source-of-truth project configuration.
