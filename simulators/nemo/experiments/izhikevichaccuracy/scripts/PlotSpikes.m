%Extract the SpikeStream spikes from the file
spikeStrSpikeData = dlmread('D:\Home\Programs\spikestream\trunk\log\nemo_spikes_1.dat');

%Minimum neuron ID
minNeurID = min(spikeStrSpikeData(:,2));

%Scatter plot of spike stream spike data
scatter(spikeStrSpikeData(:,1), spikeStrSpikeData(:,2)-minNeurID, 'filled');
title('SpikeStream Data');


%Extract the Izhikevich spikes from the file
izhiSpikeData = dlmread('D:\Home\Programs\Izhikevich\izhi_spikes_1.dat');

%Scatter plot of Izhikevich spike data
figure;
scatter(izhiSpikeData(:,1), izhiSpikeData(:,2), 'filled');
title('Izhikevich Spike Data');

%Extract the NeMo spikes from the file
nemoSpikeData = dlmread('D:\Home\Programs\Izhikevich\nemo_spikes_1.dat');

%Scatter plot of NeMo spike data
figure;
scatter(nemoSpikeData(:,1), nemoSpikeData(:,2), 'filled');
title('NeMo Spike Data');


%Extract the spikes from the file
nemoSpikeStreamSpikeData = dlmread('D:\Home\Programs\Izhikevich\nemo_spikestream_spikes_1.dat');

%Minimum neuron ID
minNeurID2 = min(nemoSpikeStreamSpikeData(:,2));

%Scatter plot of SpikeStream in NeMo spike data
figure;
scatter(nemoSpikeStreamSpikeData(:,1), nemoSpikeStreamSpikeData(:,2) - minNeurID2, 'filled');
title('SpikeStream Ported to NeMo Data');
