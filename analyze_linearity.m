data=importdata('putty.log');
dactextvals = cell2mat(data.textdata(:,1));
dactextvals = dactextvals(:,3:6);
dacvals = hex2dec(dactextvals);

adctextvals = cell2mat(data.textdata(:,3));
adctextvals = adctextvals(:,3:6);
adcvals = hex2dec(adctextvals);

adcampvals = data.data;


[counts,centers] = hist(adcampvals,100);
figure(1)
bar(centers,counts);
DNL = counts/mean(counts) - 1;
bar(centers,DNL)
INL = cumsum(DNL);
bar(centers,INL)
title('INL from Histogram')
xlabel('Output Current (A)')
figure(2)
plot(adcampvals(:))
INL_source = adcampvals;
hold on
perfect_line=(INL_source(length(INL_source))-INL_source(1))/(length(dacvals)).*((1:length(dacvals))') + INL_source(1);
plot(perfect_line);
title('Output Current over DAC value')
xlabel('DAC code (starting from 0)')
ylabel('Current (A)')
hold off

%calculate INL
figure(3)
INL=perfect_line - INL_source(:);
plot(INL)
title('INL')
xlabel('DAC code (starting from 0)')
ylabel('Current (A)')

figure(4)
INL_source = adcvals;
perfect_line=(INL_source(length(INL_source))-INL_source(1))/(length(dacvals)).*((1:length(dacvals))') + INL_source(1);
INL=perfect_line - INL_source(:);
plot(INL)
title('INL normalized to LSB')
xlabel('DAC code (starting from 0)')
ylabel('ADC value')


% check for missing codes on input
for i=1:length(dacvals)-1,
    if not(dacvals(i) == (dacvals(i+1) - 1)),
        sprintf('missing code %d %x\n',i, dacvals(i))
    end
end