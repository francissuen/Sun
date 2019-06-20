try:
    from mars import dependency
except ImportError:
    import subprocess
    import sys
    subprocess.run([sys.executable, "-m", "pip", "install",
                    "git+ssh://git@github.com/francissuen/mars.git@master"])
    from mars import dependency

    
deps = dependency.Dependency()

cmake_utility = {
    "src_path": """\
https://github.com/francissuen/fsCMake/releases/download/{0}/\
cmake_utility.tar.xz\
"""}
cmake_utility["src_path"] = cmake_utility["src_path"].format("v0.1.0-alpha")
deps.add(dependency.DepInfo(cmake_utility))

deps.fix()
