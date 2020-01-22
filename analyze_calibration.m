clear

data=readcell('putty.log','FileType','text','Delimiter','\t');

index = find(strcmp(data, 'RAMP BEGIN'));
start_index = index(find(strcmp(data(index,2),'channel 1')))+2;
end_index = find(strcmp(data(start_index:length(data)), 'RAMP END'));
end_index = end_index(1) + start_index - 2;

dac_codes = cell2mat(data(start_index:end_index,1));
dac_codes = dac_codes(:,3:6);
dac_codes = hex2dec(dac_codes);

currents = cell2mat(data(start_index:end_index,4))
%%
% ignoring first and last calibration value improves uncalibrated
% linearity, this is probably the case because the DAC needs some head room
%currents = currents(2:length(currents)-1);
%dac_codes = dac_codes(2:length(dac_codes)-1);

%%
figure(1)
plot(dac_codes,currents)
INL_source = currents;
hold on
perfect_line=(INL_source(length(INL_source))-INL_source(1))/(dac_codes(length(dac_codes))-dac_codes(1)).*(dac_codes - dac_codes(1)) + INL_source(1);
plot(dac_codes,perfect_line);
title('Output current over DAC code')
xlabel('DAC code')
ylabel('Current (A)')
hold off

%calculate INL
figure(2)
INL=perfect_line - INL_source(:);
plot(dac_codes,INL)
title('INL before calibration')
xlabel('DAC code')
ylabel('Current (A)')
disp(sprintf("max INL %f",max(INL)))

