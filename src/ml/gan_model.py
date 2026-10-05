"""
Conditional Generative Adversarial Network (C-GAN) training loop 
for behavioral telemetry simulation (Cursor kinematics & Keystroke dynamics).
"""
import torch
import torch.nn as nn
import torch.optim as optim

class ConditioningEncoder(nn.Module):
    def __init__(self, input_dim=16, embed_dim=128):
        super(ConditioningEncoder, self).__init__()
        self.net = nn.Sequential(
            nn.Linear(input_dim, 64),
            nn.ReLU(),
            nn.Linear(64, embed_dim),
            nn.LayerNorm(embed_dim)
        )

    def forward(self, c):
        return self.net(c)


class BehavioralGenerator(nn.Module):
    def __init__(self, latent_dim=64, embed_dim=128, output_dim=4):
        super(BehavioralGenerator, self).__init__()
        self.fc = nn.Linear(latent_dim + embed_dim, 256 * 16)
        self.tconv = nn.Sequential(
            nn.ConvTranspose1d(256, 128, kernel_size=4, stride=2, padding=1),
            nn.BatchNorm1d(128),
            nn.ReLU(),
            nn.ConvTranspose1d(128, 64, kernel_size=4, stride=2, padding=1),
            nn.BatchNorm1d(64),
            nn.ReLU(),
            nn.Conv1d(64, output_dim, kernel_size=3, stride=1, padding=1),
            nn.Tanh()
        )

    def forward(self, z, c_embed):
        x = torch.cat([z, c_embed], dim=1)
        x = self.fc(x)
        x = x.view(-1, 256, 16)
        return self.tconv(x)


class BehavioralDiscriminator(nn.Module):
    def __init__(self, input_dim=4, embed_dim=128):
        super(BehavioralDiscriminator, self).__init__()
        self.conv = nn.Sequential(
            nn.Conv1d(input_dim + embed_dim, 64, kernel_size=3, stride=2, padding=1),
            nn.LeakyReLU(0.2),
            nn.Conv1d(64, 128, kernel_size=3, stride=2, padding=1),
            nn.BatchNorm1d(128),
            nn.LeakyReLU(0.2),
            nn.AdaptiveAvgPool1d(1),
            nn.Flatten(),
            nn.Linear(128, 1),
            nn.Sigmoid()
        )

    def forward(self, x, c_embed):
        batch_size, _, seq_len = x.shape
        c_expanded = c_embed.unsqueeze(2).expand(-1, -1, seq_len)
        xc = torch.cat([x, c_expanded], dim=1)
        return self.conv(xc)