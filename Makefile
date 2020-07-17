all: producer.cpp consumer.cpp protocol.h
	g++ -std=c++11 -Wall -Wextra -Werror producer.cpp -o producer -lrt
	g++ -std=c++11 -Wall -Wextra -Werror consumer.cpp -o consumer -lrt

clean:
	rm -rf producer consumer
