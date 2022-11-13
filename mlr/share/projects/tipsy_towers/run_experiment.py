import ast
import click
import csv
import multiprocessing
import numpy as np
import os
import time

from itertools import repeat

from mlr.share.projects.tipsy_towers.model.experiment import Experiment, ExpType
from mlr.share.projects.tipsy_towers.utils.constants import CoreUtils


class Trials:
    def __init__(self, exp_type, trial_name):
        self._exp_type = exp_type
        self._trial_name = trial_name

    def get_exp_type(self):
        return self._exp_type

    def get_trial_name(self):
        return self._trial_name


def get_trials_list(query_exp_type=None, query_trial_names_list=None):
    return_trials_list = []

    trials_info_list_filename = os.path.join(CoreUtils.get_exp_data_dir_path(), "trials_list.csv")
    with open(trials_info_list_filename, "r") as trials_info_list_file:
        trials_info_list = csv.reader(trials_info_list_file, delimiter=',')
        trials_info_list = list(trials_info_list)
        for trial_info in trials_info_list[1:]:
            exp_type = ExpType.get_exp_type(trial_info[0])
            if query_exp_type is not None:
                if not query_exp_type == exp_type:
                    continue

            trial_name = trial_info[1]
            if query_trial_names_list is not None:
                if trial_name not in query_trial_names_list:
                    continue
            return_trials_list.append(Trials(exp_type, trial_name))

    return return_trials_list


def run_trial(trial):
    experiment = Experiment(exp_type=trial.get_exp_type(), exp_trial_num=trial.get_trial_name())
    experiment.run_experiment()


def run_exp_trial_multiprocess(exp_type):
    trails_list = get_trials_list(exp_type, ["1_4"])

    cores = multiprocessing.cpu_count()
    pool = multiprocessing.Pool(processes=cores, initializer=np.random.seed(int(time.time())))
    pool.starmap(run_trial, zip(trails_list))


@click.command()
@click.option("-b", "--build_only", default=False, is_flag=True, help="only build C++ code, do not run python")
@click.option("-B", "--build_force", default=False, is_flag=True, help="build C++ code and run file")
def main(build_only, build_force):
    if build_only:
        os.system("make --always-make --silent -f makefile_run_physx")
        return

    if build_force:
        os.system("make --always-make --silent -f makefile_run_physx")

    run_exp_trial_multiprocess(ExpType.STABILITY)


if __name__ == '__main__':
    main()
