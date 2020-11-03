/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2014      */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "../../crt0.h"
#include <stddef.h>

static uint16_t drive = 0xFFFF;
static DWORD current_sector = 0xFFFFFFFF;
static uint8_t buf[514] = {};

void setDrive(uint16_t drv){
	drive = drv;
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (void)
{
	DSTATUS stat = 0;

	if (drive == 0xFFFF) {
		stat = STA_NOINIT;
	}

	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE* buff,		/* Pointer to the destination object */
	DWORD sector,	/* Sector number (LBA) */
	UINT offset,	/* Offset in the sector */
	UINT count		/* Byte count (bit15:destination) */
)
{
	DRESULT res = 0;
	UINT i;
	
	if ((buff == NULL) || (offset >= 512) || ((offset + count) > 512)){
		res = RES_PARERR;
	} else {
	
		if (current_sector != sector) {
			readSector(sector, drive, buf, 512);
			current_sector = sector;
		}
	
		const uint8_t* tmpbuf = buf;
		tmpbuf = tmpbuf + offset;
		for (i = 0; i < count; i++) {
			if (buff) {
				*buff++ = *tmpbuf++;
			}
		}
	}
	// Put your code here

	return res;
}


#if 0
/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

DRESULT disk_writep (
	BYTE* buff,		/* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
	DWORD sc		/* Sector number (LBA) or Number of bytes to send */
)
{
	DRESULT res;


	if (!buff) {
		if (sc) {

			// Initiate write process

		} else {

			// Finalize write process

		}
	} else {

		// Send data to the disk

	}

	return res;
}

#endif
