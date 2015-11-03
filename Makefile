all: mcp

mcp: mcp.c 
	gcc mcp.c -lprocps -o mcp 

clean:
	rm mcp
