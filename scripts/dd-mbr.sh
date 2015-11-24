diskutil unmount /dev/disk2
dd if=mbr.bin of=/dev/disk2 bs=442
