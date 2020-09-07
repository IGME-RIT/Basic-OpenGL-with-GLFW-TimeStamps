Documentation Author: Niko Procopi 2020

This tutorial was designed for Visual Studio 2019
If the solution does not compile, retarget the solution
to a different version of the Windows SDK. If you do not
have any version of the Windows SDK, it can be installed
from the Visual Studio Installer Tool

Welcome to the GpuTimers Tutorial!
Prerequesites: BasicVR

Warning: Performance is intentionally bad, the next tutorial fixes that

In this tutorial, we show how to measure performance
on a deeper level than simply looking at frame-rate.
We can determine how much time passes on the GPU between
different phases of rendering the frame, just like how
timers can record different areas of C++ code on the CPU
in previous tutorials

This tutorial determines how much time it takes
to render the left eye, the right eye, and how
long it takes to render the entire frame.

When you run the sample, you can see in the console
window that the frame is rendering quickly, but the
frame-rate will still drop. This is normal.

main.cpp lines 143-153
Create variables that we will use to measure GPU time.
We are creating "queries", which pull data from the GPU
back to the CPU. In this case, for measuring time.

main.cpp lines 199, 261, 303
glQueryCounter is just like glDrawArrays, they dont
execute immediately, but they are added to a command
buffer, which all get executed when the buffer is finished.
glQueryCounter is executed on the GPU, recording time, at
different phases of rendering the frame

main.cpp lines 305-320   <-- this is why performance is bad
First thing we need to do is stall the CPU, using a "while"
loop to wait until queries are available. Next, we call 
glGetQueryObjectui64v, which freezes the GPU until the timer
data is fully transferred from GPU to CPU. When that is
finished, then the frame is finally delivered to the screen,
causing bad FPS 

In the next tutorial, we use GPU timers without causing
the GPU to freeze, allowing for high FPS while debugging. 
However, there is a trade-off...

Extra:
	Put "#if 0" around the code that uses '1' and '2' buttons
	for calibration, becuase I dont need it, but if anyone 
	wants to use it in the future, it is there

	Removed fullscreen mode because I'm not currently testing
	on a VR headset; if anyone wants to use a headset, feel
	free to put that back

	Reference for Timer Queries
	https://www.lighthouse3d.com/tutorials/opengl-timer-query/

	