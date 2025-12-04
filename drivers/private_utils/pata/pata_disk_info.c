#include "pata_utils.h"

void pata_decode_raw_disk_info(raw_disk_info_t dinfo, disk_info_t* result) {
	result->drivetype = dinfo.bl;
	result->sectors = dinfo.cl & 0b00111111;
	result->cylinders = ((dinfo.cl & 0b11000000) << 2) | dinfo.ch;
	result->heads = dinfo.dh;
	result->drives = dinfo.dl;
}

raw_disk_info_t pata_retrieve_raw_disk_info() {
	raw_disk_info_t* dinfo = (raw_disk_info_t*)0x5200;
	return *dinfo;
}

