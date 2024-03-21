xcopy .\mkpsxiso\cuesheet.xml .

mkpsxiso64.exe -o game.bin -y cuesheet.xml

del cuesheet.xml

pause

