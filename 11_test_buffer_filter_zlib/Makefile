test_buffer_filter_zlib:test_buffer_filter_zlib.cpp zlib_server.cpp zlib_client.cpp
	g++ $^ -o $@ -levent
	./$@
clean:
	rm -rf test_buffer_filter_zlib
	rm -rf *.o
