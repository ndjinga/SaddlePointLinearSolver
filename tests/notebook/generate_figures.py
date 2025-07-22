from plot_utils import *

all_types = sorted(df['matrix-type'].unique())
all_types_name = "All Types"

types_2D = ['2DCartesian',
 '2DKershaw',
 '2DLocallyRefined',
 '2DPolygons',
 '2DQuadrangles',
 '2DTriangles' ]
types_2D_name = "2D"

types_3D = [
 '3DAssemblyMesh',
 '3DCheckerBoard',
 '3DHexa',
 '3DKershaw',
 '3DLocRaf',
 '3DPrism',
 '3DPrismHexa',
 '3DRandom',
 '3DTetra',
 '3DVoronoi']
types_3D_name = "3D"


SAVE_FIGURES = True
SHOW_FIGURES = False

# Total error by mesh size and mesh type


plot_error_for_types(types_2D, types_2D_name, show_figures=SHOW_FIGURES, save_figures=SAVE_FIGURES)
plot_error_for_types(types_3D, types_3D_name, show_figures=SHOW_FIGURES, save_figures=SAVE_FIGURES)


# Number of iterations by mesh size and mesh type


plot_iter_for_types(types_2D, types_2D_name, show_figures=SHOW_FIGURES, save_figures=SAVE_FIGURES)
plot_iter_for_types(types_3D, types_3D_name, show_figures=SHOW_FIGURES, save_figures=SAVE_FIGURES)


# Total Time by mesh size and mesh type (Sequential / 1 proc)

plot_total_time_for_types(types_2D, types_2D_name, show_figures=SHOW_FIGURES, save_figures=SAVE_FIGURES)
plot_total_time_for_types(types_3D, types_3D_name, show_figures=SHOW_FIGURES, save_figures=SAVE_FIGURES)


# Memory consumption by mesh size and mesh type

plot_memory_consumption_for_types(types_2D, types_2D_name, show_figures=SHOW_FIGURES, save_figures=SAVE_FIGURES)
plot_memory_consumption_for_types(types_3D, types_3D_name, show_figures=SHOW_FIGURES, save_figures=SAVE_FIGURES)


# Condition number by mesh size and mesh type

plot_condition_number_for_types(types_2D, types_2D_name, show_figures=SHOW_FIGURES, save_figures=SAVE_FIGURES)
plot_condition_number_for_types(types_3D, types_3D_name, show_figures=SHOW_FIGURES, save_figures=SAVE_FIGURES)


# Residual / Error ratio by mesh size and mesh type

plot_residual_error_ratio_for_types(types_2D, types_2D_name, show_figures=SHOW_FIGURES, save_figures=SAVE_FIGURES)
plot_residual_error_ratio_for_types(types_3D, types_3D_name, show_figures=SHOW_FIGURES, save_figures=SAVE_FIGURES)


# ## Total Time by Matrix Size and Number of Processors for Each Mesh Type

plot_procs_for_types(show_figures=SHOW_FIGURES, save_figures=SAVE_FIGURES)