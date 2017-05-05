for j in {1..15}
    do
        for i in {1..10}
        do
            echo "Running on $j threads"
            echo -n "$j " >> timevthreads.txt
            ./main.out ../data/autosokoban/5/1.txt $j >> tests.txt
        done
    done
