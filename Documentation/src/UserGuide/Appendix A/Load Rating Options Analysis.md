Analysis {#appendix_a_load_rating_options_analysis}
============
The parameters on this tab control the structural analysis performed by XBRate

Item | Description
-----|-------------
Live Load Step Size | Enter the maximum step size for moving live load. XBRate determines live load moments and shears by placing many combinations of loaded lanes and lane positions on the cross beam. For each live load arrangement, the lanes are moved transverse across the pier based on a uniform step size. The Maximum Live Load Step Size parameter indicates the maximum uniform step that is to be used in the live load analysis. The smaller the step size, the more accurate the analysis, however the number or live load configurations can increase dramatically increasing analysis time. Larger steps sizes reduces the number of live load configurations and decreases analysis time, though it comes at the cost of less accurate analysis.
Maximum Number of Lanes | Enter the maximum number of lanes to be considered or the keyword "ALL" if all lanes are to be loaded. The number of lanes to be analyzed is determined by AASHTO MBE 6A.2.3.2. Governing cases typically occur when fewer than the maximum number of lanes are loaded. This parameter sets the maximum number of lanes to be considered in the live load analysis. Using fewer lanes will increase the speed of the analysis. 
