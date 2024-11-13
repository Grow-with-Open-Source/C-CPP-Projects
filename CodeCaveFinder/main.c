#include <windows.h>
#include <stdio.h>
#include <winternl.h>

#pragma comment(lib, "ntdll.lib")

#define CAVE_SIZE 50
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

typedef NTSTATUS(NTAPI* _NtReadFile)(
    HANDLE FileHandle,
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    PVOID Buffer,
    ULONG Length,
    PLARGE_INTEGER ByteOffset,
    PULONG Key
);

int FindCodeCave(char* target) {
    IO_STATUS_BLOCK iosb;
    HANDLE file;
    
    file = CreateFileA(target, GENERIC_READ, 0, NULL, OPEN_EXISTING, 
                      FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        printf("[!] Failed to open file\n");
        return 0;
    }

    DWORD fsize = GetFileSize(file, NULL);
    BYTE* buffer = (BYTE*)malloc(fsize);
    if (!buffer) {
        printf("[!] Failed to allocate memory\n");
        CloseHandle(file);
        return 0;
    }

    _NtReadFile* NtReadFile = (_NtReadFile*)GetProcAddress(
        GetModuleHandleA("ntdll.dll"), 
        "NtReadFile"
    );
    if (!NtReadFile) {
        printf("[!] Failed to get NtReadFile\n");
        free(buffer);
        CloseHandle(file);
        return 0;
    }

    NTSTATUS status = NtReadFile(file, NULL, NULL, NULL, &iosb, 
                                buffer, fsize, NULL, NULL);
    CloseHandle(file);

    if (status != STATUS_SUCCESS) {
        printf("[!] Failed to read file\n");
        free(buffer);
        return 0;
    }

    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)buffer;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(buffer + dos->e_lfanew);
    PIMAGE_SECTION_HEADER first = IMAGE_FIRST_SECTION(nt);

    for (int i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        BYTE* start = buffer + first[i].PointerToRawData;
        DWORD size = first[i].SizeOfRawData;
        int nullcount = 0;
        int total_caves = 0;

        for (DWORD x = 0; x < size; x++) {
            __asm {
                mov esi, start          ; load section start
                add esi, x              ; add current offset
                xor eax, eax           ; clear eax
                mov al, byte ptr[esi]  ; load current byte
                test al, al            ; check if zero
                jnz reset_counter      ; if not zero, reset
                inc nullcount          ; increment counter
                jmp check_size         ; check cave size
                reset_counter:         
                mov nullcount, 0       ; reset counter
                check_size:            ; continue
            }

            if (nullcount >= CAVE_SIZE) {
                printf("[+] Cave in section %s at RVA: 0x%x with size: %d bytes\n",
                       first[i].Name, 
                       first[i].VirtualAddress + x - nullcount + 1, 
                       nullcount);
                total_caves++;
                nullcount = 0;
            }
        }
        if (total_caves > 0) {
            printf("\n[*] Found %d caves in section %s\n", 
                   total_caves, first[i].Name);
        }
    }

    free(buffer);
    return 1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("[!] Usage: %s <file>\n", argv[0]);
        return 1;
    }

    printf("[*] Looking for caves >= %d bytes...\n\n", CAVE_SIZE);
    FindCodeCave(argv[1]);
    return 0;
}
