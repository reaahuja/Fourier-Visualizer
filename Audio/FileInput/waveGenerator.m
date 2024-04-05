frequency = 75;
samplingRate = 8000; 
duration = 5;


t = 0:(1/samplingRate):(duration-1/samplingRate);

% generate sin and plot
sineWave = sin(2*pi*frequency*t);
plot(t, sineWave);
xlabel('Time');
ylabel('Amplitude');
title('75 Hz sine sampled @ 8000 Hz');

N = length(sineWave); 
fftResult = fft(sineWave);
f = sampling_rate*(0:(N/2))/N;

% generate spectrum 
transform = 2*abs(fftResult(1:N/2+1))/N; % take up to half sampling rate (nyquist frequency)
plot(f, magnitude_spectrum);
xlabel('Frequency');
ylabel('Magnitude');
title('Magnitude of 75 Hz sine sampled @ 8000 Hz');

% put sine samples into formatted array 
fid = fopen('transform.txt', 'w');
fprintf(fid, "{");
length(transform)
for i = 1:length(transform)
    fprintf(fid, "%0.6f", transform(i));
    if i < length(transform)
        fprintf(fid, ", ");
    end
end
fprintf(fid, "}");
fclose(fid);

% put sine samples into formatted array 
fid = fopen('sine.txt', 'w');
fprintf(fid, "{");
length(sineWave)
for i = 1:length(sineWave)
    fprintf(fid, "%0.6f", sineWave(i));
    if i < length(sineWave)
        fprintf(fid, ", ");
    end
end
fprintf(fid, "}");
fclose(fid);


