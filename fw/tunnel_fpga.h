#include "fpga.h"

extern const struct regfield fpga_regtable[];

#define FPGA_BASE 0x03000000

#define FPGAREG_ID 0x0000
#define FPGAREG_TEST 0x0004

#define FPGAREG_INT_ENABLE 0x0008
#define FPGAREG_INT_PENDING 0x000c

#define ETH_RX_IRQ 0x1 // Ethernet Rx data available interrupt

#define FPGAREG_PCIE_STATUS 0x0010
#define LMMI_BUSY_BIT 0x80000000
#define PL_LINK_UP_BIT 0x40000000
#define DL_LINK_UP_BIT 0x20000000
#define TL_LINK_UP_BIT 0x10000000
#define SOP_COUNT_SHIFT 0
#define SOP_COUNT_WIDTH 16
#define SEND_COUNT_SHIFT 16
#define SEND_COUNT_WIDTH 12

#define FPGAREG_PCIE_CTRL 0x0014

#define RESET_BIT 0x01
#define CONFIG_BIT 0x10000000
#define SEND_BIT 0x20000000
#define LMMI_BIT 0x40000000
#define WRITE_BIT 0x80000000

#define FPGAREG_PCIE_LMMI 0x0018

#define FPGAREG_PCIE_CONFIG 0x001C

#define FPGAREG_PCIE_FIFO 0x0030

#define FPGAREG_PCIE_FIFO_STATUS 0x0034

#define SOP_BIT 0x01
#define EOP_BIT 0x02
#define NE_BIT 0x04

#define FPGAREG_SDRAM_LOW 0x80
#define FPGAREG_SDRAM_HIGH 0x84
#define FPGAREG_SDRAM_ADDR 0x88

#define SDRAM_WRITE 0x800000
#define SDRAM_MODE 0x1000000
#define SDRAM_REQ_DONE 0x80000000
#define SDRAM_READ_DONE 0x40000000


void reboot();
