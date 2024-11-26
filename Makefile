
rt: *.cpp *.hpp
	xxd -i help.txt > usage.hpp
	g++ -O3 -flto -std=c++20 \
		texture.cpp \
		solid_texture.cpp \
		image_texture.cpp \
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
		phong.cpp \
		light.cpp \
		utils.cpp -o rt