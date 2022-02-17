# Buffmo

## Overview

The Buffmo program (Buffmo.py) is a basic but comprehensive tutorial for programmatically generating noisy dynamical and measurement data and implementing a standard Kalman filter to perform estimation on the artifically unknown states.

I wrote the Buffmo program as a tutorial for students when I served as TA for a course called Statistical Estimation for Dynamical Systems (ASEN 5044) at CU Boulder during Fall 2021. It's written primarily in a functional format to achieve maximum comparability with MATLAB scripts, which were preferred by some of the students in the class.

The program is accompanied by several figures and an extensive write-up (Buffmo.pdf) that covers everything from equations of motion and the state-space formulation to code implementation. The figures include sample trajectory & error plots, and NEES & NIS statistic plots averaged over many truth model runs.

### Use Guide

You can run Buffmo.py as you would any other Python program to generate plots (I prefer to do so using IPython). If you plan to run it from a command line and want to generate plots, I'd recommend replacing the lines "plt.show()" with "plt.save("file_name",dpi=desired_dpi)" to store them automatically.

Other than the few noise & dynamical parameters listed benath the comment that reads 'Define parameters' (lines 15-21), there isn't too much to play around with in this program. Additional tunable variables include the number of truth model runs performed "nTMT" and the certainty level "alpha".