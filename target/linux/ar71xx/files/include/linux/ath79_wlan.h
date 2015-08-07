#ifndef _ATH79_WLAN_H_
#define _ATH79_WLAN_H

#define ATH79_WLAN_FW_DUMP

#ifdef ATH79_WLAN_FW_DUMP
#include <linux/export.h>
#define ATH79_FW_DUMP_MEM_SIZE          ((2*1024*1024))

int ath79_get_wlan_fw_dump_buffer(void** dump_buff, u32 *buff_size);
#endif /* ATH79_WLAN_FW_DUMP */

#endif /* _ATH79_WLAN_H_ */
