test_signal: test_signal.cpp
	g++ $^ -o $@ -levent
	@echo "---------------------app start------------------------"
	./$@
clean:
	rm -rf test_server
	rm -rf *.o
