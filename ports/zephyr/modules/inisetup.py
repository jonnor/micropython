import vfs
from micropython import const

def check_bootsec(block_dev):
    buf = bytearray(block_dev.ioctl(5, 0))  # 5 is SEC_SIZE
    block_dev.readblocks(0, buf)
    empty = True
    for b in buf:
        if b != 0xFF:
            empty = False
            break
    if empty:
        return True
    fs_corrupted()


def fs_corrupted():
    import time
    import micropython

    # Allow this loop to be stopped via Ctrl-C.
    micropython.kbd_intr(3)

    while 1:
        print(
            """\
The filesystem appears to be corrupted. If you had important data there, you
may want to make a flash snapshot to try to recover it. Otherwise, perform
factory reprogramming of MicroPython firmware (completely erase flash, followed
by firmware programming).
"""
        )
        time.sleep(3)


def setup(block_dev, prog_size=256):
    check_bootsec(block_dev)
    print("Performing initial setup")
    if block_dev.info()[4] == "vfs":
        vfs.VfsLfs2.mkfs(block_dev, progsize=prog_size)
        fs = vfs.VfsLfs2(block_dev, progsize=prog_size)
    elif block_dev.info()[4] == "ffat":
        vfs.VfsFat.mkfs(block_dev, progsize=prog_size)
        fs = vfs.VfsFat(block_dev, progsize=prog_size)
    vfs.mount(fs, "/")
    with open("boot.py", "w") as f:
        f.write(
            """\
# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
#import webrepl
#webrepl.start()
"""
        )
    return fs
