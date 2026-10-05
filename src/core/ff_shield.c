/**
 * ff_shield.c
 * Core logical implementation of low-overhead anti-fingerprinting overrides[cite: 1, 2].
 */
#include "ff_shield.h"
#include <string.h>

int32_t ff_core_init(ff_runtime_context_t* ctx, const ff_browser_profile_t* template_profile) {
    if (ctx == NULL || template_profile == NULL) return FF_ERR_NULL_POINTER;
    memset(ctx, 0, sizeof(ff_runtime_context_t));
    memcpy(&(ctx->active_profile), template_profile, sizeof(ff_browser_profile_t));
    
    uint64_t init_state = template_profile->base_profile_seed;
    uint64_t init_seq   = template_profile->base_profile_seed ^ 0xDA3E5D4B54713525ULL;
    ctx->crypto_prng.state = 0U;
    ctx->crypto_prng.inc = (init_seq << 1u) | 1u;
    ff_prng_generate_uint32(&(ctx->crypto_prng));
    ctx->crypto_prng.state += init_state;
    ff_prng_generate_uint32(&(ctx->crypto_prng));
    ctx->is_initialized = 1;
    ctx->blocked_telemetry_count = 0;
    return FF_STATUS_OK;
}

uint32_t ff_prng_generate_uint32(ff_pcg32_ctx_t* prng) {
    uint64_t old_state = prng->state;
    prng->state = old_state * 6364136223846793005ULL + prng->inc;
    uint32_t xorshifted = (uint32_t)(((old_state >> 18u) ^ old_state) >> 27u);
    uint32_t rot = (uint32_t)(old_state >> 59u);
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

int32_t ff_noise_inject_canvas(ff_runtime_context_t* ctx, ff_memory_buffer_t* canvas_buffer) {
    if (ctx == NULL || canvas_buffer == NULL) return FF_ERR_NULL_POINTER;
    if (!ctx->is_initialized) return FF_ERR_UNINITIALIZED;
    if (canvas_buffer->memory_ptr == NULL || canvas_buffer->data_size_bytes == 0) return FF_ERR_BOUNDS_VIOLATION;

    uint8_t* pixel_array = canvas_buffer->memory_ptr;
    size_t buffer_length = canvas_buffer->data_size_bytes;

    for (size_t index = 0; index < buffer_length; index += 4) {
        if ((index + 3) >= buffer_length) break;
        uint32_t noise_mask = ff_prng_generate_uint32(&(ctx->crypto_prng));
        uint8_t delta_noise = (uint8_t)(noise_mask & 0x01);
        pixel_array[index]     ^= delta_noise;
        pixel_array[index + 1] ^= delta_noise;
        pixel_array[index + 2] ^= delta_noise;
    }
    return FF_STATUS_OK;
}

int32_t ff_noise_inject_audio(ff_runtime_context_t* ctx, float* audio_buffer, size_t sample_count) {
    if (ctx == NULL || audio_buffer == NULL) return FF_ERR_NULL_POINTER;
    if (!ctx->is_initialized) return FF_ERR_UNINITIALIZED;
    if (sample_count == 0) return FF_ERR_BOUNDS_VIOLATION;

    for (size_t sample_idx = 0; sample_idx < sample_count; sample_idx++) {
        uint32_t prng_val = ff_prng_generate_uint32(&(ctx->crypto_prng));
        double distortion_delta = ((double)(prng_val % 200) - 100.0) / 1000000000.0;
        audio_buffer[sample_idx] += (float)distortion_delta;
        if (audio_buffer[sample_idx] > 1.0f)  audio_buffer[sample_idx] = 1.0f;
        if (audio_buffer[sample_idx] < -1.0f) audio_buffer[sample_idx] = -1.0f;
    }
    return FF_STATUS_OK;
}

int32_t ff_telemetry_inspect_packet(ff_runtime_context_t* ctx, const char* target_url, size_t url_len) {
    if (ctx == NULL || target_url == NULL) return FF_ERR_NULL_POINTER;
    if (url_len == 0 || url_len > 2048) return FF_ERR_BOUNDS_VIOLATION;

    const char* blocklist[] = {
        "google-analytics.com",
        "telemetry.browser",
        "segment.io",
        "crashlytics.com",
        "metrics.backend"
    };
    size_t blocklist_size = sizeof(blocklist) / sizeof(blocklist[0]);
    for (size_t i = 0; i < blocklist_size; i++) {
        if (strstr(target_url, blocklist[i]) != NULL) {
            ctx->blocked_telemetry_count++;
            return FF_ERR_TELEMETRY_DROP;
        }
    }
    return FF_STATUS_OK;
}

int32_t ff_core_destroy(ff_runtime_context_t* ctx) {
    if (ctx == NULL) return FF_ERR_NULL_POINTER;
    memset(ctx, 0, sizeof(ff_runtime_context_t));
    return FF_STATUS_OK;
}