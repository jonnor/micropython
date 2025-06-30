import gc
import micropython
from zephyr import FlashArea
import vfs

BLOCK_SIZE = const(512)
PROG_SIZE = const(256)

block_dev = FlashArea(FlashArea.ID_Storage, BLOCK_SIZE)

try:
    if block_dev:
        fs = vfs.VfsLfs2(block_dev, progsize=PROG_SIZE)
        vfs.mount(fs, "/")
except OSError:
    import inisetup

    inisetup.setup(block_dev, PROG_SIZE)

sys.path.append("/")
sys.path.append("/lib")
gc.collect()
