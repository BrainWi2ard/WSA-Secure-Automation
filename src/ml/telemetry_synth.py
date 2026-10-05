"""
Inference wrapper for generating cohort-mimetic behavioral streams using the C-GAN model.
"""
import torch
from gan_model import ConditioningEncoder, BehavioralGenerator

class TelemetrySynthesizer:
    def __init__(self, model_weights_path=None, latent_dim=64, embed_dim=128):
        self.latent_dim = latent_dim
        self.encoder = ConditioningEncoder(input_dim=16, embed_dim=embed_dim)
        self.generator = BehavioralGenerator(latent_dim=latent_dim, embed_dim=embed_dim)
        
        if model_weights_path:
            checkpoint = torch.load(model_weights_path)
            self.encoder.load_state_dict(checkpoint['encoder'])
            self.generator.load_state_dict(checkpoint['generator'])
            
        self.encoder.eval()
        self.generator.eval()

    def synthesize(self, cohort_metadata: torch.Tensor, seed_tensor: torch.Tensor) -> torch.Tensor:
        with torch.no_grad():
            c_embed = self.encoder(cohort_metadata)
            z = seed_tensor * torch.randn(cohort_metadata.size(0), self.latent_dim)
            return self.generator(z, c_embed)