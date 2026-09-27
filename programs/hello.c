// The kernel passes us an API function to open the Notepad
typedef void (*api_open_notepad)(const char*);

void main(api_open_notepad open_notepad) {
    open_notepad("SUCCESS!\n\nThis text was written by HELLO.BIN.\nI am a standalone C program loaded\noff the FAT32 hard drive!");
}
