import click
import os

from mlr.share.projects.integrated_model.utils.constants import CoreUtils, NameUtils
from mlr.share.projects.integrated_model.utils.msg_utils import Msg


@click.command()
@click.option("-b", "--build_only", default=False, is_flag=True, help="only build C++ code, do not run python")
@click.option("-B", "--build_force", default=False, is_flag=True, help="build C++ code and run file")
@click.option('-e', '--exp_type', type=click.STRING, help='experiment type information')
@click.option('-t', '--exp_trial_num', type=click.STRING, help='experiment trial number information')
@click.option("-i", "--run_init", default=False, is_flag=True, help="view init file")
@click.option("-f", "--run_final", default=False, is_flag=True, help="view file to final file")
@click.option("-inter", "--run_intermediate", default=False, is_flag=True, help="view intermediate file")
@click.option('-s', '--source', type=click.STRING, help='<folder_num>/<intermediate_filename>')
@click.option('-p', '--g_file_path', default="", type=click.STRING, help='file path to a .g file')
def main(build_only, build_force, exp_type, exp_trial_num, run_init, run_final,
         run_intermediate, source, g_file_path):
    if build_only:
        os.system("make --always-make --silent -f makefile_run_viewer")
        return

    if build_force:
        os.system("make --always-make --silent -f makefile_run_viewer")

    init_filename = exp_type + CoreUtils.UNDERSCORE + exp_trial_num + CoreUtils.DOT_G
    intermediate_folder_name = exp_type + CoreUtils.UNDERSCORE + exp_trial_num + CoreUtils.UNDERSCORE
    final_filename = exp_type + CoreUtils.UNDERSCORE + exp_trial_num + CoreUtils.DOT_F_G

    file_path = None
    if len(g_file_path) > 1:
        file_path = g_file_path
    else:
        if run_init:
            file_path = os.path.join(CoreUtils.get_exp_data_dir_path(), "init_files", init_filename)

        if run_intermediate:
            file_path = os.path.join(CoreUtils.get_intermediate_files_dir_path(), intermediate_folder_name + source)

        if run_final:
            file_path = os.path.join(CoreUtils.get_exp_data_dir_path(), "fin_files", final_filename)

    if file_path is None:
        Msg.print_error("ERROR [run_viewer]: must enter the type of file to view")

    arguments_list = [file_path]
    arguments_string = " ".join(arguments_list)
    os.system("./viewer.exe " + arguments_string)


if __name__ == '__main__':
    main()
