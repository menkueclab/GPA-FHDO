clear

data=readcell('putty.log','FileType','text','Delimiter','\t');
index = find(strcmp(data, 'RAMP BEGIN'));

for n = 1:length(index)

    start_index = index(n)+2;
    end_index = find(strcmp(data(start_index:length(data)), 'RAMP END'));
    end_index = end_index(1) + start_index - 2;
    channel_string = data(index(n),2);


    dac_codes = cell2mat(data(start_index:end_index,1));
    dac_codes = dac_codes(:,3:6);
    dac_codes = hex2dec(dac_codes);

    dacvals = cell2mat(data(start_index:end_index,2));

    adc_codes = cell2mat(data(start_index:end_index,3));
    adc_codes = adc_codes(:,3:6);
    adc_codes = hex2dec(adc_codes);

    adcampvals = cell2mat(data(start_index:end_index,4));


    [counts,centers] = hist(adc_codes,100);
    figure(4*n+1)
    bar(centers,counts);
    DNL = counts/mean(counts) - 1;
    bar(centers,DNL)
    INL = cumsum(DNL);
    bar(centers,INL)
    title(strcat('INL from Histogram',{' '},channel_string))
    xlabel('Output Current (A)')
    figure(4*n+2)
    plot(adcampvals(:))
    INL_source = adcampvals;
    hold on
    perfect_line=(INL_source(length(INL_source))-INL_source(1))/(length(dacvals)).*((1:length(dacvals))') + INL_source(1);
    plot(perfect_line);
    title(strcat('Output Current over DAC value for',{' '},channel_string))
    xlabel('DAC code (starting from 0)')
    ylabel('Current (A)')
    hold off

    %calculate INL
    figure(4*n+3)
    INL=perfect_line - INL_source(:);
    plot(INL)
    title(strcat('INL',{' '},channel_string))
    xlabel('DAC code (starting from 0)')
    ylabel('Current (A)')

    figure(4*n+4)
    INL_source = adc_codes;
    perfect_line=(INL_source(length(INL_source))-INL_source(1))/(length(dacvals)).*((1:length(dacvals))') + INL_source(1);
    INL=perfect_line - INL_source(:);
    plot(INL)
    title(strcat('INL normalized to LSB',{' '},channel_string))
    xlabel('DAC code (starting from 0)')
    ylabel('ADC code')


    % check for missing codes on input
    for i=1:length(dac_codes)-1,
        if not(dac_codes(i) == (dac_codes(i+1) - 1)),
            sprintf('missing code %d %x\n',i, dac_codes(i))
        end
    end

end