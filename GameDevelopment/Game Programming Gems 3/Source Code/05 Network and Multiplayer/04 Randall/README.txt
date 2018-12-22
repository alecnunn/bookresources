This source code REQUIRES STLPort >= 4.0 to compile under DevStudio.

Microsoft's included STL does not compile with warnings enabled. STLPort doesn't either, 
but has far fewer warnings than Microsoft's STL.

Some of the included code employs a hash_map. Specifically, push_sockets need it. If
you MUST compile without a modern STL, then edit push_socket.cpp and change the 
hash_maps to regular maps.

Linux builds should be fine with modern distributions.

The files are free for non-commercial use, except for publication in Gems3 which could
conceivably be considered "commercial" use of the code. In spirit, I don't care if
the code is reprinted for $$, I am more concerned about it's use in compiled form :)

Thanks!
Justin Randall
jrandall@jrlogic.dyndns.org

