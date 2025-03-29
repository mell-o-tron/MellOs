MAX_CYLINDERS = 79
MAX_HEADS = 1
MAX_SECTORS = 36
SECTOR_SIZE = 512
START_MEM = 0x8200

START_FROM = 0x04

def lba_to_chs(lba):
    lba += START_FROM - 1
    cylinder = lba // ((MAX_HEADS + 1) * MAX_SECTORS)
    remainder = lba % ((MAX_HEADS + 1) * MAX_SECTORS)
    head = remainder // MAX_SECTORS
    sector = (remainder % MAX_SECTORS) + 1
    return (cylinder, head, sector)

def where_to_map(lba):
    chs = lba_to_chs(lba)
    return (chs[0], chs[1], chs[2], hex(START_MEM + lba * SECTOR_SIZE))

# print(where_to_map(0), '-', where_to_map(127))
# print(where_to_map(128), '-', where_to_map(255))

print(where_to_map(0), '-', where_to_map(62))
print(where_to_map(63), '-', where_to_map(63 + 127))
print(where_to_map(63 + 128), '-', where_to_map(63 + 128 + 127))