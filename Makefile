
rt: *.cpp *.hpp
	xxd -i help.txt > usage.hpp
	g++ -O3 -flto -std=c++11 \
		camera.cpp \
		material.cpp \
		lambertian.cpp \
		metal.cpp \
		hitrecord.cpp \
		object.cpp \
		ray.cpp \
		plane.cpp \
		sphere.cpp \
		vec3.cpp \
		world.cpp \
		main.cpp \
		dielectric.cpp \
		utils.cpp -o rt