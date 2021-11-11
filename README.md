## SwarmCon - marker-based localization system for swarm robotics

### What is SwarmCon ?
SwarmCon is a minimalistic version of the <a href="http://www.youtube.com/watch?v=KgKrN8_EmUA">WhyCon</a> localization system intended for swarm applications.
While the core of the <i>SwarmCon</i> system is the same as the <i>WhyCon</i> one, it was designed to have as least dependencies as possible.
Moreover, the <i>SwarmCon</i> is especially intended for external localization of ground-robot swarms.
Thus, unlike <i>WhyCon</i>, <i>SwarmCon</i> can distinguish between individual robots and calculate their heading.

##### This is a fork of SwarmCon for Tampere University Networked Robotics Lab

### Setting up SwarmCon

This repo features a simple install script `./setup.sh` for ubuntu/debian based linux distributions.

#### Dependencies

All the following libraries are probably in your packages.

1. **libsdl1.2-dev** for graphical user interface.
2. **libsdl-ttf2.0-dev** to print stuff in the GUI.
3. **libncurses5-dev** to print stuff on the terminal.
4. **luvcview** or **guvcview** to set-up the camera.

### Usage

#### Changing parameters
Most common parameters should be changed by editing `src/config.h` and recompiling the program

#### Compiling
```
$ cd src
$ make
```

#### Flags
The program has a few flags to change its behaviour
- `--nogui`: don't show the GUI
- `--savevideo`: save video
- `--savelog`: save log

<hr>

#### Quick setup for initial testing

1. Install the required [libraries](#Dependencies).
2. Check the available resolutions, framerates and formats of your USB camera by running **guvcview -L**.
3. Run **guvcview** and adjust your camera settings (exposure, brightness etc).
4. Download the software from GitHub and go to the **src** directory.
5. Adjust the camera resolution in the file **config.h**.
6. Compile the software - just type **make**.
7. Download, resize and print one circular <a href="etc/test.pdf">pattern</a>
8. Try a test run - you need to run the binary from the **bin** directory. Run `./swarmcon /dev/videoX 1`, where X is the number of the camera and 1 tells the system to track one pattern.</li>
9. You should see the image with some numbers below the circle. Pressing **D** shows the segmentation result.
10. At this point, you can also change camera brightness, exposure, contrast by pressing <i>(SHIFT) b, e, c</i> respectively. These settings are stored in <i>etc/camera.cfg</i> and reloaded on restart.
11. Open your browser to view localhost:6666. You should see the circle position.

#### Advanced setup for more robots in a rectangular field

1. Calibrate your camera using the MATLAB (or Octave) calibration toolbox and put the **Calib_Results.m** in the **etc** directory. (this step is not necessary, but it improves localization precision)
2. Go to the **etc** directory and call `create.sh N M`to generate patterns for **NxM** robots.
3. Print the generated file **pattern_n_m.pdf**, put the eliptical markers on your robots and place the remaining four circular markers at the corners of their operation space.
4. Modify the dimensions of the operation space in the **config.h**.
5. If you have resized the markers (their default size is 30mm), then adjust the their diameter in the **config.h** file.
6. Run `make` to recompile, put your camera facing down, overhead your swarm.
6. Go to **bin** directory and run ``./swarmcon /dev/videoX Y``, where X is the number of your camera and Y is the number of patterns you want to track, i.e. `Y = (N * M) + 4`.
7. Once all the patterns are found, press **a** and the four outermost patterns will be used to calculate the coordinate system.
8. Each pattern will have four numbers - ID, heading and x,y in mm.
9. Pressing **+**,**-** changes the number of localized patterns.

#### To postprocess the stored videos

1. To create a log of the robot positions, simply run **swarmcon** with the flag `--savelog`.
2. If your camera supports the MJPEG format, then the system can create a video in the **output** folder with the flag `--savevideo`.
3. If your camera does not support MJPEG, **swarmcon** will save the video feed as a serie of bitmaps, that you can process later as well.
4. You can run **swarmcon video_file_name Y** to process that video in the same way as when using the camera, i.e. **video_file_name** instead of **/dev/videoX**.
5. Processing a saved video rather than the camera feed is likely to provide more precise results.
6. Running the system with a **--nogui** argument e.g. `./swarmcon /dev/video0 1 --nogui` causes text-only output - this can speed-up postprocessing.

#### Some additional remarks

- At this point, you can start experimenting with the syste by adding whatever features you might think useful.
- We have tried to comment the code so an experienced programmer should be able to alter the system accordingly. However, if you have any questions regarding the code, feel free to contact [Tom Krajnik](http://scholar.google.co.uk/citations?user=Qv3nqgsAAAAJ&hl=en&oi=ao) or [Matias Nitsche](https://scholar.google.co.uk/citations?user=Z0hQoRUAAAAJ&hl=en&oi=ao). **The Code has been modified, these are only the original authors and they might not be able to answer**
- If you use this localization system for your research, please don't forget to cite at least one relevant paper from these [bibtex](http://raw.githubusercontent.com/wiki/gestom/CosPhi/papers/WhyCon.bib) records.
- Have fun!
</ol>

<hr>

### Who did it ?
The <i>WhyCon</i> system was developed as a joint project between the University of Buenos Aires, Czech Technical University and University of Lincoln in UK.
The main contributors were [Matias Nitsche](https://scholar.google.co.uk/citations?user=Z0hQoRUAAAAJ&hl=en&oi=ao), [Tom Krajnik](http://scholar.google.co.uk/citations?user=Qv3nqgsAAAAJ&hl=en&oi=ao) and [Jan Faigl](https://scholar.google.co.uk/citations?user=-finD_sAAAAJ&hl=en).
<i>WhyCon</i> was first presented on International Conference on Advanced Robotics 2013 [[1](#references)], later in the Journal of Intelligent and Robotics Systems [[2](#references)] and finally at the Workshop on Open Source Aerial Robotics during the International Conference on Intelligent Robotic Systems, 2015 [[3](#references)]. Its early version was also presented at the International Conference of Robotics and Automation, 2013 [[4](#references)].
If you decide to use this software for your research, please cite <i>WhyCon</i> using the one of the references provided in this [bibtex](http://raw.githubusercontent.com/wiki/gestom/CosPhi/papers/WhyCon.bib) file.

### References
1. T. Krajník, M. Nitsche et al.: **[External localization system for mobile robotics.](http://raw.githubusercontent.com/wiki/gestom/CosPhi/papers/2013_icar_whycon.pdf)** International Conference on Advanced Robotics (ICAR), 2013. [[bibtex](http://raw.githubusercontent.com/wiki/gestom/CosPhi/papers/2013_icar_whycon.bib)].
2. T. Krajník, M. Nitsche et al.: **[A Practical Multirobot Localization System.](http://raw.githubusercontent.com/wiki/gestom/CosPhi/papers/2015_JINT_whycon.pdf)** Journal of Intelligent and Robotic Systems (JINT), 2014. [[bibtex](http://raw.githubusercontent.com/wiki/gestom/CosPhi/papers/2015_JINT_whycon.bib)].
3. M. Nitsche, T. Krajník et al.: **[WhyCon: An Efficent, Marker-based Localization System.](http://raw.githubusercontent.com/wiki/gestom/CosPhi/papers/2015_irososar_whycon.pdf)** IROS Workshop on Open Source Aerial Robotics, 2015. [[bibtex](http://raw.githubusercontent.com/wiki/gestom/CosPhi/papers/2015_irososar_whycon.bib)].
4. J. Faigl, T. Krajník et al.: **[Low-cost embedded system for relative localization in robotic swarms.](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6630694)** International Conference on Robotics and Automation (ICRA), 2013. [[bibtex](http://raw.githubusercontent.com/wiki/gestom/CosPhi/papers/2013_icra_whycon.bib)].

### Acknowledgements

The development of this WhyCon was supported by EU within its Seventh Framework Programme project ICT-600623 ``STRANDS''.
The Czech Republic and Argentina have given support through projects 7AMB12AR022, ARC/11/11, 13-18316P and 17-27006Y STRoLL - Spatio-Temporal Representations for Mobile Robot Navigation. We sincerely acknowledge [Jean Pierre Moreau](http://jean-pierre.moreau.pagesperso-orange.fr/infos.html) for his excellent libraries for numerical analysis that we use in our project.
