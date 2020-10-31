#include "ata.h"
#include "intrinsics.h"
uint8_t ata_get_status(uint32_t ctl_base){
	return inb(ctl_base + ATA_CTL_STATUS);
}
void ata_soft_reset(uint32_t ctl_base){
	uint8_t ctrl_reg = inb(ctl_base + ATA_CTL_CTLR);
	ctrl_reg |= (1 << ATA_SRST);
	outb(ctl_base + ATA_CTL_CTLR, ctrl_reg);
	ctrl_reg &= ~(1 << ATA_SRST);
	outb(ctl_base + ATA_CTL_CTLR, ctrl_reg);
	while (ata_get_status(ctl_base) & 0x80){
		asm volatile ( "outb %%al, $0x80" : : "a"(0) );
	}
}
uint16_t detect_devtype(uint8_t slavebit, uint32_t ata_base, uint32_t ctl_base){
	outb(ata_base + ATA_DSEL, 0xA0 | slavebit<<4);
	inb(ctl_base);			/* wait 400ns for drive select to work */
	inb(ctl_base);
	inb(ctl_base);
	inb(ctl_base);
	uint8_t cl=inb(ata_base + ATA_LBAMID);	/* get the "signature bytes" */
	uint8_t ch=inb(ata_base + ATA_LBAHI);
	uint16_t dc = ch;
	dc = (dc << 8) + cl;
	return dc;
}
void ata_recv(uint32_t ata_base, uint16_t* data, uint32_t num){
	rep_insw(ata_base, data, num);
}
