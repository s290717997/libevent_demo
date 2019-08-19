test_server: test_conf.cpp
	g++ $^ -o $@ -levent
	@echo "---------------------app start------------------------"
	./$@
clean:
	rm -rf test_server
	rm -rf *.o
