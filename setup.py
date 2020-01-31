try:
    from mars import dependency
except ImportError:
    import subprocess
    import sys
    subprocess.run([sys.executable, "-m", "pip", "install",
                    "git+ssh://git@github.com/francisjsun/mars.git@master"])
    from mars import dependency

    
deps = dependency.Dependency()

vesta = {
    "src_path": """\
https://github.com/francisjsun/vesta.git"""}
deps.add(dependency.DepInfo(vesta), dependency.fs_git_proj_dep_sln)

deps.fix()
