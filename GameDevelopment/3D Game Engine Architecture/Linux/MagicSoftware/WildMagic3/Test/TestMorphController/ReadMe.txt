The *.raw files have binary data.  Each file is a collection of 4-byte
quantities stored in little endian order.

FaceIndices.raw
  element[0] = triangle quantity T (int, T = 2576)
  element[1..3*T] = T triples of integers, each triple an indexed triangle

FaceTargetI.raw (0 <= I <= 4, target quantity Q = 5)
  element[0] = vertex quantity V (int, V = 1330)
  element[1..V] = V triples of floats (Vector3f)

FaceKeyI.raw (0 <= I <= 5, key quantity K = 6)
  element[0] = target quantity Q (int)
  element[1] = key time (float)
  element[2..Q+1] = weights for targets (float)
