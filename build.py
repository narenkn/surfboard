from distutils.core import setup, Extension
import os

debussy_home = os.environ["VERDI_HOME"];

inc_dir = [debussy_home + '/share/FsdbReader', os.environ["ANCHOR_surfboard"] + os.sep + 'lib/creg', os.environ["ANCHOR_surfboard"] + os.sep + 'lib/pyc'];
libs = [debussy_home + '/share/FsdbReader/LINUXAMD64/libnffr.a', debussy_home + '/share/FsdbReader/LINUXAMD64/libnsys.a'];

setup (name="sb", version="1.0",
      ext_modules=[Extension("sb", [os.environ["ANCHOR_surfboard"] + os.sep + "lib/pyc/sb.cc"], extra_compile_args=[], define_macros=[("NO_ENV_DKI", None), ("__CREG_MSG__", None)], include_dirs=inc_dir, extra_objects=libs)]);

