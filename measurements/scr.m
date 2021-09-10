times = load('Times.bin');

for i = 1:length(times)
  times1(i) = i/10; 
end
times = times';
error = times - times1 ;

for i = 1:length(times)-1
  diff(i) = (times(i+1)-times(i)- 0.1)*1000 ;
end
figure(1);
plot(diff);
mean(diff)
