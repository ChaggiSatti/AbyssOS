#include "ata.h"
#include "io.h"

#define ATA_DATA        0x1F0
#define ATA_SECTOR_CNT  0x1F2
#define ATA_LBA_LOW     0x1F3
#define ATA_LBA_MID     0x1F4
#define ATA_LBA_HIGH    0x1F5
#define ATA_DRIVE       0x1F6
#define ATA_COMMAND     0x1F7
#define ATA_STATUS      0x1F7

#define ATA_CMD_READ_PIO  0x20
#define ATA_CMD_WRITE_PIO 0x30

#define ATA_SR_BSY     0x80    
#define ATA_SR_DRQ     0x08 

void ata_wait_bsy(){
    while(inb(ATA_STATUS) & ATA_SR_BSY);
}

void ata_wait_drq(){
    while(!(inb(ATA_STATUS) & ATA_SR_DRQ));
}

void ata_init(){
    outb(0x3F6, 0x02);
}

void ata_read_sector(uint32_t lba, uint8_t* buf){
    ata_wait_bsy();
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_CNT, 1);
    outb(ATA_LBA_LOW, (uint8_t)lba);
    outb(ATA_LBA_MID, (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));

    outb(ATA_COMMAND, ATA_CMD_READ_PIO);
    ata_wait_bsy();
    ata_wait_drq();

    insw(ATA_DATA, buf, 256);
}

void ata_write_sector(uint32_t lba, uint8_t* buffer) {
    ata_wait_bsy();
    
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_CNT, 1);
    outb(ATA_LBA_LOW, (uint8_t)lba);
    outb(ATA_LBA_MID, (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));
    
    outb(ATA_COMMAND, ATA_CMD_WRITE_PIO);
    
    ata_wait_bsy();
    ata_wait_drq();
    
    outsw(ATA_DATA, buffer, 256);
    ata_wait_bsy();
    outb(ATA_COMMAND, 0xE7); 
    ata_wait_bsy();
}