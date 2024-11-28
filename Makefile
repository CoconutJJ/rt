
rt: **/*.cpp include/*.hpp
	xxd -i help.txt > usage.hpp
	g++ -O3 -flto -std=c++20 -Iinclude/ \
		texture/texture.cpp \
		texture/solid_texture.cpp \
		texture/image_texture.cpp \
		world/camera.cpp \
		world/world.cpp \
		material/material.cpp \
		material/phong.cpp \
		material/lambertian.cpp \
		material/metal.cpp \
		material/dielectric.cpp \
		object/object.cpp \
		object/plane.cpp \
		object/sphere.cpp \
		object/quad.cpp \
		light/light.cpp \
		light/quad_light.cpp \
		light/point_light.cpp \
		hitrecord.cpp \
		ray.cpp \
		vec3.cpp \
		main.cpp \
		utils.cpp -o rt