import click
import os

from mlr.share.projects.tipsy_towers.utils.constants import CoreUtils, NameUtils
from mlr.share.projects.tipsy_towers.utils.msg_utils import Msg


@click.command()
@click.option("-b", "--build_only", default=False, is_flag=True, help="only build C++ code, do not run python")
@click.option("-B", "--build_force", default=False, is_flag=True, help="build C++ code and run file")
@click.option('-n', '--no_save', default=True, is_flag=True, help='do not save resulting state')
@click.option('-p', '--g_file_path', default="", type=click.STRING, help='file path to a .g file')
def main(build_only, build_force, no_save, g_file_path):
    if build_only:
        os.system("make --always-make --silent -f makefile_run_stability")
        return

    if build_force:
        os.system("make --always-make --silent -f makefile_run_stability")

    file_path = None
    if len(g_file_path) > 1:
        file_path = g_file_path

    if file_path is None:
        Msg.print_error("ERROR [run_viewer]: must enter the type of file to view")
        assert False

    block_names_list = "#".join(NameUtils.TEST_STACK_BLOCKS)

    arguments_string = " ".join([file_path, block_names_list])
    os.system("./run_stability.exe " + arguments_string)


if __name__ == '__main__':
    main()
