# python script to make graphs for psrs assignment

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import os

# setup clean paper style with larger fonts
plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.size'] = 14
plt.rcParams['axes.linewidth'] = 0.8
plt.rcParams['axes.grid'] = True
plt.rcParams['grid.alpha'] = 0.3
plt.rcParams['grid.linestyle'] = '-'
plt.rcParams['xtick.labelsize'] = 12
plt.rcParams['ytick.labelsize'] = 12
plt.rcParams['axes.labelsize'] = 14
plt.rcParams['legend.fontsize'] = 11

# get directories
script_dir = os.path.dirname(os.path.abspath(__file__))
results_dir = os.path.join(script_dir, '../logs/')

def read_speedup_data():
    filepath = os.path.join(results_dir, 'results_speedup.txt')
    df = pd.read_csv(filepath)
    return df

# plot 1: speedup vs processors for all array sizes
def plot_speedup_vs_processors():
    df = read_speedup_data()
    
    fig, ax = plt.subplots(figsize=(9, 6))
    
    thread_cols = [c for c in df.columns if c != 'n']
    threads = [int(c.replace('p', '')) for c in thread_cols]
    
    markers = ['o', 's', '^', 'D', 'v', 'p']
    colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b']
    
    max_speedup = 0
    
    for i in range(len(df)):
        row = df.iloc[i]
        size = int(row['n'])
        speedups = [row[col] for col in thread_cols]
        max_speedup = max(max_speedup, max(speedups))
        
        if size >= 1000000:
            label = f"{size // 1000000}M"
        else:
            label = f"{size // 1000}K"
        
        ax.plot(threads, speedups, 
                marker=markers[i % len(markers)],
                linestyle='-',
                color=colors[i % len(colors)],
                markersize=8,
                linewidth=1.8,
                label=label)
    
    y_limit = max_speedup * 1.3
    ax.plot([1, y_limit], [1, y_limit], 
            linestyle='--', color='black', linewidth=1.5, label='Linear')
    
    ax.set_xlabel('Processors')
    ax.set_ylabel('Speedup')
    ax.set_xticks(threads)
    ax.set_ylim(0, y_limit)
    ax.legend(loc='upper left', frameon=True)
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    
    plt.tight_layout()
    savepath = os.path.join(script_dir, 'plot_speedup_processors.png')
    plt.savefig(savepath, dpi=300, bbox_inches='tight')
    print(f"Saved: {savepath}")
    plt.close()

# plot 2: efficiency vs processors
def plot_efficiency():
    df = read_speedup_data()
    
    fig, ax = plt.subplots(figsize=(9, 6))
    
    thread_cols = [c for c in df.columns if c != 'n']
    threads = [int(c.replace('p', '')) for c in thread_cols]
    
    markers = ['o', 's', '^', 'D', 'v', 'p']
    colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b']
    
    for i in range(len(df)):
        row = df.iloc[i]
        size = int(row['n'])
        speedups = [row[col] for col in thread_cols]
        efficiency = [s / p for s, p in zip(speedups, threads)]
        
        if size >= 1000000:
            label = f"{size // 1000000}M"
        else:
            label = f"{size // 1000}K"
        
        ax.plot(threads, efficiency, 
                marker=markers[i % len(markers)],
                linestyle='-',
                color=colors[i % len(colors)],
                markersize=8,
                linewidth=1.8,
                label=label)
    
    ax.axhline(y=1.0, linestyle='--', color='black', linewidth=1.5, label='Ideal')
    
    ax.set_xlabel('Processors')
    ax.set_ylabel('Efficiency (Speedup / p)')
    ax.set_xticks(threads)
    ax.set_ylim(0, 1.2)
    ax.legend(loc='upper right', frameon=True)
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    
    plt.tight_layout()
    savepath = os.path.join(script_dir, 'plot_efficiency.png')
    plt.savefig(savepath, dpi=300, bbox_inches='tight')
    print(f"Saved: {savepath}")
    plt.close()

# plot 3: speedup vs ideal - grouped bars for all array sizes
def plot_speedup_vs_ideal():
    df = read_speedup_data()
    
    thread_cols = [c for c in df.columns if c != 'n']
    threads = [int(c.replace('p', '')) for c in thread_cols]
    num_threads = len(threads)
    
    size_labels = []
    for i in range(len(df)):
        size = int(df.iloc[i]['n'])
        if size >= 1000000:
            size_labels.append(f"{size // 1000000}M")
        else:
            size_labels.append(f"{size // 1000}K")
    
    num_sizes = len(size_labels)
    
    fig, ax = plt.subplots(figsize=(13, 6))
    
    x = np.arange(num_threads)
    width = 0.8 / num_sizes
    colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b']
    
    for i in range(num_sizes):
        row = df.iloc[i]
        speedups = [row[col] for col in thread_cols]
        offset = (i - num_sizes/2 + 0.5) * width
        ax.bar(x + offset, speedups, width, 
               label=size_labels[i], color=colors[i % len(colors)],
               edgecolor='black', linewidth=0.5)
    
    ax.plot(x, threads, 'k--', linewidth=2, marker='_', markersize=15, 
            markeredgewidth=2, label='Ideal')
    
    ax.set_xlabel('Processors')
    ax.set_ylabel('Speedup')
    ax.set_xticks(x)
    ax.set_xticklabels([f'p={t}' for t in threads])
    ax.legend(loc='upper left', frameon=True)
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    
    plt.tight_layout()
    savepath = os.path.join(script_dir, 'plot_speedup_vs_ideal.png')
    plt.savefig(savepath, dpi=300, bbox_inches='tight')
    print(f"Saved: {savepath}")
    plt.close()

# plot 4: phase breakdown stacked bar chart
def plot_phase_breakdown():
    filepath = os.path.join(results_dir, 'results_phases.txt')
    
    # check if file exists
    if not os.path.exists(filepath):
        print("No phase data file found, skipping phase plot")
        return
    
    df = pd.read_csv(filepath)
    
    # use largest array size for cleaner visualization
    largest_n = df['n'].max()
    df_filtered = df[df['n'] == largest_n]
    
    fig, ax = plt.subplots(figsize=(10, 6))
    
    processors = df_filtered['p'].values
    p1 = df_filtered['phase1'].values
    p2 = df_filtered['phase2'].values
    p3 = df_filtered['phase3'].values
    p4 = df_filtered['phase4'].values
    
    x = np.arange(len(processors))
    width = 0.6
    
    # stacked bars
    ax.bar(x, p1, width, label='Phase 1 (Local Sort)', color='#1f77b4')
    ax.bar(x, p2, width, bottom=p1, label='Phase 2 (Sampling)', color='#ff7f0e')
    ax.bar(x, p3, width, bottom=p1+p2, label='Phase 3 (Partition)', color='#2ca02c')
    ax.bar(x, p4, width, bottom=p1+p2+p3, label='Phase 4 (Merge)', color='#d62728')
    
    ax.set_xlabel('Processors')
    ax.set_ylabel('Time (seconds)')
    ax.set_xticks(x)
    ax.set_xticklabels([f'p={p}' for p in processors])
    ax.legend(loc='upper right', frameon=True)
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    
    plt.tight_layout()
    savepath = os.path.join(script_dir, 'plot_phase_breakdown.png')
    plt.savefig(savepath, dpi=300, bbox_inches='tight')
    print(f"Saved: {savepath}")
    plt.close()

if __name__ == '__main__':
    print("Generating plots...")
    print("=" * 40)
    plot_speedup_vs_processors()
    plot_efficiency()
    plot_speedup_vs_ideal()
    plot_phase_breakdown()
    print("=" * 40)
    print("Done!")
