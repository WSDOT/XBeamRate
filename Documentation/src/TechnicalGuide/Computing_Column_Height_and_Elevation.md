Computing Column Height and Elevations {#computing_column_height_and_elevations}
======================================
The height of columns and elevations are established from the input parameters. The sketch below shows the basic dimensions involved in the calculations.
![](columnelevation.png)

##Top of Column Elevation
The top of column elevation is computed as

Ytop = (Deck Elevation at Alignment) + (Xcol)(S) - TS - Hsuper - Hsub

where

Ytop   = top of column elevation

Xcol   = offset from the alignment centerline to the subject column

S      = slope of the roadway surface in the plane of the pier

TS     = thickness of the deck slab measured to the top of the superstructure diaphragm

Hsuper = depth of the superstructure (excluding the deck slab)

Hsub   = height of the cross beam (substructure depth)


##Bottom of Column Elevation
When the column height is input, the bottom of column elevation is computed as 

Ybot = Ytop - Hcolumn

where

Ytop = top of column elevation

Hcolumn = input height of column

##Column Height
When the bottom of column elevation is input, the column height is computed as

Hcolumn = Ytop - Ybot.

> NOTE: When integrated with PGSuper/PGSplice, the depth of the superstructure (Hsuper) is taken to be the depth of the tallest precast girder at the pier. The dimensions of oak blocks, bearing pads, grout pads, girder recesses, etc. are not taking into account. For this reason, you are advised to use more precise methods of determining the bottom of column elevation for purposes other than the pier model used by XBRate. **DO NOT USE THE COMPUTED BOTTOM OF COLUMN ELEVATION FOR PS&E OR CONSTRUCTION**

