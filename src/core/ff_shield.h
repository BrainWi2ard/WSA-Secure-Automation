/**
 * ff_shield.h
 * High-performance hardware masking and deterministic noise injection structures.
 * Modeled for zero-allocation performance paths inside WebCore environments.
 */
#ifndef FF_SHIELD_H
#define FF_SHIELD_H

#include <stdint.h>
#include <stddef.h>

#define FF_STATUS_OK             0
#define FF_ERR_NULL_POINTER     -1
#define FF_ERR_BOUNDS_VIOLATION -2
#define FF_ERR_OVERFLOW         -3
#define FF_ERR_UNINITIALIZED    -4
#define FF_ERR_TELEMETRY_DROP   -5

#define FF_MAX_UA_LEN          256
#define FF_MAX_LOCALE_LEN       16
#define FF_MAX_TZ_LEN           64

typedef struct {
    uint64_t state;
    uint64_t inc;
} ff_pcg32_ctx_t;

typedef struct {
    char     user_agent[FF_MAX_UA_LEN];
    char     locale[FF_MAX_LOCALE_LEN];
    char     timezone[FF_MAX_TZ_LEN];
    uint32_t screen_width;
    uint32_t screen_height;
    uint32_t hardware_concurrency;
    uint64_t device_memory_gb;
    double   latitude;
    double   longitude;
    uint64_t base_profile_seed;
} ff_browser_profile_t;

typedef struct {
    uint8_t* memory_ptr;
    size_t   data_size_bytes;
    size_t   max_capacity_bytes;
} ff_memory_buffer_t;

typedef struct {
    ff_browser_profile_t active_profile;
    ff_pcg32_ctx_t       crypto_prng;
    uint8_t              is_initialized;
    uint64_t             blocked_telemetry_count;
} ff_runtime_context_t;

int32_t ff_core_init(ff_runtime_context_t* ctx, const ff_browser_profile_t* template_profile);
uint32_t ff_prng_generate_uint32(ff_pcg32_ctx_t* prng);
int32_t ff_noise_inject_canvas(ff_runtime_context_t* ctx, ff_memory_buffer_t* canvas_buffer);
int32_t ff_noise_inject_audio(ff_runtime_context_t* ctx, float* audio_buffer, size_t sample_count);
int32_t ff_telemetry_inspect_packet(ff_runtime_context_t* ctx, const char* target_url, size_t url_len);
int32_t ff_core_destroy(ff_runtime_context_t* ctx);

#endif /* FF_SHIELD_H */