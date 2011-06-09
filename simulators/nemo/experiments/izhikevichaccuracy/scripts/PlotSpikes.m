%Extract the SpikeStream spikes from the file
spikeStrSpikeData = dlmread('C:\Users\daogamez\Home\Programs\spikestream\trunk\log\nemo_spikes_1.dat');

%Minimum neuron ID
minNeurID = min(spikeStrSpikeData(:,2));

%Scatter plot of spike stream spike data
figure;
scatter(spikeStrSpikeData(:,1), spikeStrSpikeData(:,2)-minNeurID, 'filled');
title('SpikeStream Data');


%Extract the Izhikevich spikes from the file
izhiSpikeData = dlmread('F:\New\Izhikevich\izhi_spikes_1.dat');

%Scatter plot of Izhikevich spike data
figure;
scatter(izhiSpikeData(:,1), izhiSpikeData(:,2), 'filled');
title('Izhikevich Spike Data');

%Extract the NeMo spikes from the file
nemoSpikeData = dlmread('F:\New\Izhikevich\nemo_spikes_1.dat');

%Scatter plot of NeMo spike data
figure;
scatter(nemoSpikeData(:,1), nemoSpikeData(:,2), 'filled');
title('NeMo Spike Data');
% 
% 
% %Extract the spikes from the file
% nemoSpikeStreamSpikeData = dlmread('F:\New\Izhikevich\nemo_spikestream_spikes_1.dat');
% 
% %Minimum neuron ID
% minNeurID = min(nemoSpikeStreamSpikeData(:,2));
% 
% %Scatter plot of SpikeStream in NeMo spike data
% figure;
% scatter(nemoSpikeStreamSpikeData(:,1), nemoSpikeStreamSpikeData(:,2) - minNeurID, 'filled');
% title('SpikeStream Ported to NeMo Data');
% 
% 
% %Extract the spikes from the file
% networkBuilderData = dlmread('F:\New\Izhikevich\nemo_spikestream_networkbuilder_spikes_1.dat');
% 
% %Minimum neuron ID
% minNeurID = min(networkBuilderData(:,2));
% 
% %Scatter plot of SpikeStream in NeMo spike data
% figure;
% scatter(networkBuilderData(:,1), networkBuilderData(:,2) - minNeurID, 'filled');
% title('SpikeStream NetworkBuilder ported to NeMo Data');
% 
