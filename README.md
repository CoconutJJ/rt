# rt: a ray tracer

Working on a ray tracer to learn about computer graphics.

Currently includes:
- Planes, Quads, Spheres
- Texture Mapping
- Lights: Point Lights, Quad Lights
- Phong Shading

Still need to add:

- Transparent materials (Glass)
- Bump/Normal Maps
- 

## How to run

To compile `rt`, run the following commands

```
$ chmod u+x BUILD.sh
$ ./BUILD.sh
$ ./bin/rt ...
```
In order to test the normal maps, I also created a tool that converts height 
maps to normal maps. It will also be compiled when you run `BUILD.sh`. To use
this conversion tool, run:
```
$ ./bin/height_map_conv ...
```

## Knowledge Resources

I first started by following this [online book](https://raytracing.github.io/),
but halfway through the second book, I felt like this book was more focused on
just implementing ray tracer instead of learning the actual concepts. So I
started doing my own thing by reading these 
[lecture notes](http://www.cs.utoronto.ca/~strider/LectureNotes.html).

## Progress

Started following Ray Tracing in One Weekend. Got metallic and diffuse spheres
working.

![](static/iter1.png)

Got texture maps working...

![](static/iter2.png)

Started doing my own thing, following the lecture notes. Implemented Phong 
illumination

![](static/iter3.png)

Applying textures, shadows and Phong lighting...

![](static/iter4.png)