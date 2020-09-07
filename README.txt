Documentation Author: Niko Procopi 2020

This tutorial was designed for Visual Studio 2019
If the solution does not compile, retarget the solution
to a different version of the Windows SDK. If you do not
have any version of the Windows SDK, it can be installed
from the Visual Studio Installer Tool

Welcome to the GpuTimers Tutorial!
Prerequesites: BasicVR

Press 'b' key to benchmark!
Holding 'b' while moving will cause lag, this is normal

In this tutorial, we show how to measure performance on a deeper 
level than simply looking at frame-rate. We can determine how much 
time passes on the GPU between different phases of rendering the 
frame, just like how timers can record different areas of C++ code on 
the CPU in previous tutorials

This tutorial determines how much time it takes to render the left eye, 
the right eye, and how long it takes to render the entire frame.

main.cpp lines 143-153
Create variables that we will use to measure GPU time.
We are creating "queries", which pull data from the GPU
back to the CPU. In this case, for measuring time.

main.cpp lines 206,271, 316
glQueryCounter is just like glDrawArrays, they dont
execute immediately, but they are added to a command
buffer, which all get executed when the buffer is finished.
glQueryCounter is executed on the GPU, recording time, at
different phases of rendering the frame

main.cpp lines 318-333
First thing we need to do is stall the CPU, using a "while"
loop to wait until queries are available. Next, we call 
glGetQueryObjectui64v, which freezes the GPU until the timer
data is fully transferred from GPU to CPU. When that is
finished, then the frame is finally delivered to the screen,
causing bad FPS 

Varying methods:
	Other APIs like DirectX and Vulkan have features where the
	timer data can be sent back to the CPU while new frames
	are rendering. I personally do not know if it is possible in OpenGL.
	Basically, without lag, it renders a frame, records time stamps,
	then draws 5 more frames on the screen, then prints the results
	of the first frame. Of course, Dx and Vk can also pause after each
	frame, just like this tutorial. It's useful for benchmarking fast
	animations, like explosions, frame by frame

Extra:
	Removed fullscreen mode because I'm not currently testing
	on a VR headset; if anyone wants to use a headset, feel
	free to put that back

	Reference for Timer Queries
	https://www.lighthouse3d.com/tutorials/opengl-timer-query/

	