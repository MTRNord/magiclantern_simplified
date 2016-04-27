#ifndef HW_EOS_H

#define HW_EOS_H

/* macros to define machine types */
#define EOS_MACHINE(cam, addr, digic_version) \
    static void eos_init_##cam(MachineState *args) \
    { eos_init_common("ROM-"#cam".BIN", addr, digic_version); } \
    \
    QEMUMachine canon_eos_machine_##cam = { \
        .name = #cam, 0, \
        .desc = "Canon EOS "#cam, \
        .init = &eos_init_##cam, \
    };

/** Some small engio API **/
#define REG_PRINT_CHAR 0xCF123000
#define REG_SHUTDOWN   0xCF123004
#define REG_DUMP_VRAM  0xCF123008
#define REG_PRINT_NUM  0xCF12300C
#define REG_GET_KEY    0xCF123010
#define REG_BMP_VRAM   0xCF123014
#define REG_IMG_VRAM   0xCF123018
#define REG_RAW_BUFF   0xCF12301C
#define REG_DISP_TYPE  0xCF123020

#define COUNT(x)        ((int)(sizeof(x)/sizeof((x)[0])))

#define STR_APPEND(orig,fmt,...) ({ int _len = strlen(orig); snprintf(orig + _len, sizeof(orig) - _len, fmt, ## __VA_ARGS__); });

#define BMPPITCH 960
#define BM(x,y) ((x) + (y) * BMPPITCH)

/** ARM macros **/

#define B_INSTR(pc,dest) \
            ( 0xEA000000 \
            | ((( ((uint32_t)dest) - ((uint32_t)pc) - 8 ) >> 2) & 0x00FFFFFF) \
            )

#define FAR_CALL_INSTR   0xe51ff004    // ldr pc, [pc,#-4]
#define LOOP_INSTR       0xeafffffe    // 1: b 1b

/** Memory configuration **/
#define ROM0_ADDR     0xF0000000
#define ROM1_ADDR     0xF8000000
#define ROM0_SIZE     0x01000000
#define ROM1_SIZE     0x01000000

#define TCM_SIZE      0x00001000
#define RAM_SIZE      0x40000000
#define CACHING_BIT   0x40000000

#define RAM2_ADDR     0x80000000    /* a second memory block on DIGIC 6? */
#define RAM2_SIZE     0x40000000

#define IO_MEM_START  0xC0000000    /* common to all DIGICs */
#define IO_MEM_LEN45  0x10000000    /* for DIGIC 4/5 */
#define IO_MEM_LEN6   0x20000000    /* for DIGIC 6 */

/* define those for logging RAM access (reads + writes) */
/* caveat: this area will be marked as IO, so you can't execute anything from there */
//~ #define TRACE_MEM_START  0x00000000
//~ #define TRACE_MEM_LEN    0x00800000

#define Q_HELPER_ADDR 0x30000000

/* defines for memory/register access */
#define INT_ENTRIES 0x100

#define MODE_MASK  0xF0
#define MODE_READ  0x10
#define MODE_WRITE 0x20

typedef struct
{
    uint8_t transfer_format;
    uint8_t current_reg;
    uint8_t regs[16];
} RTCState;

typedef struct
{
    SDState *card;
    uint32_t cmd_hi;
    uint32_t cmd_lo;
    uint32_t cmd_flags;
    uint32_t irq_flags;
    uint32_t read_block_size;
    uint32_t write_block_size;
    uint32_t transfer_count;
    uint32_t dma_enabled;
    uint32_t dma_addr;
    uint32_t dma_count;
    uint32_t response[5];
    uint32_t status;
} SDIOState;

typedef struct
{
    IDEBus bus;
} CFState;

struct palette_entry
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t opacity;
};

typedef struct
{
    QemuConsole *con;
    int invalidate;
    enum {DISPLAY_LCD, DISPLAY_HDMI_1080, DISPLAY_HDMI_480, DISPLAY_SD_PAL, DISPLAY_SD_NTSC} type;
    uint32_t bmp_vram;
    uint32_t img_vram;
    uint32_t raw_buff;
    struct palette_entry palette_4bit[16];
    struct palette_entry palette_8bit[256];
    int is_4bit;
} DispState;

typedef struct
{
    int buf[16];    /* ring buffer */
    int head;       /* for extracting keys from the buffer */
    int tail;       /* for inserting keys into the buffer */
} KeybState;

struct HPTimer
{
    int active;
    int output_compare;
    int triggered;
};

struct mpu_init_spell
{
  unsigned char in_spell[128];
  unsigned char out_spells[128][128];  
};

typedef struct
{
    int status;                     /* register 0xC022009C */
    int sending;
    int receiving;
    
    unsigned char recv_buffer[128];
    int recv_index;
    
    int spell_set;              /* used for replaying MPU messages */
    int out_spell;
    int out_char;

    struct { short spell_set; short out_spell; } send_queue[0x100];
    int sq_head;                /* for extracting items */
    int sq_tail;                /* for inserting (queueing) items */

} MPUState;

typedef struct
{
    ARMCPU *cpu;
    MemoryRegion *system_mem;
    MemoryRegion tcm_code;
    MemoryRegion tcm_data;
    MemoryRegion ram;
    MemoryRegion ram_uncached;
    MemoryRegion ram2;
    MemoryRegion rom0;
    MemoryRegion rom1;
    uint8_t *rom0_data;
    uint8_t *rom1_data;
    MemoryRegion iomem;
    MemoryRegion tracemem;
    MemoryRegion tracemem_uncached;
    qemu_irq interrupt;
    QemuThread interrupt_thread_id;
    uint32_t verbosity;
    uint32_t tio_rxbyte;
    uint32_t irq_enabled[INT_ENTRIES];
    uint32_t irq_schedule[INT_ENTRIES];
    uint32_t irq_id;
    QemuMutex irq_lock;
    uint32_t digic_timer;
    uint32_t timer_reload_value[3];
    uint32_t timer_current_value[3];
    uint32_t timer_enabled[3];
    struct HPTimer HPTimers[8];
    uint32_t clock_enable;
    uint32_t flash_state_machine;
    DispState disp;
    KeybState keyb;
    RTCState rtc;
    SDIOState sd;
    CFState cf;
    MPUState mpu;
} EOSState;

typedef struct
{
    SysBusDevice busdev;
    MemoryRegion mem;
    void *storage;

    BlockDriverState *bs;
    uint32_t nb_blocs;
    uint64_t sector_len;
    uint8_t width;
    uint8_t be;
    int wcycle;
    int bypass;
    int ro;
    uint8_t cmd;
    uint8_t status;
    uint16_t ident0;
    uint16_t ident1;
    uint16_t ident2;
    uint16_t ident3;
    char *name;
} ROMState;

typedef struct
{
    const char *name;
    unsigned int start;
    unsigned int end;
    unsigned int (*handle) ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
    unsigned int parm;
} EOSRegionHandler;

unsigned int eos_handle_rom ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_flashctrl ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_dma ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_ram ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_sio ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_sio3 ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_mreq ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_cartridge ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_tio ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_timers ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_timers_ ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_digic_timer ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_hptimer ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_intengine ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_basic ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_unk ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_gpio ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_sdio ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_sddma ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_cfdma ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_asif ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_handle_display ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );

unsigned int eos_handle_ml_helpers ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );

unsigned int eos_handle_digic6 ( unsigned int parm, EOSState *s, unsigned int address, unsigned char type, unsigned int value );

void eos_set_mem_w ( EOSState *s, uint32_t addr, uint32_t val );
void eos_set_mem_h ( EOSState *s, uint32_t addr, uint16_t val );
void eos_set_mem_b ( EOSState *s, uint32_t addr, uint8_t val );
uint32_t eos_get_mem_w ( EOSState *s, uint32_t addr );
uint16_t eos_get_mem_h ( EOSState *s, uint32_t addr );
uint8_t eos_get_mem_b ( EOSState *s, uint32_t addr );

unsigned int eos_default_handle ( EOSState *s, unsigned int address, unsigned char type, unsigned int value );
EOSRegionHandler *eos_find_handler( unsigned int address);
unsigned int eos_handler ( EOSState *s, unsigned int address, unsigned char type, unsigned int value );
unsigned int eos_trigger_int(EOSState *s, unsigned int id, unsigned int delay);
unsigned int flash_get_blocksize(unsigned int rom, unsigned int size, unsigned int word_offset);

static void eos_load_image(EOSState *s, const char* file, int offset, int max_size, uint32_t addr, int swap_endian);

/* EOS ROM device */
/* its not done yet */
#if defined(EOS_ROM_DEVICE_IMPLEMENTED)
static void eos_rom_class_init(ObjectClass *class, void *data);
static int eos_rom_init(SysBusDevice *dev);

ROMState *eos_rom_register(hwaddr base, DeviceState *qdev, const char *name, hwaddr size,
                                BlockDriverState *bs,
                                uint32_t sector_len, int nb_blocs, int width,
                                uint16_t id0, uint16_t id1,
                                uint16_t id2, uint16_t id3, int be);
#endif

#define MEM_WRITE_ROM(addr, buf, size) \
    cpu_physical_memory_write_rom(&address_space_memory, addr, buf, size)

#endif /* HW_EOS_H */
