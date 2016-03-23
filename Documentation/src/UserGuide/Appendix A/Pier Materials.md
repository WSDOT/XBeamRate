Materials and Reinforcement {#appendix_a_pier_materials}
============
The pier materials and cross beam reinforcment are defined on this tab.

Material Properties
-------------------

Item | Description
-----|-------------
Reinforcement | Use the drop down list to select the type of reinforcement used for longitudinal reinforcement and stirrups
f'c | Enter the concrete strength
Ec | When check, enter the modulus of elasticity of the concrete. Otherwise the modulus of elasticity is computed in accordance with the AASHTO LRFD Specifications and the concrete properties
[More Properites...] | Opens the @subpage appendix_a_concrete window.


Longitudinal Reinforcement
-----------------------------
Multiple layers of longitudinal reinforcement can be defined for the cross beam. For each row enter the following parameters:


Parameter | Description
----------|---------------
XBeam Face | The face from which the reinforcement is located and the cover is measured.<br>Bottom - reinforcement located from the bottom of the cross beam<br>Top - reinforcement is located from the top of the cross beam for Expansion and Continuous piers, located from the top of the superstructure diaphragm for Integral piers<br>Top Lower XBeam - reinforcement is located from the top of the lower cross beam for Integral piers
Datum | Defines the datum for locating reinforcement<br>Full Length - reinforcement runs the full length of the cross beam<br>Left End - reinforcement is located from the left end of the cross beam. Reinforcement is defined by its start location and length.<br>Right End - reinforcement is located from the right end of the cross beam. Reinforcement is defined by its start location and length.
Start | Location of the start of the reinforcement (only used with Left End and Right End Datum)
End | Location of the end of the reinforcement (only used with Left End and Right End Datum)
Cover | Clear cover from the specified XBeam Face to the reinforcement
Bar Size | The size of the reinforcing bar
# of Bars | Number of reinforcing bars in the layer
Spacing | Center-to-center distance between reinforcing bars in the layer. Reinforcing bars are placed symmetrically about the center of the cross beam.
Left Hook | Indicates if the left end of the reinforcement is hooked. If hooked, it is considered to be fully developed at its left end
Right Hook | Indicates if the right end of the reinforcement is hooked. If hooked, it is considered to be fully developed at its right end


### Cross Beam\\Lower Cross Beam Stirrups
Defines the stirrups in the substructure portion of the cross beam as a sequence of zones. 


If the Symmetric about centerline is check, the zones are mirrored about the mid-point of the cross beam.


Parameter | Description
----------|------------
Zone Length | Length of the stirrup zone
Bar Size | The size of the reinforcement bar
Spacing | Spacing of the bars within the zone
# of Legs | Number of stirrup legs at each bar location


The exact arrangement of reinforcement isn't that important. XBRate computes Av/S for each zone as (Abar)*(# Legs)/S and uses this value over the entire zone length.


### Full Depth Stirrups
Defines the stirrups that are anchored in the substructure portion of the cross beam and are extended into the superstructure diaphragm. This input is only applicable
to Integral piers.


The input is the same as described above.
