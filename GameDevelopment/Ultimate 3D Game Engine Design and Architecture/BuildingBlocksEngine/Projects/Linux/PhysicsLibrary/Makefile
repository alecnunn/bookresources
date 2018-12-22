CC = g++ -Wall -ansi

all:
	$(CC) -c ../../../source/PointMass.cpp -I /usr/include -I ../../../source
	$(CC) -c ../../../source/PhysicsSpace.cpp -I /usr/include -I ../../../source
	$(CC) -c ../../../source/CollisionResponse.cpp -I /usr/include -I ../../../source

clean:
	@echo Cleaning up...
	@rm PointMass
	@rm PhysicsSpace
	@rm CollisionResponse
	@rm Frustum
	@rm Matrix
	@rm Octree
	@rm Plane
	@rm Quaternion
	@rm Ray
	@rm Vector
	@echo Done.
