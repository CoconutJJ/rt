
rt: *.cpp *.hpp
	g++ -O3 -flto -std=c++11 \
		camera.cpp \
		material.cpp \
		material.hpp \
		lambertian.cpp \
		lambertian.hpp \
		metal.cpp \
		metal.hpp \
		hitrecord.cpp \
		hitrecord.hpp \
		object.cpp \
		ray.cpp \
		plane.cpp \
		sphere.cpp \
		sphere.hpp \
		vec3.cpp \
		world.cpp \
		main.cpp \
		camera.hpp \
		object.hpp \
		plane.hpp \
		ray.hpp \
		vec3.hpp \
		utils.cpp \
		utils.hpp \
		world.hpp 
