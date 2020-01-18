data=importdata('putty.log');
dac_codes = cell2mat(data.textdata(:,1));
dac_codes = dac_codes(:,3:6);
dac_codes = hex2dec(dac_codes);

currents = data.data(:,1);
figure(1)
plot(dac_codes,currents)
INL_source = currents;
hold on
perfect_line=(INL_source(length(INL_source))-INL_source(1))/(dac_codes(length(dac_codes))-dac_codes(1)).*((dac_codes)') + INL_source(1);
plot(dac_codes,perfect_line);
title('Output Current over DAC code')
xlabel('DAC code')
ylabel('Current (A)')
hold off

%calculate INL
figure(2)
INL=perfect_line' - INL_source(:);
plot(dac_codes,INL)
title('INL')
xlabel('DAC code')
ylabel('Current (A)')

