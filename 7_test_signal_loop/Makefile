test_signal_loop: test_signal_loop.cpp
	g++ $^ -o $@ -levent
	@echo "---------------------app start------------------------"
	./$@
clean:
	rm -rf test_server
	rm -rf *.o
