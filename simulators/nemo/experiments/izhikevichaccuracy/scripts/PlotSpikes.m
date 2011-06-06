%Extract the SpikeStream spikes from the file
spikeStrSpikeData = dlmread('../../../../log/nemo_spikes_1.dat');

%Minimum neuron ID
minNeurID = min(spikeStrSpikeData(:,2));

%Scatter plot of spike stream spike data
scatter(spikeStrSpikeData(:,1), spikeStrSpikeData(:,2)-minNeurID, 'filled');


%Extract the Izhikevich spikes from the file
izhiSpikeData = dlmread('C:\Users\Taropeg\Home\Programs\Izhikevich\debug\spikes_1.dat');

%Scatter plot of Izhikevich spike data
figure;
scatter(izhiSpikeData(:,1), izhiSpikeData(:,2), 'filled');
