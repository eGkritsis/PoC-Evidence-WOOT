import random

# === CONFIGURABLE ===
ROUNDS = 6000
KEY_LEN = 31
URL = "https://secure.eicar.org/eicar.com"

# ====================

url_bytes = bytearray(URL.encode('utf-8'))
URL_LEN = len(url_bytes)

def generate_key(seed):
    random.seed(seed * 12345)
    return bytearray((random.randint(0, 25) + ord('a')) for _ in range(KEY_LEN))

# Save keys for debug/validation
keys = []

# Encrypt in layers
for i in range(ROUNDS):
    key = generate_key(i)
    keys.append(key)
    for j in range(URL_LEN):
        url_bytes[j] ^= key[j % KEY_LEN]

def write_keys_to_header(keys, filename="keys.h"):
    with open(filename, "w") as f:
        f.write("// Keys used in reverse order for decryption\n")
        f.write(f"uint8_t keys[{len(keys)}][{len(keys[0])}] = {{\n")
        for key in keys:
            f.write("    {\n        ")
            f.write(", ".join(f"0x{k:02X}" for k in key))
            f.write("\n    },\n")
        f.write("};\n")

# Assuming 'keys' is a list of bytes objects (keys in reverse order)
write_keys_to_header(keys)

def write_encrypted_url_to_header(data, filename="encrypted_url.h"):
    with open(filename, "w") as f:
        f.write(f"#define URL_LEN {len(data)}\n")
        f.write("uint8_t encrypted_url[URL_LEN] = {\n    ")
        f.write(", ".join(f"0x{b:02X}" for b in data))
        f.write("\n};\n")

write_encrypted_url_to_header(url_bytes)

# Output the encrypted array
print(f"#define URL_LEN {URL_LEN}")
print("unsigned char encrypted_url[URL_LEN] = { ", end="")
print(", ".join(f"0x{b:02X}" for b in url_bytes), end=" ")
print("};")

# Optional: print keys in hex (for validation or embedding)
print("\n// Debug keys used:")
for i, key in enumerate(keys):
    hexed = ", ".join(f"0x{b:02X}" for b in key)
    print(f"// Round {i} key: {{ {hexed} }}")
