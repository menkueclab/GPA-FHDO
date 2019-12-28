data=importdata('putty.log');
adccodestext = cell2mat(data.textdata(:,1));
adccodestext = adccodestext(:,3:6);
adccodes = hex2dec(adccodestext);

adcampvals = data.data;

figure(1)
plot(adcampvals)
title('Current over Time (set to 2 A)')
ylabel('Output Current (A)')
xlabel('Time (ms)')

overtemp_index = find(adcampvals<1);
figure(2)
plot(adcampvals(1:overtemp_index-1))
title('Current over Time (set to 2 A)')
ylabel('Output Current (A)')
xlabel('Time (ms)')
