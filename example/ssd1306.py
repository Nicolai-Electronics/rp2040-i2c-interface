import smbus, sys, time
bus = smbus.SMBus(1)

def init():
    bus.write_byte_data(0x3c, 0x00, 0xae)
    bus.write_byte_data(0x3c, 0x00, 0xd5)
    bus.write_byte_data(0x3c, 0x00, 0x80)
    bus.write_byte_data(0x3c, 0x00, 0xa8)
    bus.write_byte_data(0x3c, 0x00, 0x3f)
    bus.write_byte_data(0x3c, 0x00, 0xd3)
    bus.write_byte_data(0x3c, 0x00, 0x00)
    bus.write_byte_data(0x3c, 0x00, 0x40)
    bus.write_byte_data(0x3c, 0x00, 0x20)
    bus.write_byte_data(0x3c, 0x00, 0x00)
    bus.write_byte_data(0x3c, 0x00, 0xa1)
    bus.write_byte_data(0x3c, 0x00, 0xc8)
    bus.write_byte_data(0x3c, 0x00, 0xda)
    bus.write_byte_data(0x3c, 0x00, 0x12)
    bus.write_byte_data(0x3c, 0x00, 0x81)
    bus.write_byte_data(0x3c, 0x00, 0xcf)
    bus.write_byte_data(0x3c, 0x00, 0xd9)
    bus.write_byte_data(0x3c, 0x00, 0xf1)
    bus.write_byte_data(0x3c, 0x00, 0xdb)
    bus.write_byte_data(0x3c, 0x00, 0x30)
    bus.write_byte_data(0x3c, 0x00, 0x8d)
    bus.write_byte_data(0x3c, 0x00, 0x14)
    bus.write_byte_data(0x3c, 0x00, 0x2e)
    bus.write_byte_data(0x3c, 0x00, 0xa4)
    bus.write_byte_data(0x3c, 0x00, 0xa6)
    bus.write_byte_data(0x3c, 0x00, 0xaf)

def position(x0 = 0, x1 = 127, y0 = 0, y1 = 7):
    bus.write_byte_data(0x3c, 0x00, 0x21)
    bus.write_byte_data(0x3c, 0x00, x0)
    bus.write_byte_data(0x3c, 0x00, x1)
    bus.write_byte_data(0x3c, 0x00, 0x22)
    bus.write_byte_data(0x3c, 0x00, y0)
    bus.write_byte_data(0x3c, 0x00, y1)

init()
position()

def convert(data_in):
    data_out = [0x00] * 128 * 8
    for x in range(128):
        for y in range(64):
            in_addr = x // 8 + (y * 128//8)
            in_bit = 7 - x % 8
            out_addr = x + ((y // 8)*128)
            out_bit = y % 8
            if data_in[in_addr] & 1 << in_bit:
                data_out[out_addr] |= 1 << out_bit
    return data_out

for x in range(64):
    bus.write_i2c_block_data(0x3c, 0x40, [0x00] * 16)

while True:
    data_in = list(sys.stdin.buffer.read(128*64//8))
    data_out = convert(data_in)
    for position in range(128*8//32):
        out = data_out[position*32:(position*32)+32]
        bus.write_i2c_block_data(0x3c, 0x40, out)
