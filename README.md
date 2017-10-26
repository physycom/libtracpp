<a href="http://www.physycom.unibo.it">
<div class="image">
<img src="https://cdn.rawgit.com/physycom/templates/697b327d/logo_unibo.png" width="90" height="90" alt="© Physics of Complex Systems Laboratory - Physics and Astronomy Department - University of Bologna">
</div>
</a>
<a href="https://travis-ci.com/physycom/libtracpp">
<div class="image">
<img src="https://travis-ci.com/physycom/libtracpp.svg?token=ujjUseBa9hYbKckXBkxJ&branch=master" width="90" height="20" alt="Build Status">
</div>
</a>

### Purpose
This document presents the C++11 port of the original `libtra` C89 library, designed to reduce number of points without losing informations about tracks and roads.
The purpose of the algorithm is, given a curve composed of line segments, to find a similar curve with fewer points. The algorithm defines 'dissimilar' based on the maximum distance between the original curve and the simplified curve. The simplified curve consists of a subset of the points that defined the original curve.

### Details
The library has to be deployed as a submodule for the repositories that need using it.

