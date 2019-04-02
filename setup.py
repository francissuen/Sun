from mars import dependency

dependencies = [
    dependency.Dependency({
        "name": "fsCMake",
        "addr": """https://github.com/francissuen/fsCMake/releases/download/\
        v1.0.3\
        /fsCMake.tar.xz"""})
]

for d in dependencies:
    d.fix()
