function print_time
    while /bin/true
        printf "%s\r" (date +%s.%N)
    end
end
