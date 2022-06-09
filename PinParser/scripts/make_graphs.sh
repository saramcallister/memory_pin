#!/bin/bash 

if [[ $1 == "ad" ]]; then 
    python3 scatter_graph.py 503/ad/* graphs/503_scatter_ad
    python3 scatter_graph.py 505/ad/* graphs/505_scatter_ad
    python3 scatter_graph.py 519/ad/* graphs/519_scatter_ad
    python3 scatter_graph.py 557/cld/ad/* graphs/557_cld_scatter_ad
    python3 scatter_graph.py 557/input/ad/* graphs/557_input_scatter_ad
    python3 scatter_graph.py 557/cpu2006/ad/* graphs/557_cpu2006_scatter_ad
    python3 scatter_graph.py 602/ad/* graphs/602_scatter_ad

#     python3 plot_graph.py 503/ad/* graphs/503_plot_ad
#     python3 plot_graph.py 505/ad/* graphs/505_plot_ad
#     python3 plot_graph.py 519/ad/* graphs/519_plot_ad
#     python3 plot_graph.py 557/cld/ad/* graphs/557_cld_plot_ad
#     python3 plot_graph.py 557/input/ad/* graphs/557_input_plot_ad
#     python3 plot_graph.py 557/cpu2006/ad/* graphs/557_cpu2006_plot_ad
#     python3 plot_graph.py 602/ad/* graphs/602_plot_ad

#     tar -cvzf ad_graphs.tar.gz graphs/*_ad.png
else 
    python3 graph_pebs.py 503/pebs/* graphs/503_pebs
    python3 graph_pebs.py 505/pebs/* graphs/505_pebs
    python3 graph_pebs.py 519/pebs/* graphs/519_pebs
    python3 graph_pebs.py 557/cld/pebs/* graphs/557_cld_pebs
    python3 graph_pebs.py 602/pebs/* graphs/602_pebs

    tar -cvzf pebs_graphs.tar.gz graphs/*_pebs.png
fi
