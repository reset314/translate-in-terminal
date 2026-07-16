import os
import csv
import struct

bins = { f"{c1}{c2}": [] for c1 in 'abcdefghijklmnopqrstuvwxyz' for c2 in 'abcdefghijklmnopqrstuvwxyz' }
bin_other = []

def sort_by_word():
    for c1 in 'abcdefghijklmnopqrstuvwxyz':
        for c2 in 'abcdefghijklmnopqrstuvwxyz':
            key = f"{c1}{c2}"
            bins[key] = sorted(bins[key], key=lambda x: x[0])


# stardict
with open("./ECDICT/ecdict.csv", newline = '') as f:
    reader = csv.reader(f)
    for row in reader:
        word_v = row[0]
        index_word = word_v[:2]
        if len(word_v) < 2:
            bin_other.append((row[0], row[1], row[2], row[3], row[4], row[10]))
        if index_word in bins:
            bins[index_word].append((row[0], row[1], row[2], row[3], row[4], row[10]))
        else:
            bin_other.append((row[0], row[1], row[2], row[3], row[4], row[10]))

sort_by_word()

def write_bin(key, entries):
    entries_data = []
    for item in entries:
        word, phonetic, definition, translate, pos, exchange = item
        entries_data.append((
            word.encode('utf-8') + b'\0',
            phonetic.encode('utf-8') + b'\0',
            definition.encode('utf-8') + b'\0',
            translate.encode('utf-8') + b'\0',
            pos.encode('utf-8') + b'\0',
            exchange.encode('utf-8') + b'\0'
        ))
    header_size = 32
    word_pool_offset = header_size
    word_pool = b''.join(d[0] for d in entries_data)
    phonetic_pool_offset = word_pool_offset + len(word_pool)
    phonetic_pool = b''.join(d[1] for d in entries_data)
    definition_pool_offset = phonetic_pool_offset + len(phonetic_pool)
    definition_pool = b''.join(d[2] for d in entries_data)
    translate_pool_offset = definition_pool_offset + len(definition_pool)
    translate_pool = b''.join(d[3] for d in entries_data)
    pos_pool_offset = translate_pool_offset + len(translate_pool)
    pos_pool = b''.join(d[4] for d in entries_data)
    exchange_pool_offset = pos_pool_offset + len(pos_pool)
    exchange_pool = b''.join(d[5] for d in entries_data)

    index_data = b''
    word_pos = word_pool_offset
    phonetic_pos = phonetic_pool_offset
    definition_pos = definition_pool_offset
    translate_pos = translate_pool_offset
    pos_pos = pos_pool_offset
    exchange_pos = exchange_pool_offset

    for d in entries_data:
        w_len = len(d[0]) - 1
        ph_len = len(d[1]) - 1
        def_len = len(d[2]) - 1
        tr_len = len(d[3]) - 1
        pos_len = len(d[4]) - 1
        ex_len = len(d[5]) - 1
        index_data += struct.pack('<I H', word_pos, w_len)
        index_data += struct.pack('<I H', phonetic_pos, ph_len)
        index_data += struct.pack('<I H', definition_pos, def_len)
        index_data += struct.pack('<I H', translate_pos, tr_len)
        index_data += struct.pack('<I H', pos_pos, pos_len)
        index_data += struct.pack('<I H', exchange_pos, ex_len)
        word_pos += len(d[0])
        phonetic_pos += len(d[1])
        definition_pos += len(d[2])
        translate_pos += len(d[3])
        pos_pos += len(d[4])
        exchange_pos += len(d[5])

    filename = f"./ecdict_bin/{key}.bin"
    with open(filename, 'wb') as f:
        f.write(struct.pack('<I I I I I I I I',
                            0x44494354,
                            len(entries),
                            word_pool_offset,
                            phonetic_pool_offset,
                            definition_pool_offset,
                            translate_pool_offset,
                            pos_pool_offset,
                            exchange_pool_offset))
        f.write(word_pool)
        f.write(phonetic_pool)
        f.write(definition_pool)
        f.write(translate_pool)
        f.write(pos_pool)
        f.write(exchange_pool)
        f.write(index_data)

for c1 in 'abcdefghijklmnopqrstuvwxyz':
    for c2 in 'abcdefghijklmnopqrstuvwxyz':
        key = f"{c1}{c2}"
        write_bin(key, bins[key])

write_bin("other", bin_other)
            
