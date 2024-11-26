# package plugin - deploy


pseudo code:

-- core handler function - entry point

1. start script -> manifest as input
2. read the file
3. parse the manifest
4. generate graph
5. print all nodes and paths

path 1: all success
path 2: failure 1
path 3: failure 2

6. execute:
 - start with first node on the graph - stored somewhere.
 while(not end node) {
    do action - get result
    - get next node (pass result)
 }

7. cleanup and exit.

deploy.cpp [API interface - framework to plugin communication]
apihandlers.cpp [handlers for deploy plugin exposed apis -- /prepare , /deploy, /status]
