from pathlib import Path

VIREN2D_ROOT_PATH = Path(__file__).absolute().parents[3]
VIREN2D_DATA_PATH = VIREN2D_ROOT_PATH / 'examples' / 'data'

VIREN2D_COLORMAP_CATEGORIES = {
    'sequential': [
        'cividis', 'cvd-sequential', 'cvd-sequential-vivid', 'gouldian',
        'viridis', 'inferno', 'hell', 'thermal', 'black-body', 'hot',
        'cold', 'water', 'copper', 'spring', 'summer', 'autumn', 'winter',
        'gray', 'yarg'
    ],
    'cyclic': [
        'cvd-orientation', 'orientation-4', 'orientation-6', 'hsv',
        'optical-flow', 'twilight', 'twilight-shifted'
    ],
    'categorical': [
        'categories-10', 'categories-12', 'categories-20', 'glasbey-dark',
        'glasbey-light'
    ],
    'color-blind': [
        'cividis', 'cvd-sequential', 'cvd-sequential-vivid',
        'cvd-diverging', 'cvd-orientation'
    ],
    'diverging': [
        'cvd-diverging', 'seismic', 'temperature',
        'temperature-dark', 'spectral-diverging',
        
    ],
    'rainbow': [
        'jet', 'rainbow', 'turbo'
    ],
    'specialized': [
        'ocean', 'water',  'earth', 'terrain', 'relief',
        'relief-low-contrast', 'thermal', 'spectral', 'disparity',
        'optical-flow'
    ]
}