# Checkpoint 2 -- Adding a timer

Starting from checkpoint 1, with explanatory comments removed for brevity's sake.

We'll want to look at performance later, so we might as well just add some timings functionality. 

Since the time to render a frame can vary quite significantly we'll want to calculate the average value over multiple frames. We will also calculate the standard deviation (variation in the frame time) and the standard error (variation in the average frame time) to get a grasp on how large the fluctuations are. 

We calculate the average *and* error without having to store all the values (praise mathematics). 