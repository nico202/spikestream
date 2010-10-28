# A SpikeStream pattern for injection into a neuron group during simulation.
# The pattern defines a three dimensional cuboid that will be aligned with the centre of the neuron
# group. The parts of the pattern that are outside of the neuron group will be ignored. 
# Only specify the parts of the pattern that are firing or which receive injected current.
# When specifying cuboids, a range of numbers can be specified using the Matlab format. 
# For example 3:8 x 2:4 x 3:3 specifies a box whose X coordinates range from 3-8 inclusive,
# whose Y coordinates range from 2-4 inclusive and whose Z coordinates range from 3-3 inclusive.
# Individual points are specified using three comma separated numbers, e.g 1,1,1.
# Coordinates can include decimal places.

# The name that appears in the drop down combo to select the pattern. This must be specified
Name: Example pattern

# A box with side length of 10 starting at 3,2,1
3:12 x 2:11 x 1:10

# The point (1.3, 2.2, 1.7)
1.3, 2.2, 1.7

# Another box
5:6x5:6x5:10 

# Another point
10,11,12
