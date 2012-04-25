#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/delay.h>
#define MASTER                      1
#define SPDIF
#undef  SPDIFIOCTL

#define SPDIF_CONFIG_CHANNEL(x)     ((3&x)<<20)
#define SPDIF_MODE_LEFT             1
#define SPDIF_MODE_RIGHT            2
#define SPDIF_CONFIG_SAMP_FREQ(x)   ((0xf&x)<<24)
#define SPDIF_SAMP_FREQ_48          2
#define SPDIF_SAMP_FREQ_44          0
#define SPDIF_CONFIG_ORG_FREQ(x)    ((0xf&x)<<4)
#define SPDIF_ORG_FREQ_48           0xd
#define SPDIF_ORG_FREQ_44           0xf
#define SPDIF_CONFIG_SAMP_SIZE(x)   (0xf&x)
#define SPDIF_S_8_16                2
#define SPDIF_S_24_32               0xb

#define PAUSE				        0x1
#define START				        0x2
#define RESUME				        0x4

#define STATUSMASK1			        (1ul << 31)
#define STATUSMASK			        (1ul << 4)
#define SHIFT_WIDTH			        11
#define ADDR_VALID			        1

#define I2S_RESET                   1
#define MBOX_RESET                  (1ul << 1)
#define MBOX_INTR_MASK              (1ul << 7)

#define MONO                    	(1ul << 14)
#define ATH_I2S_NUM_DESC            160
#define ATH_I2S_BUFF_SIZE           24

#define I2S_LOCK_INIT(_sc)		spin_lock_init(&(_sc)->i2s_lock)
#define I2S_LOCK_DESTROY(_sc)
#define I2S_LOCK(_sc)			spin_lock_irqsave(&(_sc)->i2s_lock, (_sc)->i2s_lockflags)
#define I2S_UNLOCK(_sc)			spin_unlock_irqrestore(&(_sc)->i2s_lock, (_sc)->i2s_lockflags)

#define I2S_VOLUME      _IOW('N', 0x20, int)
#define I2S_FREQ        _IOW('N', 0x21, int)
#define I2S_DSIZE       _IOW('N', 0x22, int)
#define I2S_MODE        _IOW('N', 0x23, int)
#define I2S_FINE        _IOW('N', 0x24, int)
#define I2S_COUNT       _IOWR('N', 0x25, int)
#define I2S_PAUSE       _IOWR('N', 0x26, int)
#define I2S_RESUME      _IOWR('N', 0x27, int)
#define I2S_MCLK        _IOW('N', 0x28, int)

typedef struct {
	unsigned int OWN		:  1,    /* bit 00 */
	             EOM		:  1,    /* bit 01 */
	             rsvd1	    :  6,    /* bit 07-02 */
	             size	    : 12,    /* bit 19-08 */
	             length	    : 12,    /* bit 31-20 */
	             rsvd2	    :  4,    /* bit 00 */
	             BufPtr	    : 28,    /* bit 00 */
	             rsvd3	    :  4,    /* bit 00 */
	             NextPtr	: 28;    /* bit 00 */
#ifdef SPDIF
    unsigned int Va[6];
    unsigned int Ua[6];
    unsigned int Ca[6];
    unsigned int Vb[6];
    unsigned int Ub[6];
    unsigned int Cb[6];
#endif
} ath_mbox_dma_desc;

/*
 * XXX : This is the interface between i2s and wlan
 *       When adding info, here please make sure that
 *       it is reflected in the wlan side also
 */
typedef struct i2s_stats {
    unsigned int write_fail;
    unsigned int rx_underflow;
} i2s_stats_t;

typedef struct i2s_buf {
    uint8_t *bf_vaddr;
    unsigned long bf_paddr;
} i2s_buf_t;

typedef struct i2s_dma_buf {
    ath_mbox_dma_desc *lastbuf;
    ath_mbox_dma_desc *db_desc;
    dma_addr_t db_desc_p;
    i2s_buf_t db_buf[ATH_I2S_NUM_DESC];
    int tail;
} i2s_dma_buf_t;

typedef struct ath_i2s_softc {
	int			ropened,
				popened,
				sc_irq,
				ft_value,
				ppause,
				rpause;
	char			*sc_pmall_buf,
				*sc_rmall_buf;
	i2s_dma_buf_t		sc_pbuf,
				sc_rbuf;
	wait_queue_head_t	wq_rx,
				wq_tx;
	spinlock_t		i2s_lock;
    unsigned long i2s_lockflags;
	struct timer_list	pll_lost_lock;
	uint32_t		pll_timer_inited,
				dsize,
				vol,
				freq;
} ath_i2s_softc_t;

ath_i2s_softc_t sc_buf_var;
i2s_stats_t stats;

