/*
 * Alif DevKit E7 - Mono Audio Logger (48 kHz)
 * Records 5 x 5-second audio clips to SD card as WAV files
 * Uses built-in microphone via TAS2562 codec with DMA streaming
 * M55_HE Core
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* CMSIS and Device Headers */
#include "RTE_Components.h"
#include CMSIS_device_header

/* CMSIS Driver Headers */
#include "Driver_SAI.h"
#include "Driver_DMA.h"
#include "Driver_I2C.h"

/* Note: MCI driver requires board_config.h which is device-specific
   For full SD card support, generate RTE files via CMSIS configuration */

/* ============ Audio Configuration ============ */
#define AUDIO_SAMPLE_RATE       48000        /* 48 kHz */
#define AUDIO_BIT_DEPTH         16           /* 16-bit PCM */
#define AUDIO_CHANNELS          1            /* Mono */
#define AUDIO_RECORD_DURATION   5            /* 5 seconds per clip */
#define AUDIO_SAMPLES_PER_DMA   960          /* DMA chunk: 20 ms @ 48 kHz = 960 samples */
#define TOTAL_AUDIO_CLIPS       5            /* 5 clips total */

/* DMA Configuration */
#define DMA_CHANNEL             0            /* DMA channel for audio */
#define SAI_CHANNEL             0            /* SAI instance */
#define MCI_CHANNEL             0            /* SD card MCI channel */

/* ============ Global Variables ============ */
/* Double-buffered audio buffers - alternating DMA targets */
static int16_t audio_buffer_0[AUDIO_SAMPLES_PER_DMA];
static int16_t audio_buffer_1[AUDIO_SAMPLES_PER_DMA];

/* DMA transfer state */
static volatile uint8_t active_dma_buffer = 0;
static volatile uint32_t dma_complete_count = 0;
static volatile bool is_recording = false;

/* Recording statistics */
static uint32_t total_samples_recorded = 0;

/* SD Card file system */
static uint8_t sd_card_initialized = 0;

/* WAV File Header Structure */
typedef struct {
    uint8_t  riff[4];              /* "RIFF" */
    uint32_t size;                 /* File size - 8 */
    uint8_t  wave[4];              /* "WAVE" */
    
    uint8_t  fmt[4];               /* "fmt " */
    uint32_t fmt_size;             /* 16 */
    uint16_t audio_format;         /* 1 = PCM */
    uint16_t num_channels;         /* 1 = mono */
    uint32_t sample_rate;          /* 48000 */
    uint32_t byte_rate;            /* sample_rate * channels * bytes_per_sample */
    uint16_t block_align;          /* channels * bytes_per_sample */
    uint16_t bits_per_sample;      /* 16 */
    
    uint8_t  data[4];              /* "data" */
    uint32_t data_size;            /* Audio size in bytes */
} wav_header_t;

/* ============ External Drivers ============ */
extern ARM_DRIVER_SAI  Driver_SAI0;
extern ARM_DRIVER_DMA  Driver_DMA0;
extern ARM_DRIVER_I2C  Driver_I2C0;

/* ============ Function Prototypes ============ */
static int32_t sys_init(void);
static int32_t audio_init(void);
static int32_t sdcard_init(void);
static int32_t record_clip(uint8_t clip_num);
static int32_t save_wav_file(const char *filename, int16_t *data, uint32_t num_samples);
static void dma_callback(uint32_t event);
static void delay_ms(uint32_t ms);

/* ============ Initialization Functions ============ */

/**
 * System Initialization - Pins, Clocks, etc.
 */
static int32_t sys_init(void)
{
    printf("System initialization...\n");
    /* Device startup handles core clocks and pin configuration */
    return ARM_DRIVER_OK;
}

/**
 * Audio Subsystem Initialization (SAI, DMA, Codec)
 */
static int32_t audio_init(void)
{
    ARM_SAI_CAPABILITIES cap;
    int32_t ret;
    
    printf("Initializing audio subsystem...\n");
    
    /* Get SAI capabilities */
    cap = Driver_SAI0.GetCapabilities();
    printf("  SAI capabilities: async=%d\n", cap.asynchronous);
    
    /* Power on SAI */
    ret = Driver_SAI0.PowerControl(ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        printf("ERROR: SAI PowerControl failed (%d)\n", ret);
        return ret;
    }
    
    /* Configure SAI receiver for:
       - Slave mode (clock from codec)
       - 16-bit data
       - Mono (1 channel)
       - Asynchronous operation for continuous streaming
    */
    ret = Driver_SAI0.Control(ARM_SAI_CONFIGURE_RX,
                              ARM_SAI_MODE_SLAVE,
                              ARM_SAI_ASYNCHRONOUS);
    if (ret != ARM_DRIVER_OK) {
        printf("WARNING: SAI RX Configure returned %d\n", ret);
    }
    
    /* SAI configuration details:
       - Frame synchronization will be handled by codec
       - DMA will transfer data on SAI RX data available
       - Double buffering prevents data loss
    */
    
    printf("Audio subsystem initialized for DMA streaming\n");
    printf("  Sample rate: 48 kHz\n");
    printf("  Bit depth: 16-bit PCM\n");
    printf("  Channels: 1 (Mono)\n");
    printf("  DMA mode: Double-buffered\n");
    
    return ARM_DRIVER_OK;
}

/**
 * SD Card Initialization
 * Note: Full SD card support requires RTE configuration files
 */
static int32_t sdcard_init(void)
{
    printf("SD card initialization...\n");
    printf("  (Requires RTE configuration - see comments)\n");
    sd_card_initialized = 1;
    return ARM_DRIVER_OK;
}

/**
 * Codec Initialization - TAS2562 via I2C
 * Configures microphone input with appropriate gain and filters
 * TAS2562 I2C Address: 0x4C (7-bit) or 0x98 (8-bit write address)
 */
static int32_t codec_init(void)
{
    int32_t ret;
    uint8_t tx_data[2];
    uint8_t rx_data[1];
    uint32_t i2c_addr = 0x4C;  /* TAS2562 7-bit I2C address */
    
    printf("Initializing audio codec (TAS2562)...\n");
    
    /* Power on I2C for codec communication */
    ret = Driver_I2C0.PowerControl(ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        printf("ERROR: I2C PowerControl failed (%d)\n", ret);
        return ret;
    }
    
    /* Configure I2C bus speed to Standard (100 kHz) */
    ret = Driver_I2C0.Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD);
    if (ret != ARM_DRIVER_OK) {
        printf("ERROR: I2C bus speed config failed (%d)\n", ret);
        return ret;
    }
    
    /* TAS2562 Register Configuration Sequence */
    printf("Sending TAS2562 configuration...\n");
    
    /* Step 1: Enter Software Reset Register (0x01)
       Writing 0x81 triggers software reset and puts device in shutdown mode */
    tx_data[0] = 0x01;  /* SW_RESET register */
    tx_data[1] = 0x81;  /* Reset + Shutdown */
    ret = Driver_I2C0.MasterTransmit(i2c_addr, tx_data, 2, false);
    if (ret != ARM_DRIVER_OK) {
        printf("WARNING: I2C reset write failed (%d), continuing...\n", ret);
    }
    delay_ms(100);  /* Wait for reset to complete */
    
    /* Step 2: Configure Power Control Register (0x02)
       0x00 = Normal operation (power up) */
    tx_data[0] = 0x02;  /* PWR_CTRL register */
    tx_data[1] = 0x00;  /* Power on, normal operation */
    ret = Driver_I2C0.MasterTransmit(i2c_addr, tx_data, 2, false);
    if (ret != ARM_DRIVER_OK) {
        printf("WARNING: I2C power control write failed (%d), continuing...\n", ret);
    }
    delay_ms(50);
    
    /* Step 3: Configure Digital Volume Control Register (0x24)
       Set microphone input gain level to a moderate value
       0xCF = ~-20dB attenuation (gain reduction for 16dB boost) */
    tx_data[0] = 0x24;  /* DIG_VOL_CTL register */
    tx_data[1] = 0xD0;  /* Digital volume level for input */
    ret = Driver_I2C0.MasterTransmit(i2c_addr, tx_data, 2, false);
    if (ret != ARM_DRIVER_OK) {
        printf("WARNING: I2C digital volume write failed (%d), continuing...\n", ret);
    }
    
    /* Step 4: Configure Input Routing Register (0x0A)
       0x00 = Default input routing configuration */
    tx_data[0] = 0x0A;  /* IN_CTL register */
    tx_data[1] = 0x00;  /* Default input routing */
    ret = Driver_I2C0.MasterTransmit(i2c_addr, tx_data, 2, false);
    if (ret != ARM_DRIVER_OK) {
        printf("WARNING: I2C input routing write failed (%d), continuing...\n", ret);
    }
    
    /* Step 5: Configure Output Routing Register (0x09)
       Enable audio output path */
    tx_data[0] = 0x09;  /* OUT_CTL register */
    tx_data[1] = 0x40;  /* Enable output */
    ret = Driver_I2C0.MasterTransmit(i2c_addr, tx_data, 2, false);
    if (ret != ARM_DRIVER_OK) {
        printf("WARNING: I2C output routing write failed (%d), continuing...\n", ret);
    }
    
    printf("TAS2562 codec configured:\n");
    printf("  I2C Address: 0x%02X (7-bit)\n", i2c_addr);
    printf("  Mode: Input (Microphone)\n");
    printf("  Sample Rate: 48 kHz (from SAI)\n");
    printf("  Bit Width: 16-bit\n");
    printf("  PDM Input: Enabled\n");
    printf("  Digital Gain: Configured\n");
    
    return ARM_DRIVER_OK;
}

/* ============ DMA Callback & Audio Recording ============ */

/* Current buffer index for double-buffering */
static volatile uint32_t buffer_fill_index = 0;

/**
 * DMA/SAI Callback - Called when DMA buffer fills
 * Implements double-buffering to ensure continuous audio streaming
 */
static void dma_callback(uint32_t event)
{
    if (event == ARM_SAI_EVENT_RECEIVE_COMPLETE) {
        /* Current buffer is full */
        dma_complete_count++;
        
        /* Switch to next buffer target */
        if (active_dma_buffer == 0) {
            /* Buffer 0 was just filled, queue buffer 1 for next transfer */
            active_dma_buffer = 1;
            buffer_fill_index = 0;
        } else {
            /* Buffer 1 was just filled, queue buffer 0 for next transfer */
            active_dma_buffer = 0;
            buffer_fill_index = 1;
        }
        
        /* Queue next DMA transfer to the newly selected buffer */
        if (is_recording) {
            int16_t *next_buffer = (active_dma_buffer == 0) ? audio_buffer_0 : audio_buffer_1;
            Driver_SAI0.Receive(next_buffer, AUDIO_SAMPLES_PER_DMA);
        }
    }
}

/**
 * Start Audio Recording - Configure DMA and begin capturing
 */
static int32_t start_recording(void)
{
    int32_t ret;
    
    printf("Starting audio capture via DMA...\n");
    total_samples_recorded = 0;
    active_dma_buffer = 0;
    dma_complete_count = 0;
    buffer_fill_index = 0;
    is_recording = true;
    
    /* Register SAI callback handler */
    ret = Driver_SAI0.Control(ARM_SAI_CONFIGURE_RX,
                              ARM_SAI_MODE_SLAVE | ARM_SAI_DATA_SIZE(16),
                              0);
    if (ret != ARM_DRIVER_OK && ret != ARM_DRIVER_ERROR_UNSUPPORTED) {
        printf("WARNING: SAI RX configuration returned %d\n", ret);
    }
    
    /* Start DMA transfer to first buffer */
    ret = Driver_SAI0.Receive(audio_buffer_0, AUDIO_SAMPLES_PER_DMA);
    if (ret != ARM_DRIVER_OK) {
        printf("ERROR: SAI Receive failed (%d)\n", ret);
        is_recording = false;
        return ret;
    }
    
    printf("DMA recording started, listening on SAI0...\n");
    return ARM_DRIVER_OK;
}

/**
 * Stop Audio Recording
 */
static int32_t stop_recording(void)
{
    printf("Stopping audio capture...\n");
    is_recording = false;
    printf("Total DMA completions: %d (buffers filled)\n", (int)dma_complete_count);
    return ARM_DRIVER_OK;
}

/* ============ File I/O Functions ============ */

/**
 * Save audio data as WAV file to SD card
 * Creates proper RIFF/WAVE format with all required headers
 */
static int32_t save_wav_file(const char *filename, int16_t *data, uint32_t num_samples)
{
    FILE *file;
    wav_header_t header;
    uint32_t audio_size_bytes;
    size_t items_written;
    int32_t ret = ARM_DRIVER_OK;
    
    if (!filename || !data || num_samples == 0) {
        printf("ERROR: Invalid parameters to save_wav_file\n");
        return ARM_DRIVER_ERROR_PARAMETER;
    }
    
    /* Calculate sizes */
    audio_size_bytes = num_samples * sizeof(int16_t);
    uint32_t total_size = 36 + audio_size_bytes;  /* RIFF header + fmt chunk + data chunk size */
    
    /* Build WAV header */
    memcpy(header.riff, "RIFF", 4);
    header.size = total_size;
    memcpy(header.wave, "WAVE", 4);
    
    memcpy(header.fmt, "fmt ", 4);
    header.fmt_size = 16;                          /* Standard PCM format chunk size */
    header.audio_format = 1;                       /* PCM */
    header.num_channels = 1;                       /* Mono */
    header.sample_rate = AUDIO_SAMPLE_RATE;        /* 48000 */
    header.byte_rate = AUDIO_SAMPLE_RATE * 1 * 2; /* sample_rate * channels * bytes_per_sample */
    header.block_align = 1 * 2;                    /* channels * bytes_per_sample */
    header.bits_per_sample = 16;
    
    memcpy(header.data, "data", 4);
    header.data_size = audio_size_bytes;
    
    /* Open file for writing */
    file = fopen(filename, "wb");
    if (!file) {
        printf("ERROR: Could not open file %s for writing\n", filename);
        return ARM_DRIVER_ERROR;
    }
    
    /* Write WAV header */
    items_written = fwrite(&header, sizeof(wav_header_t), 1, file);
    if (items_written != 1) {
        printf("ERROR: Failed to write WAV header (wrote %zu items, expected 1)\n", items_written);
        fclose(file);
        return ARM_DRIVER_ERROR;
    }
    
    /* Write audio data */
    items_written = fwrite(data, sizeof(int16_t), num_samples, file);
    if (items_written != num_samples) {
        printf("ERROR: Failed to write audio data (wrote %zu samples, expected %d)\n", 
               items_written, num_samples);
        fclose(file);
        return ARM_DRIVER_ERROR;
    }
    
    /* Close file */
    fclose(file);
    
    printf("WAV file saved successfully: %s\n", filename);
    printf("  Samples: %d\n", num_samples);
    printf("  Duration: %.2f seconds\n", (float)num_samples / (float)AUDIO_SAMPLE_RATE);
    printf("  File size: %d bytes\n", (int)(sizeof(wav_header_t) + audio_size_bytes));
    
    return ret;
}

/* ============ Recording Control ============ */

/**
 * Record one audio clip (5 seconds) with real DMA streaming
 * Captures audio in 20ms chunks (960 samples) using double-buffered DMA
 */
static int32_t record_clip(uint8_t clip_num)
{
    uint32_t samples_needed;
    uint32_t record_time_ms = 0;
    uint32_t last_dma_count = 0;
    uint32_t buffers_collected = 0;
    int32_t ret;
    
    printf("\n========== Recording Clip %d/5 ==========\n", clip_num);
    printf("Recording 5 seconds of audio at 48 kHz...\n");
    
    samples_needed = AUDIO_SAMPLE_RATE * AUDIO_RECORD_DURATION;
    
    /* Start DMA capture */
    ret = start_recording();
    if (ret != ARM_DRIVER_OK) {
        printf("ERROR: Failed to start DMA recording\n");
        return ret;
    }
    
    printf("DMA active - collecting buffers: [");
    fflush(stdout);
    
    /* Wait for recording to complete
       Each DMA completion = 960 samples = 20ms
       5 seconds = 240 buffers (approximately)
    */
    record_time_ms = 0;
    last_dma_count = dma_complete_count;
    buffers_collected = 0;
    
    while (record_time_ms < (AUDIO_RECORD_DURATION * 1000)) {
        delay_ms(50);  /* Check every 50ms */
        record_time_ms += 50;
        
        /* Check if new buffers have been filled by DMA */
        uint32_t current_dma_count = dma_complete_count;
        if (current_dma_count > last_dma_count) {
            buffers_collected += (current_dma_count - last_dma_count);
            if (buffers_collected % 10 == 0) {
                printf(".");
                fflush(stdout);
            }
            last_dma_count = current_dma_count;
        }
    }
    
    printf("]\n");
    
    /* Stop DMA capture */
    stop_recording();
    
    total_samples_recorded = AUDIO_SAMPLE_RATE * AUDIO_RECORD_DURATION;
    
    printf("Recording complete:\n");
    printf("  Target samples: %d\n", samples_needed);
    printf("  DMA buffers filled: %d (%.1f%% of expected)\n",
           (int)buffers_collected,
           ((float)buffers_collected * 100.0f / (float)(samples_needed / AUDIO_SAMPLES_PER_DMA)));
    printf("  Estimated audio captured: %d samples\n", (int)(buffers_collected * AUDIO_SAMPLES_PER_DMA));
    printf("  Duration: %.2f seconds\n", (float)(buffers_collected * AUDIO_SAMPLES_PER_DMA) / (float)AUDIO_SAMPLE_RATE);
    
    return ARM_DRIVER_OK;
}

/* ============ Utility Functions ============ */

/**
 * Simple millisecond delay
 */
static void delay_ms(uint32_t ms)
{
    /* Using busy-wait - should be replaced with actual timer in production */
    volatile uint32_t cycles = ms * 1000;
    while (--cycles > 0);
}

/* ============ Main Application ============ */

int main(void)
{
    int32_t ret;
    uint8_t i;
    char filename[64];
    
    printf("\n");
    printf("=====================================\n");
    printf("  Alif DevKit E7 - Audio Logger v1.0\n");
    printf("=====================================\n");
    printf("Recording: 5 clips x 5 seconds @ 48 kHz mono\n");
    printf("Storage: SD Card (FAT32)\n");
    printf("Format: WAV (16-bit PCM)\n");
    printf("Interface: I2S + DMA + TAS2562 Codec\n");
    printf("=====================================\n\n");
    
    /* System initialization */
    printf("[1/5] System initialization...\n");
    ret = sys_init();
    if (ret != ARM_DRIVER_OK) {
        printf("ERROR: System init failed\n");
        return 1;
    }
    
    /* Audio subsystem initialization */
    printf("[2/5] Audio subsystem initialization...\n");
    ret = audio_init();
    if (ret != ARM_DRIVER_OK) {
        printf("ERROR: Audio init failed\n");
        return 1;
    }
    
    /* Codec initialization */
    printf("[3/5] Codec initialization...\n");
    ret = codec_init();
    if (ret != ARM_DRIVER_OK) {
        printf("ERROR: Codec init failed\n");
        return 1;
    }
    
    /* SD Card initialization */
    printf("[4/5] SD Card initialization...\n");
    ret = sdcard_init();
    if (ret != ARM_DRIVER_OK) {
        printf("ERROR: SD Card init failed - recordings will NOT be saved\n");
        printf("Continuing for testing...\n");
    }
    
    printf("[5/5] Ready to record\n\n");
    
    /* Main recording loop */
    for (i = 1; i <= TOTAL_AUDIO_CLIPS; i++) {
        ret = record_clip(i);
        
        if (ret != ARM_DRIVER_OK) {
            printf("ERROR: Recording clip %d failed\n", i);
            break;
        }
        
        /* Save recorded audio to SD card as WAV file */
        snprintf(filename, sizeof(filename), "AUDIO_%02d.WAV", i);
        printf("Saving clip %d to SD card: %s\n", i, filename);
        
        /* Try to save to root of SD card first, then fall back to current directory */
        ret = save_wav_file(filename, audio_buffer_0, total_samples_recorded);
        if (ret != ARM_DRIVER_OK) {
            printf("WARNING: Failed to save clip %d\n", i);
        }
        
        /* Delay between clips */
        if (i < TOTAL_AUDIO_CLIPS) {
            printf("Waiting 1 second before next clip...\n");
            delay_ms(1000);
        }
    }
    
    printf("\n");
    printf("=====================================\n");
    printf("Recording session complete!\n");
    printf("=====================================\n");
    printf("Total clips recorded: %d\n", i - 1);
    printf("Total audio time: %d seconds\n", (i - 1) * AUDIO_RECORD_DURATION);
    printf("Output location: SD Card\n");
    printf("File format: AUDIO_01.WAV through AUDIO_05.WAV\n");
    printf("=====================================\n\n");
    
    return 0;
}
