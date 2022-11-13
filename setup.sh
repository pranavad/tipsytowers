#!/bin/bash


. load_config.sh


usage="Syntax: $(basename "$0") [-h|--help] [COMPONENTS...] -- will set up the project environment,
where:
    -h | --help     Print this help
    COMPONENTS...   Specify component to set up

Valid COMPONENTS:
    all: setup all components (container will be pulled, not built; python packages will be installed)
    cont_[pull|build]: pull the singularity container or build it
    python: add python venv and install necessary packages
    p_remove: remove python venv and installed packages
    data: pull data"


echo_blue () { echo -e "\033[0;36m $* \033[0m"; }
remove(){ if [ -d "$1" ]; then rm -rf "$1"; fi }
add(){ if [ ! -d "$1" ]; then mkdir -p "$1"; fi }


if [[ $# -eq 0 ]] || [[ "$*" =~ "--help" ]] || [[ "$*" =~ "-h" ]];then
    echo "$usage"
    exit 0
fi


## ==========================================================================
## ------------------------- singularity container setup ------------------- ##
## ==========================================================================
if [[ "$*" =~ "cont_pull" ]] || [[ "$*" =~ "all" ]];then
    echo "Pulling singularity container..."
    wget "https://yale.box.com/shared/static/vja3gphjjyj39bz2236nd4qnon6kc4yy.sif" -O "${ENV[cont]}"
elif [[ "$*" =~ "cont_build" ]];then
    echo "Building singularity container..."
    SINGULARITY_TMPDIR=/var/tmp sudo -E singularity build "${ENV[cont]}" Singularity
else
    echo "Not touching container"
fi


## ==========================================================================
## ------------------------- python setup ---------------------------------- ##
## ==========================================================================
if [[ "$*" =~ "python" ]] || [[ "$*" =~ "all" ]];then
    echo_blue "Setting up Python venv..."

    singularity exec "${ENV[cont]}" bash -c "python -m venv ${ENV[env]}"
    ./run.sh "python -m pip install --upgrade pip"
    ./run.sh "source ${ENV[env]}/bin/activate"
    ./run.sh "python -m pip install pillow"
    ./run.sh "python -m pip install numpy"
    ./run.sh "python -m pip install scipy"
    ./run.sh "python -m pip install sklearn"
    ./run.sh "python -m pip install matplotlib"
    ./run.sh "python -m pip install click-help-colors"
fi

if [[ "$*" =~ "p_remove" ]] || [[ "$*" =~ "all" ]];then
    echo_blue "Removing Python venv..."
    remove "poetry.lock"
    ./run.sh poetry env info -p | xargs -I{} rm -rf {}
    remove
fi


## ==========================================================================
## ------------------------- cpp setup ---------------------------------- ##
## ==========================================================================
if [[ "$*" =~ "cpp" ]] || [[ "$*" =~ "all" ]];then
    echo_blue "Setting up CPP venv..."
    
    singularity exec "${ENV[cont]}" bash -c "cd mlr/share && make"
    singularity exec "${ENV[cont]}" bash -c "cd mlr/share/projects/tipsy_towers && make"

fi

if [[ "$*" =~ "p_remove" ]] || [[ "$*" =~ "all" ]];then
    echo_blue "Removing Python venv..."
    remove "poetry.lock"
    ./run.sh poetry env info -p | xargs -I{} rm -rf {}
    remove
fi


## ==========================================================================
## ------------------------- data setup ------------------------------------ ##
## ==========================================================================
if [[ "$*" =~ "data" ]] || [[ "$*" =~ "all" ]];then
    echo "Pulling data..."
else
    echo "Not pulling any data"
fi
