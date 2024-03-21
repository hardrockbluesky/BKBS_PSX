ccpsx -Xo0x80010000 -Wall -O2 main.c -o main.cpe
cpe2x /ca main.cpe

.\mkpsxiso\mkpsxiso.exe -o game.bin -y .\mkpsxiso\cuesheet.xml

pause "delete extra files?"
del mem.map
del main.sym
del main.cpe
del main.exe
del image.cue
del game.iso