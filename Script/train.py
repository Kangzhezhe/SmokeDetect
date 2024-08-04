import torch
import os
from torch.utils.data import Dataset, DataLoader
import numpy as np
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
os.chdir(os.path.dirname(__file__))
import torch.onnx
class CustomDataset(Dataset):
    def __init__(self, data_list, label):
        self.data_list = data_list
        self.label = label
        self.length = len(data_list)
        
    def __len__(self):
        return self.length
        
    def __getitem__(self, idx):
        data = self.data_list[idx]
        label = self.label[idx]
        return {'data': data, 'label': label}
    
def train_test_split(data_combined, label_combined,test_size=0.2):
    num_samples = data_combined.shape[0]
    split_index = int(num_samples * (1 - test_size))

    indices = np.random.permutation(num_samples)
    train_indices, test_indices = indices[:split_index], indices[split_index:]

    X_train, X_test = data_combined[train_indices], data_combined[test_indices]
    y_train, y_test = label_combined[train_indices], label_combined[test_indices]
    return X_train, X_test, y_train, y_test

def read_data(path):
    with open(path,'r') as f:
        lines = f.readlines()
    line = [l.strip().split(',') for l in lines ]
    data = np.array(line)
    data = data[:,1:].astype(np.float32)
    return data

def normalize(array):
    norms = np.linalg.norm(array, ord=2, axis=0)  # Calculate L2 norm along axis 0
    normalized_array = array / norms
    return normalized_array

def get_dataset():
    data_yes = read_data('output/yes1.txt')
    data_no = read_data('output/no1.txt')
    data_water = read_data('output/water1.txt')
   
    label_yes = torch.zeros(len(data_yes), 3)  # Three classes: yes, no, water
    label_yes[:, 0] = 1  # One-hot encoding for class 'yes'
    label_no = torch.zeros(len(data_no), 3)
    label_no[:, 1] = 1  # One-hot encoding for class 'no'
    label_water = torch.zeros(len(data_water), 3)
    label_water[:, 2] = 1  # One-hot encoding for class 'water'

    data_combined = np.concatenate((data_yes, data_no, data_water), axis=0)
    data_combined = normalize(torch.tensor(data_combined))
    label_combined = torch.cat((label_yes, label_no, label_water))
    X_train, X_test, y_train, y_test = train_test_split(data_combined, label_combined, test_size=0.2)

    dataset_train = CustomDataset(X_train, y_train)
    dataset_test = CustomDataset(X_test, y_test)

    train_loader = DataLoader(dataset_train, batch_size=32)
    test_loader = DataLoader(dataset_test, batch_size=32)

    return train_loader,test_loader

train_loader,test_loader = get_dataset()


# Define your neural network model
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

# Initialize your model
model = SimpleNN()

# Define your loss function and optimizer
criterion = nn.CrossEntropyLoss()
optimizer = optim.Adam(model.parameters(), lr=0.01)

input = torch.randn(32, 3)
out = model(input)
print(out)


def eval_model(model, test_loader):
    model.eval()
    with torch.no_grad():
        correct = 0
        total = 0
        for batch_data in test_loader:
            data, target = batch_data['data'], batch_data['label']
            output = model(data)
            
            # Convert one-hot encoded target to class indices
            target_indices = torch.argmax(target, dim=1)
            
            _, predicted = torch.max(output, 1)
            
            total += target.size(0)
            correct += (predicted == target_indices).sum().item()

        accuracy = correct / total
    return accuracy

def train():
    # Training loop
    num_epochs = 200
    for epoch in range(num_epochs):
        model.train()
        for batch_idx,batch_data in enumerate(train_loader):
            data, target = batch_data['data'], batch_data['label']
            optimizer.zero_grad()
            output = model(data)
            # import ipdb;ipdb.set_trace()
            loss = criterion(output, target)
            loss.backward()
            optimizer.step()
            
            if batch_idx % 100 == 0:
                print(f'Epoch {epoch}, Batch {batch_idx}, Loss: {loss.item()}')
                accuracy = eval_model(model, test_loader)
                print(f'Accuracy on test set: {100 * accuracy}%')
                accuracy = eval_model(model, train_loader)
                print(f'Accuracy on train set: {100 * accuracy}%')

accuracy = eval_model(model, test_loader)
print(f'Accuracy on test set: {100 * accuracy}%')

# dummy_input = torch.randn(1, 3)
# torch.onnx.export(model, dummy_input, "smoke.onnx", verbose=True)

import ipdb;ipdb.set_trace()