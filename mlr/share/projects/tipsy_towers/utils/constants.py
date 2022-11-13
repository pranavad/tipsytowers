import os


class NameUtils:
    NAME_RED = "red"
    NAME_YELLOW = "yellow"
    NAME_BLUE = "blue"

    NAME_DIFF_B = "b"
    NAME_DIFF_C = "c"

    TABLE = "table"

    NO_BLOCK_ON_TOP = "NO_BLOCK_ON_TOP"


class CoreUtils:
    SLASH = "/"
    UNDERSCORE = "_"

    DOT_TXT = ".txt"
    DOT_G = ".g"
    DOT_F_G = "_f.g"

    @staticmethod
    def get_root_dir_path():
        return os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

    @staticmethod
    def get_library_dir_path():
        return os.path.join(CoreUtils.get_root_dir_path(), "library")

    @staticmethod
    def get_exp_data_dir_path():
        return os.path.join(CoreUtils.get_library_dir_path(), "exp_data")

    @staticmethod
    def get_exp_fin_files_dir_path():
        return os.path.join(CoreUtils.get_exp_data_dir_path(), "fin_files")

    @staticmethod
    def get_init_files_dir_path():
        return os.path.join(CoreUtils.get_library_dir_path(), "init_files")

    @staticmethod
    def get_fin_files_dir_path():
        return os.path.join(CoreUtils.get_library_dir_path(), "fin_files")

    @staticmethod
    def get_test_files_data_dir_path():
        return os.path.join(CoreUtils.get_exp_data_dir_path(), "test_files")
