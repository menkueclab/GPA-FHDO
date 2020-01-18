clear

data=importdata('putty.log');
dac_codes = cell2mat(data.textdata(:,1));
dac_codes = dac_codes(:,3:6);
dac_codes = hex2dec(dac_codes);

currents = data.data(:,1);
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

