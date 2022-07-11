dd if=/dev/zero of=~/shared/disk.raw bs=1M count=16
boot/loader/install.sh -f ~/shared/disk.raw 0 8 boot/loader/bootloaderusb bitvisor.elf
qemu-img convert -f raw -O vmdk ~/shared/disk.raw ~/shared/bv.vmdk
