# python script to make graphs for psrs assignment

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import os

plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.serif'] = ['Times New Roman', 'DejaVu Serif', 'serif']
plt.rcParams['font.size'] = 22
plt.rcParams['axes.linewidth'] = 1.2
plt.rcParams['axes.grid'] = True
plt.rcParams['grid.alpha'] = 0.4
plt.rcParams['grid.linestyle'] = '--'
plt.rcParams['xtick.labelsize'] = 20
plt.rcParams['ytick.labelsize'] = 20
plt.rcParams['axes.labelsize'] = 22
plt.rcParams['axes.titlesize'] = 24
plt.rcParams['legend.fontsize'] = 18
plt.rcParams['figure.facecolor'] = 'white'
plt.rcParams['axes.facecolor'] = 'white'
plt.rcParams['savefig.facecolor'] = 'white'

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
    
    fig, ax = plt.subplots(figsize=(10, 7))
    
    thread_cols = [c for c in df.columns if c != 'n']
    threads = [int(c.replace('p', '')) for c in thread_cols]
    
    markers = ['o', 's', '^', 'D', 'v', 'p']
    colors = ['#2E86AB', '#E94F37', '#4DAA57', '#F6AE2D', '#7B68EE', '#8B4513']
    
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
                markersize=12,
                linewidth=3.0,
                markeredgecolor='black',
                markeredgewidth=0.8,
                label=label)
    
    y_limit = max_speedup * 1.25
    ax.plot([1, y_limit], [1, y_limit], 
            linestyle='--', color='black', linewidth=3, label='Linear')
    
    ax.set_xlabel('Number of Processors (p)', fontsize=22, fontweight='bold')
    ax.set_ylabel('Speedup (S)', fontsize=22, fontweight='bold')
    ax.set_xticks(threads)
    ax.set_ylim(0, y_limit)
    ax.tick_params(axis='both', labelsize=20)
    ax.legend(loc='upper left', frameon=True, fancybox=False, edgecolor='gray', fontsize=18)
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.yaxis.grid(True, linestyle='--', alpha=0.5)
    ax.xaxis.grid(False)
    ax.set_axisbelow(True)
    
    plt.tight_layout()
    savepath = os.path.join(script_dir, 'plot_speedup_processors.png')
    plt.savefig(savepath, dpi=300, bbox_inches='tight')
    print(f"Saved: {savepath}")
    plt.close()

# plot 2: efficiency vs processors
def plot_efficiency():
    df = read_speedup_data()
    
    fig, ax = plt.subplots(figsize=(10, 7))
    
    thread_cols = [c for c in df.columns if c != 'n']
    threads = [int(c.replace('p', '')) for c in thread_cols]
    
    markers = ['o', 's', '^', 'D', 'v', 'p']
    colors = ['#2E86AB', '#E94F37', '#4DAA57', '#F6AE2D', '#7B68EE', '#8B4513']
    
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
                markersize=12,
                linewidth=3.0,
                markeredgecolor='black',
                markeredgewidth=0.8,
                label=label)
    
    ax.axhline(y=1.0, linestyle='--', color='black', linewidth=3, label='Ideal (E=1)')
    
    ax.set_xlabel('Number of Processors (p)', fontsize=22, fontweight='bold')
    ax.set_ylabel('Efficiency (E = S/p)', fontsize=22, fontweight='bold')
    ax.set_xticks(threads)
    ax.set_ylim(0, 1.15)
    ax.tick_params(axis='both', labelsize=20)
    ax.legend(loc='upper right', frameon=True, fancybox=False, edgecolor='gray', fontsize=18)
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.yaxis.grid(True, linestyle='--', alpha=0.5)
    ax.xaxis.grid(False)
    ax.set_axisbelow(True)
    
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
    
    fig, ax = plt.subplots(figsize=(14, 8))
    
    x = np.arange(num_threads)
    width = 0.8 / num_sizes
    colors = ['#2E86AB', '#E94F37', '#4DAA57', '#F6AE2D', '#7B68EE', '#8B4513']
    
    for i in range(num_sizes):
        row = df.iloc[i]
        speedups = [row[col] for col in thread_cols]
        offset = (i - num_sizes/2 + 0.5) * width
        ax.bar(x + offset, speedups, width, 
               label=size_labels[i], color=colors[i % len(colors)],
               edgecolor='black', linewidth=0.8)
    
    ax.plot(x, threads, 'k--', linewidth=3.5, marker='_', markersize=30, 
            markeredgewidth=4, label='Ideal Linear')
    
    ax.set_xlabel('Number of Processors (p)', fontsize=22, fontweight='bold')
    ax.set_ylabel('Speedup (S)', fontsize=22, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels([f'{t}' for t in threads], fontsize=20)
    ax.tick_params(axis='y', labelsize=20)
    ax.legend(loc='upper left', frameon=True, fancybox=False, edgecolor='gray', ncol=2, fontsize=18)
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.yaxis.grid(True, linestyle='--', alpha=0.5)
    ax.xaxis.grid(False)
    ax.set_axisbelow(True)
    
    plt.tight_layout()
    savepath = os.path.join(script_dir, 'plot_speedup_vs_ideal.png')
    plt.savefig(savepath, dpi=300, bbox_inches='tight')
    print(f"Saved: {savepath}")
    plt.close()

# plot 4: phase breakdown stacked bar chart for ALL array sizes
def plot_phase_breakdown():
    filepath = os.path.join(results_dir, 'results_phases.txt')
    
    if not os.path.exists(filepath):
        print("No phase data file found, skipping phase plot")
        return
    
    df = pd.read_csv(filepath)
    
    # get unique array sizes
    sizes = df['n'].unique()
    num_sizes = len(sizes)
    
    # create subplots - 2 rows, 3 cols for 6 sizes - LARGER figure
    cols = 3
    rows = (num_sizes + cols - 1) // cols
    fig, axes = plt.subplots(rows, cols, figsize=(24, 9*rows))
    axes = axes.flatten()
    
    # colors - professional palette
    colors = ['#2E86AB', '#F6AE2D', '#4DAA57', '#E94F37']
    
    for idx, n in enumerate(sizes):
        ax = axes[idx]
        df_filtered = df[df['n'] == n]
        
        processors = df_filtered['p'].values
        p1 = df_filtered['phase1'].values
        p2 = df_filtered['phase2'].values
        p3 = df_filtered['phase3'].values
        p4 = df_filtered['phase4'].values
        
        x = np.arange(len(processors))
        width = 0.65
        
        # stacked bars with edge
        ax.bar(x, p1, width, label='Phase 1: Local Sort', color=colors[0], edgecolor='black', linewidth=1.0)
        ax.bar(x, p2, width, bottom=p1, label='Phase 2: Sampling', color=colors[1], edgecolor='black', linewidth=1.0)
        ax.bar(x, p3, width, bottom=p1+p2, label='Phase 3: Partition', color=colors[2], edgecolor='black', linewidth=1.0)
        ax.bar(x, p4, width, bottom=p1+p2+p3, label='Phase 4: Merge', color=colors[3], edgecolor='black', linewidth=1.0)
        
        # format size label
        if n >= 1000000:
            size_label = f"n = {n // 1000000}M elements"
        else:
            size_label = f"n = {n // 1000}K elements"
        
        ax.set_title(size_label, fontsize=28, fontweight='bold', pad=12)
        ax.set_xlabel('Processors', fontsize=24, fontweight='bold')
        ax.set_ylabel('Time (seconds)', fontsize=24, fontweight='bold')
        ax.set_xticks(x)
        ax.set_xticklabels([f'{p}' for p in processors], fontsize=22)
        ax.tick_params(axis='y', labelsize=22)
        ax.spines['top'].set_visible(False)
        ax.spines['right'].set_visible(False)
        ax.spines['left'].set_linewidth(1.2)
        ax.spines['bottom'].set_linewidth(1.2)
        
        # subtle grid on y-axis only
        ax.yaxis.grid(True, linestyle='--', alpha=0.4)
        ax.set_axisbelow(True)
    
    # hide extra subplots if any
    for idx in range(num_sizes, len(axes)):
        axes[idx].set_visible(False)
    
    # add single legend at top - positioned above figure with larger font
    handles, labels = axes[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc='upper center', ncol=4, 
               bbox_to_anchor=(0.5, 1.02), fontsize=24, frameon=True,
               fancybox=True, shadow=False, edgecolor='gray')
    
    plt.tight_layout(rect=[0, 0, 1, 0.94])
    plt.subplots_adjust(top=0.90, hspace=0.32, wspace=0.22)
    savepath = os.path.join(script_dir, 'plot_phase_breakdown.png')
    plt.savefig(savepath, dpi=300, bbox_inches='tight', facecolor='white')
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
