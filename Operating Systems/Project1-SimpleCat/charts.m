%Short Matlab script to run program with different sized buffers and graph the performance

t=zeros(1,19);
for i = 1:19

    s1='./copycat -o out.txt randout.txt -b ';
    s2=int2str(2^(i-1));
    command=strcat(s1,s2);
    tic;
    [status,cmdout]=unix(command);
    t(i)=toc;
end
plot(0:18,2.2./t)
xlabel('Size of buffer ( b = 2^{(n)} )');
ylabel('Throughput (MB/sec)')
