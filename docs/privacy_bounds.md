# Differential Privacy & Noise Calibration Bounds

## Differential Privacy Noise Calibration ($\sigma$)
To prevent cross-site correlation against historical baselines, zero-mean Gaussian noise is injected into runtime behavioral and hardware telemetry vectors within the unprivileged user-space container runtime.

For cursor velocity telemetry with an $L_2$-sensitivity $\Delta_2 f = 4.2 \text{ px/ms}$, targeting privacy parameters $\epsilon = 0.1$ and $\delta = 10^{-6}$:

$$\sigma = \frac{\Delta_2 f \cdot \sqrt{2 \ln(1.25 / \delta)}}{\epsilon} = \frac{4.2 \cdot \sqrt{2 \ln(1.25 / 10^{-6})}}{0.1} \approx 222.55 \text{ px/ms}$$

## Runtime System Integration Metrics
* **PCG32 PRNG State Size**: 128-bit combined state and increment registers for deterministic profile stability.
* **Canvas Dither Shift**: 1-bit low-order bit flipping on RGB color channels (alpha channel constant) via `ff_noise_inject_canvas()`.
* **WebAudio Dither Band**: $\pm 100 / 10^9$ amplitude scalar variance mapping via `ff_noise_inject_audio()`.
