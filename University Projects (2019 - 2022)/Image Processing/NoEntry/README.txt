This project has been developed with the c++11 compiler.

4 programs: - face.cpp for faces detected by Viola-Jones
	    - faces_truth.cpp for detecting faces and showing the ground truth
	    - noentry_viola.cpp for detecting no entry signs using just the Viola-Jones detector
	    - noentry_hough.cpp for detecting no entry signs by applying Hough Circle Transform

Please build the project using the provided makefile. The default program compiled with make is face.cpp.
Also, note the addition of the -std=c++11.

To build one of the four files above, for example noentry_hough.cpp, it can be compiled as "make noentry_hough".

a.out can be run from the command line with the image path for the image you want to detect the object in.
There are 16 images that can be tested, NoEntry0.bmp to NoEntry15.bmp.
If you want to for example, run noentry_hough.cpp on image NoEntry1.bmp, this can be done as follows:
$ make noentry_hough
$ ./a.out 1