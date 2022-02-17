# Truss-Me

## Overview

The Truss-Me program (Truss-Me.py) is a simple finite elements program for designing and testing truss structres composed of 2-node bar elements under applied forcing conditions and degree-of-freedom constraints.

I wrote the Truss-Me program (in part) to demonstrate my undertanding of the basics of finite-element modeling and analysis. It's based on a homework assignment from a course called Introduction to Finite Elements (ASEN 5007) which I took at CU Boulder during Fall 2018, and was inspired by the Mathematica programs used in that course (described at length in Felippa, 2004).

Other reasons for writing the program included a feeling that the whole process lends itself well to object-oriented programming, I was curious to see how the visualizations would look using interactive graphical software, and I thought that it might be a lot of fun. Incidentally: it was a lot of fun, the class-based layout of the program feels very natural to me, and the interactive 3D plotting is quite nice.

The program is accompanied by three figures exhibiting the initial truss structure with labeled nodes, a depiction of the displaced structure after forces have been applied (with displacements magnified 100x), and a colormap image of the structure demonstrating the internal strains of each bar element.

### Use Guide

You can run Truss-Me.py as you would any other Python program to generate plots (I prefer to do so using iPython!). If you plan to run it from a command line and want to generate plots, I'd recommend replacing the lines "plt.show()" with "plt.save("file_name",dpi=desired_dpi)" to store them automatically. Interactive plotting windows allow you to view the visualizations from many different angles and are highly recommended!

This program is designed to handle a variety of changes. The arrays in sections below the comment that reads 'Define node positions (m)' (line 227) can be altered to produce different truss structures, change constituent material & fabrication properties, and control applied forcing & degree-of-freedom conditions.

### References

Felippa (2004)
Felippa, Carlos A., "Introduction to finite element methods," University of Colorado 885 (2004)

Fox and Schmit (1964)
Fox, R. L. and Schmit, L. A., "Advances in the integrated approach to structural synthesis," AIAA/ASME Material Conference, 1964., pp. 294-315