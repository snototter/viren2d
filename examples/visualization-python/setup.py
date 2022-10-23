import setuptools

#TODO description
#TODO version

long_description = 'TODO'
version = 'dev'

setuptools.setup(
    name="visualizers",
    version=version,
    author="snototter",
    author_email="snototter@users.noreply.github.com",
    description="Visualizer Pipeline for viren2d.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/snototter/viren2d",
    packages=setuptools.find_packages(),
    install_requires=[
        'numpy',
        'Pillow>=5.0.0'],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.6',
)
