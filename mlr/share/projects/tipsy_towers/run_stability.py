import click
import os

from mlr.share.projects.integrated_model.model.experiment import Experiment
from mlr.share.projects.integrated_model.utils.constants import CoreUtils
from mlr.share.projects.integrated_model.utils.msg_utils import Msg


@click.command()
@click.option("-b", "--build_only", default=False, is_flag=True, help="only build C++ code, do not run python")
@click.option("-B", "--build_force", default=False, is_flag=True, help="build C++ code and run file")
@click.option("-i", "--run_init", default=False, is_flag=True, help="init file to init file")
@click.option("-f", "--run_final", default=False, is_flag=True, help="final file to final file")
@click.option("-i2f", "--run_init_to_final", default=False, is_flag=True, help="final file to final file")
@click.option("-inter", "--run_intermediate", default=False, is_flag=True, help="intermediate file to intermediate file")
@click.option('-s', '--source_dir', type=click.STRING, help='<folder_num>/<intermediate_filename>')
@click.option('-e', '--exp_type', type=click.STRING, help='experiment type information')
@click.option('-t', '--exp_trial_num', type=click.STRING, help='experiment trial number information')
@click.option('-n', '--no_save', default=True, is_flag=True, help='do not save resulting state')
@click.option('-p', '--g_file_path', default="", type=click.STRING, help='file path to a .g file')
def main(build_only, build_force,
         run_init, run_final, run_init_to_final, run_intermediate, source_dir,
         exp_type, exp_trial_num, no_save, g_file_path):
    if build_only:
        os.system("make --always-make --silent -f makefile_run_stability")
        return

    if build_force:
        os.system("make --always-make --silent -f makefile_run_stability")

    init_filename = exp_type + CoreUtils.UNDERSCORE + exp_trial_num + CoreUtils.DOT_G
    intermediate_folder_name = exp_type + CoreUtils.UNDERSCORE + exp_trial_num + CoreUtils.UNDERSCORE
    final_filename = exp_type + CoreUtils.UNDERSCORE + exp_trial_num + CoreUtils.DOT_F_G

    file_path = None
    if len(g_file_path) > 1:
        file_path = g_file_path
    else:
        if run_init:
            file_path = os.path.join(CoreUtils.get_exp_data_dir_path(), "init_files", init_filename)
        elif run_intermediate:
            file_path = os.path.join(CoreUtils.get_intermediate_files_dir_path(), intermediate_folder_name + source_dir)
        elif run_final:
            file_path = os.path.join(CoreUtils.get_exp_data_dir_path(), "fin_files", final_filename)

    if file_path is None:
        Msg.print_error("ERROR [run_viewer]: must enter the type of file to view")
        assert False

    out_file_path = os.path.join(CoreUtils.get_init_files_dir_path(), init_filename)
    if run_final or run_init_to_final:
        out_file_path = os.path.join(CoreUtils.get_fin_files_dir_path(), final_filename)

    block_names_list = "#".join(Experiment.get_block_names(exp_type=exp_type, exp_trial_num=exp_trial_num))
    grabbed_block_names_list = ""

    if no_save:
        out_file_path = "0"

    arguments_list = [file_path, block_names_list, grabbed_block_names_list, out_file_path]
    arguments_string = " ".join(arguments_list)
    os.system("./run_stability.exe " + arguments_string)


if __name__ == '__main__':
    main()
