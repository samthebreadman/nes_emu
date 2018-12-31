// NES Emulator
// An Emulator for the Nintendo Entertainment System
////////////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void error_handle(const char* format, ...)
{
    va_list arg;
    
    va_start(arg, format);
    vfprintf(stderr, format, arg);
    va_end(arg);

    exit(0x1);
}

struct cart
{
    uint8_t mapper;

    uint32_t prg_rom_size;
    uint8_t* prg_rom;
    
    bool prg_ram_battery;
    uint8_t prg_ram_size;
    uint8_t* prg_ram;

    uint32_t chr_rom_size;
    uint8_t* chr_rom;
};

void cart_load(struct cart *cart, char* filename)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
        error_handle("Error: %s not found. Aborting...", filename);

    uint8_t magic[0x4];
    fread(magic, sizeof(uint8_t), 0x4, fp);
    if (memcmp(magic, "NES\32", 0x4))
        error_handle("Error: Invalid iNES header. Aborting...");

    fseek(fp, 0x6, SEEK_SET);
    cart->mapper = (fgetc(fp) >> 0x4) | (fgetc(fp) & 0xF0);

    fseek(fp, 0x6, SEEK_SET);
    cart->prg_ram_battery = fgetc(fp) & 0x2;
    fseek(fp, 0x4, SEEK_SET);
    cart->prg_rom_size = fgetc(fp) * 0x4000;
    cart->prg_rom = malloc(cart->prg_rom_size);
    fseek(fp, 0x10, SEEK_SET);
    fread(cart->prg_rom, sizeof(uint8_t), cart->prg_rom_size, fp);
    
    fseek(fp, 0x5, SEEK_SET);
    cart->chr_rom_size = fgetc(fp) * 0x2000;
    cart->chr_rom = malloc(cart->chr_rom_size);
    fseek(fp, 0x10 + cart->prg_rom_size, SEEK_SET);
    fread(cart->chr_rom, sizeof(uint8_t), cart->chr_rom_size, fp);

    fseek(fp, 0x8, SEEK_SET);
    cart->prg_ram_size = fgetc(fp);
    cart->prg_ram = malloc(cart->prg_ram_size);

    fclose(fp);
}

void cart_destroy(struct cart *cart)
{
    free(cart->prg_ram);
    free(cart->chr_rom);
    free(cart->prg_rom);
}

int main (int argc, char* argv[])
{
    if (argc < 0x2)
        error_handle("Usage: \"emulator.exe filename\"");

    struct cart cart;

    cart_load(&cart, argv[0x1]);

    cart_destroy(&cart);

    return 0x0;
}