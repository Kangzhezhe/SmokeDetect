import json
import torch.nn as nn
import torch
import os
os.chdir(os.path.dirname(__file__))
class SimpleNN(nn.Module):
    def __init__(self):
        super(SimpleNN, self).__init__()
        self.seq = nn.Sequential(
            nn.Linear(3, 11),
            nn.ReLU(),
            nn.Linear(11, 3),
            nn.Softmax(dim=1)
        )

    def forward(self, x):
        x = self.seq(x)
        return x
    

model = SimpleNN()
model.load_state_dict(torch.load('smoke.pth'))


model_params = {key: value.tolist() for key, value in model.state_dict().items()}

with open('model_parameters.json', 'w') as f:
    json.dump(model_params, f)
