#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_DEPTH 1000000

int get_depth();

void spawn_recursive(int depth, char *prog_path);

int main() {
    char prog_path[MAX_PATH];
    GetModuleFileName(NULL, prog_path, MAX_PATH);

    int depth = get_depth();

    if (depth >= MAX_DEPTH) {
        printf("Reached max depth %d\n", depth);
        return 0;
    }

    spawn_recursive(depth, prog_path);
    return 0;
}

int get_depth() {
    // Use environment variable to store depth, initially 0
    char *env = getenv("RECURSIVE_DEPTH");
    if (!env) return 0;
    return atoi(env);
}

void spawn_recursive(int depth, char *prog_path) {
    char env_val[16];
    sprintf(env_val, "%d", depth + 1);

    // Set environment variable for child process
    SetEnvironmentVariable("RECURSIVE_DEPTH", env_val);

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (!CreateProcess(NULL, prog_path, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("CreateProcess failed at depth %d, error %lu\n", depth, GetLastError());
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    printf("Finished depth %d\n", depth);
}
