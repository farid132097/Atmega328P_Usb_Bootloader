1. Change Fuse bits [LFUSE: 0xFF]  [HFUSE: 0xD0] [LOCKFUSE: 0xCF]
2. Copy both of the files (Makefile and upload.bat) to your WinAvr project directory
3. You've two options to upload the compiled hex [while the device is in USB Bootloader Mode]
   (i ) Just double-click "upload.bat" or
   (ii) From WinAvr>Tools>Program [if you've multiple .c files, you must have to add inside the makefile]