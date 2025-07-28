import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import re
from datetime import datetime
import os

csv_path = '../tables/output.csv'

df = pd.read_csv(csv_path)

def safe_filename(title):
    # Replace anything that is not alphanumeric, '_' or '-' with '_'
    return re.sub(r'[^\w\-]', '_', title)

now_str = datetime.now().strftime('%Y-%m-%d_%H-%M')
subfolder = f"figures/{now_str}"
os.makedirs(subfolder, exist_ok=True)


def plot_metric_for_types(
    mesh_types,
    metric,
    y_label,
    title,
    apply_mean=False,
    show_figures=False,
    save_figures=False,
    ylog=True
):
    plt.clf()
    min_proc =  df['n-proc'].min()
    for mtype in mesh_types:
        sub = df[(df['matrix-type'] == mtype) & (df['n-proc'] == min_proc)]
        if apply_mean:
            grouped = sub.groupby('matrix-size')[metric].mean()
            plt.scatter(sub['matrix-size'], sub['total-time'], marker='.', color=None)
            x, y = grouped.index, grouped.values
        else:
            x, y = sub['matrix-size'], sub[metric]
            indices = np.argsort(x)
            x = np.array(x)[indices]
            y = np.array(y)[indices]
        plt.plot(x, y, marker='o', label=mtype)

    plt.xscale('log')
    if ylog:
        plt.yscale('log')
    plt.xlabel("Matrix size")
    plt.ylabel(y_label)
    plt.title(title)
    plt.legend(title="Mesh type", bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0.)
    plt.grid(True, which="both", ls="--")
    if save_figures:
        plt.savefig(f'{subfolder}/{safe_filename(title)}.png', bbox_inches='tight')
    if show_figures:
        plt.show()
    plt.clf()



def plot_error_for_types(mesh_types, mesh_group_name, show_figures=False, save_figures=False):
    title = f'Total Error vs. Matrix Size for Each Mesh Type in "{mesh_group_name}" Type List'
    plot_metric_for_types(metric="total-error",
                          y_label="Total error",
                          mesh_types=mesh_types,
                          title=title,
                          show_figures=show_figures,
                          save_figures=save_figures)


def plot_iter_for_types(mesh_types, mesh_group_name, show_figures=False, save_figures=False):
    title = f'Number of iterations vs. Matrix Size for Each Mesh Type in "{mesh_group_name}" Type List'
    plot_metric_for_types(metric="iter",
                          y_label="Number of iterations",
                          mesh_types=mesh_types,
                          title=title,
                          show_figures=show_figures,
                          save_figures=save_figures)


def plot_total_time_for_types(mesh_types, mesh_group_name, show_figures=False, save_figures=False):
    title = f'Total Time vs. Matrix Size for Each Mesh Type in "{mesh_group_name}" Type List'
    plot_metric_for_types(metric="total-time",
                          y_label="Total time",
                          mesh_types=mesh_types,
                          title=title,
                          apply_mean=True,
                          show_figures=show_figures,
                          save_figures=save_figures)


def plot_memory_consumption_for_types(mesh_types, mesh_group_name, show_figures=False, save_figures=False):
    title = f'Memory Consumption vs. Matrix Size for Each Mesh Type in "{mesh_group_name}" Type List'
    plot_metric_for_types(metric="memory-consumption",
                          y_label="Memory consumption",
                          mesh_types=mesh_types,
                          title=title,
                          show_figures=show_figures,
                          save_figures=save_figures)


def plot_condition_number_for_types(mesh_types, mesh_group_name, show_figures=False, save_figures=False):
    title = f'Condition Number vs. Matrix Size for Each Mesh Type in "{mesh_group_name}" Type List'
    plot_metric_for_types(metric="condition-number",
                          y_label="Condition number",
                          mesh_types=mesh_types,
                          title=title,
                          show_figures=show_figures,
                          save_figures=save_figures)


def plot_residual_error_ratio_for_types(mesh_types, mesh_group_name, show_figures=False, save_figures=False):
    title = f'Residual / Error ratio vs. Matrix Size for Each Mesh Type in "{mesh_group_name}" Type List'
    plot_metric_for_types(metric="residual-error-ratio",
                          y_label="Ratio",
                          mesh_types=mesh_types,
                          title=title,
                          show_figures=show_figures,
                          save_figures=save_figures)





def plot_procs_for_types(
    show_figures=False,
    save_figures=False
):
    types_maillage = sorted(df['matrix-type'].unique())
    n_types = len(types_maillage)

    ncols = min(3, n_types)
    nrows = int(np.ceil(n_types / ncols))

    fig, axs = plt.subplots(nrows, ncols, figsize=(6*ncols, 5*nrows), squeeze=False)

    for idx, mtype in enumerate(types_maillage):
        row, col = divmod(idx, ncols)
        ax = axs[row, col]
        sub = df[df['matrix-type'] == mtype]
        for nproc in sorted(sub['n-proc'].unique()):
            sub2 = sub[sub['n-proc'] == nproc]
            x = sub2['matrix-size']
            y = sub2['total-time']
            indices = np.argsort(x)
            x_sorted = np.array(x)[indices]
            y_sorted = np.array(y)[indices]
            ax.plot(x_sorted, y_sorted, marker='o', label=f'{nproc} proc')
        ax.set_xscale('log')
        ax.set_xlabel('Matrix size')
        ax.set_ylabel('Total time')
        ax.set_title(f'Mesh type: {mtype}')
        ax.legend(title='Number of procs')
        ax.grid(True, which="both", ls="--")

    for i in range(n_types, nrows * ncols):
        fig.delaxes(axs.flat[i])

    plt.tight_layout()
    if save_figures:
        plt.savefig(f'{subfolder}/procs.png')
    if show_figures:
        plt.show()