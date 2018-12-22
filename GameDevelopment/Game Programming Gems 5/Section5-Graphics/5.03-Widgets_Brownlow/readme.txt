Note From Kim Pallister: I didn't test the change, but the D3D device creation flags
in D3Dhelper.cpp could most likely be changed to D3DCREATE_MIXED_VERTEXPROCESSING. 
The multi-threaded is only needed if multiple threads plan on accessing the D3D device,
and the Mixed vertex processing should work on a wider array of hardware.
