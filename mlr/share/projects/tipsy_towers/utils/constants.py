import os


class NameUtils:
    NAME_RED = "red"
    NAME_YELLOW = "yellow"
    NAME_BLUE = "blue"
    NAME_MAUVE = "mauve"
    NAME_EMERALD = "emerald"

    NAME_DIFF_B = "b"
    NAME_DIFF_C = "c"

    TEST_STACK_BLOCKS = ["b0", "b1", "b2", "b3", "b4", "c0", "c1", "c2", "c3", "c4"]


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
    def get_test_files_data_dir_path():
        return os.path.join(CoreUtils.get_exp_data_dir_path(), "test_files")
