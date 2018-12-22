dist:
	rm -f MJ_dist.tar.gz
	tar cvf MJ_dist.tar audio/* images/* tiles/* \
		common/*.class common/*.java \
		client/*.class client/*.java \
		types/*.class types/*.java \
		server/*.class server/*.java \
		README COPYRIGHT Makefile copyright.html \
		mj.html mj_rules.html mj_ims.html mj_new.html \
		start_server stop_server
	gzip MJ_dist.tar

all:
	CLASSPATH=`pwd`:$$CLASSPATH; export CLASSPATH; javac common/*.java
	CLASSPATH=`pwd`:$$CLASSPATH; export CLASSPATH; javac types/*.java
	CLASSPATH=`pwd`:$$CLASSPATH; export CLASSPATH; javac server/*.java
	CLASSPATH=`pwd`:$$CLASSPATH; export CLASSPATH; javac client/*.java

clean:
	rm -f common/*.class
	rm -f types/*.class
	rm -f server/*.class
	rm -f client/*.class

