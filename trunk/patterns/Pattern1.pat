# A SpikeStream pattern for injection into a neuron group during simulation.
# The pattern defines a three dimensional cuboid that will be aligned with the centre of the neuron
# group. The parts of the pattern that are outside of the neuron group will be ignored. 
# Only specify the parts of the pattern that are firing.
# A range of numbers can be specified using the Matlab format. 
# For example 3:8 specifies the numbers 3,4,5,6,7,8.
# Individual points are specified using three comma separated numbers, e.g 1,1,1.
# A plane or a cuboid can be specified using 'x'.
# For example, 3 x 2:5 x 7 specifies a line covering points (3,2,7), (3,3,7), (3,4,7) and (3,5,7)
# For example, 0:9 x 1:3 x 0:4 specifies a cuboid width 10, length 10 and height 5 starting at (0,1,0)
# Coordinates can include decimal places.
# The width of the pattern runs from 0 to the maximum X index.

# The name that appears in the drop down combo to select the pattern
Name: Cats whiskers

# A box with side length of 10 starting at 3,2,1
3:12 x 2:11 x 1:10

# A point 
1.3, 2.2, 1.7

