# BKBS_PSX
One song rhythm game made to learn C and PS1 development using PSY-Q and LibGS.

[![BKBS Playthrough Youtube Embed](https://img.youtube.com/vi/pl0-Lkwz1Hw/0.jpg)](https://www.youtube.com/watch?v=pl0-Lkwz1Hw)

Pre-built .bin and .cue files can be found at the [itch.io page](https://hardrockbluesky.itch.io/bambis-knees-bambis-shaking)

Video playthrough with some mild sync issues [here.](https://www.youtube.com/watch?v=pl0-Lkwz1Hw) Emulator and real hardware should be essentially flawless, assuming your emulator does CD emu properly!

### Build Environment
* Windows XP VM from [PSXDev.net](https://www.psxdev.net/help/virtual_machine.html) [ I replaced VPC 2007 with Hyper-V ]
* PSY-Q, Sony's SDK included with that image or [available here](https://www.psxdev.net/downloads.html) with setup help [here](https://www.psxdev.net/help/psyq_install.html).

This project uses [MBDesigns' PS1 Homebrew Source Code](https://mbdesigns.itch.io/ps1-homebrew-source-code-rendering-3d-graphics-with-psyq) as a base, and I recommend that as a place to get started if you want to make homebrew for PS1. There are some additional PSY-Q quirks mentioned in his .txt files that you may need to take care of before BKBS will compile.

### Additional Tools
* A prebuilt Windows version of [CSVtoNoteBin](https://github.com/hardrockbluesky/CSVtoNoteBin) is included in the Data folder of this project. It converts CSV files from the Unreal NoteBuilder project into binary.
* The Unreal 5 project to author CSV files is [over here](https://github.com/hardrockbluesky/NoteChartBuilder). Think of it as an in-editor tool.

If you're getting started with PS1 homebrew or want my thoughts about LibGS, TMD, PSY-Q, or just homebrew in general, check the [Development Notes](https://github.com/hardrockbluesky/BKBS_PSX/blob/main/Development%20Notes.md).


### Copyright/License:
* Contents of the Data and Audio folders are owned by [hardrockbluesky](https://hardrockbluesky.github.io/3D.html).
* You may, however, use those contents non-commercially provided you properly credit me and you. Make clear what is mine and what is yours. Your usage is not my endorsement of your project. If you use it for anything I deem in poor taste or hateful, your permission/license will be revoked.
* [mkpsxiso](https://github.com/Lameguy64/mkpsxiso) is included under GPL 2.0.
* .C source files are modified or copied from MBDesigns' project linked above, with permission given from him to redistribute and modify. (Main.c having the most extensive changes)
* Any included files or code from PSY-Q are Sony's. They are likely abandonware at this point but I do not own them and neither can you!
* Given the above, and excepting anything from PSY-Q, this project and its code other than the Data and Audio folders are [licensed under MIT](License.txt). I want to see more homebrew, please!

### Other Credits:
* Artist Chained_Tan - [Base for the character model](https://twitter.com/chained_tan/status/1425635566234873858).
* LameGuy64, Sparkletron, Nicholas Noble, SpicyJPEG, Stenzek, and many other kind people in the PSX.Dev Discord - Example code, sample projects, problem solving. Thanks so much!
