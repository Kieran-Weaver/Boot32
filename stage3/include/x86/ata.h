#ifndef ATA_H
#define ATA_H
#include <stdint.h>

#define ATA_PRIMARY_IO 0x1F0
#define ATA_PRIMARY_CTL 0x3F6
#define ATA_SECONDARY_IO 0x170
#define ATA_SECONDARY_CTL 0x376

// Offsets from IO base
#define ATA_DR 		0x0
#define ATA_ERR 	0x1
#define ATA_FEAT 	0x1
#define ATA_SC		0x2
#define ATA_LBALO	0x3
#define ATA_LBAMID	0x4
#define ATA_LBAHI	0x5
#define ATA_DSEL	0x6
#define ATA_STATUS	0x7
#define ATA_CMDR	0x7

// Error register bits
#define ATA_AMNF	0
#define ATA_TKZNF	1
#define ATA_ABRT	2
#define ATA_MCR		3
#define ATA_IDNF	4
#define ATA_MC		5
#define ATA_UNC		6
#define ATA_BBK		7

// Device control register
#define ATA_nIEN	1
#define ATA_SRST	2
#define ATA_HOB		7

// Offsets from CTL base
#define ATA_CTL_STATUS	0x0
#define ATA_CTL_CTLR	0x0
#define ATA_CTL_DEVADDR	0x1

void ata_soft_reset(uint32_t ctl_base);
uint16_t detect_devtype(uint8_t slavebit, uint32_t ata_base, uint32_t ctl_base);
uint8_t ata_get_status(uint32_t ctl_base);
void ata_recv(uint32_t io_base, uint16_t* data, uint32_t num);
#endif
