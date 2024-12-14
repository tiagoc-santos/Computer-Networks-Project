all: player server

server:
		$(MAKE) -C src/server

player:
		$(MAKE) -C src/player

clean:
		$(MAKE) -C src/server clean
		$(MAKE) -C src/player clean